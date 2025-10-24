/*
 * =====================================================================================
 *
 *       Filename:  Kafka.cpp
 *
 *    Description:  Kafka
 *
 *        Version:  1.0
 *        Created:  2024年03月01日 15时08分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  vyouzhi (vz), vyouzhi@gmail.com
 *   Organization:  Etomc2.com
 *        LICENSE:  BSD-3-Clause license
 *
 *  Copyright (c) 2019-2022, vyouzhi
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *  notice, this list of conditions and the following disclaimer in the
 *  documentation and/or other materials provided with the distribution.
 *  * Neither the name of vyouzhi and/or the DataFrame nor the
 *  names of its contributors may be used to endorse or promote products
 *  derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL vyouzhi BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * =====================================================================================
 */

#include "libs/kafka/Consumer.hpp"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "OMSPack/SessionGlobal.hpp"
#include "Toolkit/GlobalConfig.hpp"
#include "Toolkit/Norm.hpp"
#include "Toolkit/Util.hpp"
#include "Toolkit/UtilTime.hpp"
#include "Toolkit/eLog.hpp"
#include "assembler/BaseType.hpp"
#include "libs/DB/pg.hpp"
#include "libs/kafka/protocol/nbo.hpp"
#include "libs/kafka/protocol/proto.hpp"
#include "quickfix/FixFields.h"
#include "quickfix/FixValues.h"
#include "quickfix/fix44/MessageCracker.h"
#include "quickfix/fix44/Quote.h"
#include "quickfix/fix44/QuoteStatusReport.h"

namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KfConsumeCb::SymbolIntP
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void KfConsumeCb::SymbolInit(const char* p, int sz)
{
    std::size_t idx = 0;

    if (FinFabr->_offer_time > 0) {
        // 已初始了，有变动，就使用 MarketIng
        return;
    }

    SystemInitMessage sinit;
    std::size_t mlen = 1;  // len(msgtype + aligned)
    std::size_t stock_len = fldsiz(SystemInitMessage, Stock);
    mlen += stock_len;
    mlen += fldsiz(SystemInitMessage, CfiCode);
    mlen += fldsiz(SystemInitMessage, Itype);
    mlen += fldsiz(SystemInitMessage, OfferTime);
    mlen += fldsiz(SystemInitMessage, unix_time);

    if (sz != (int)(mlen - 1)) {
        std::string err = elog::format("sz:%d  mlen:%ld \n", sz, (mlen - 1));

        elog::bug(err);
        return;
    }

    stock_len--;
    memcpy(&sinit.Stock, p, stock_len);
    idx += stock_len;

    idx += parse_uint_t(p + idx, sinit.CfiCode);

    // 加上 e2q
    if (sinit.CfiCode > 0) {
        sinit.CfiCode += E2QCfiStart;
    }
    sinit.Itype = *(p + idx);
    idx++;

    idx += parse_uint_t(p + idx, sinit.OfferTime);
    idx += parse_uint_t(p + idx, sinit.unix_time);

    sinit.Aligned = *(p + idx);

    std::string symbol = std::string(sinit.Stock);
    FixSymbolType info{symbol, DoIAction::LIST, OnlyEA::FORANLYONE};
    FinFabr->_fix_symbols.insert({sinit.CfiCode, info});

    std::size_t gidx = GlobalDBPtr->getId();
    Pgsql* gsql = GlobalDBPtr->ptr(gidx);
    std::string table = "public.";
    if (gsql != nullptr) {
        gsql->public_table(table);
        gsql->insert_table("stockinfo");
        gsql->insert_field("symbol", sinit.CfiCode);
        gsql->insert_field("stock", sinit.Stock);
        gsql->insert_field("verid", FinFabr->_QuantVerId);
        gsql->insert_field("ctime", sinit.unix_time);
        InsertCommit(gsql);
    }

    GlobalDBPtr->release(gidx);

    if (sinit.Aligned == Aligned_t::PULL) {
        elog::info("SymbolInit ok symbols size:", FinFabr->_fix_symbols.size());
        //  转成豪秒
        FinFabr->_offer_time = NUMBERVAL(sinit.OfferTime) * 1000;

        /**
         * 给目前已登录的，如果后来登录的就下次优化再说吧
         */
        for (auto it = SessionSymList.begin(); it != SessionSymList.end();
             it++) {
            // FixGuard
            MassQuote(it->first);
        }
    }

} /* -----  end of function KfConsumeCb::SymbolIntP  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KfConsumeCb::MarketIng
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void KfConsumeCb::MarketIng(const char* p, int sz)
{
    std::size_t idx = 0;
    MarketDelOrIPOMessage mdoi;
    std::size_t mlen = 1;
    std::size_t stock_len = fldsiz(MarketDelOrIPOMessage, Stock);
    mlen += stock_len;
    mlen += fldsiz(MarketDelOrIPOMessage, action);
    mlen += fldsiz(MarketDelOrIPOMessage, CfiCode);
    mlen += fldsiz(MarketDelOrIPOMessage, unix_time);
    mlen += fldsiz(MarketDelOrIPOMessage, count_down);

    if (sz != (int)(mlen - 1)) {
        std::string err = elog::format("sz:%d  mlen:%ld \n", sz, (mlen - 1));
        elog::bug(err);
        return;
    }

    mdoi.action = *(p + idx);
    idx++;

    stock_len--;
    memcpy(&mdoi.Stock, p + idx, stock_len);
    idx += stock_len;

    idx += parse_uint_t(p + idx, mdoi.CfiCode);

    mdoi.CfiCode += E2QCfiStart;

    idx += parse_uint_t(p + idx, mdoi.unix_time);

    idx += parse_uint_t(p + idx, mdoi.count_down);

    //  std::uint32_t del_code = 0;
    std::string symbol = std::string(mdoi.Stock);

    if (mdoi.action == DoIAction::LIST) {
        if (FinFabr->_fix_symbols.count(mdoi.CfiCode) == 0) {
            FixSymbolType info{symbol, DoIAction::LIST, OnlyEA::FORANLYONE};
            FinFabr->_fix_symbols.insert({mdoi.CfiCode, info});

            std::size_t gidx = GlobalDBPtr->getId();
            Pgsql* gsql = GlobalDBPtr->ptr(gidx);
            std::string table = "public.";
            if (gsql != nullptr) {
                gsql->public_table(table);
                gsql->insert_table("stockinfo");
                gsql->insert_field("symbol", mdoi.CfiCode);
                gsql->insert_field("stock", mdoi.Stock);
                gsql->insert_field("verid", FinFabr->_QuantVerId);
                gsql->insert_field("ctime", (mdoi.unix_time / 1000));
                InsertCommit(gsql);
            }

            GlobalDBPtr->release(gidx);
        }
        else {
            // 已存在不没必要了
            // 可能需要重新激活一下吧
            elog::info("CfiCode exist:", mdoi.CfiCode);
            return;
        }
    }

    if (mdoi.action == DoIAction::DELISTING) {
        //  del_code = 1;

        if (FinFabr->_fix_symbols.count(mdoi.CfiCode) == 1) {
            FinFabr->_fix_symbols[mdoi.CfiCode].dia = DoIAction::DELISTING;
            FinFabr->_fix_symbols[mdoi.CfiCode].unix_time = mdoi.unix_time;
        }
        else {
            elog::bug("bug cficode:", mdoi.CfiCode);
            // 不存在不没必要了
            return;
        }
    }
    // if (FinFabr->_fix_symbol_only_for_ea == OnlyEA::LOCKFOREA) {
    // 这种情况下，只发有这个 code 的 session
    // }
    // else {
    for (auto it = SessionSymList.begin(); it != SessionSymList.end(); it++) {
        if (mdoi.action == DoIAction::LIST) {
            if (it->second.empty()) {
                // 这个情况可能是新的账号在上市新股票前登录的
                // 可能需要用 MassQuote 这个，否则 EA 不能初始化的
                MassQuote(it->first);
            }
            else {
                // 这个情况，是对现在的账号处理通告
                Quote(it->first, mdoi);
            }
        }
        if (mdoi.action == DoIAction::DELISTING) {
            if (std::find(std::begin(it->second), std::end(it->second),
                          mdoi.CfiCode) != std::end(it->second)) {
                // 如果是退市，就只发给当前的一个sessionid
                // elog::echo("delisting:", mdoi.CfiCode,
                //            " name:", it->first.getTargetCompID().getValue());
                Quote(it->first, mdoi);
            }
        }
    }
    // }

} /* -----  end of function KfConsumeCb::MarketIng  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KfConsumeCb::Quote
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void KfConsumeCb::Quote(const FIX::SessionID& session,
                        MarketDelOrIPOMessage mdoi)
{
    // elog::info("Quote");
    std::string cstr = "";

    std::string sym = std::string(mdoi.Stock);

    FIX::QuoteID id;
    id.setValue(elog::format("%ld", mdoi.unix_time));

    FIX44::Quote quote(id);

    FIX::Symbol symbol = FIX::Symbol(sym);
    quote.setField(symbol);

    FIX::QuoteType qt = FIX::QuoteType_INDICATIVE;

    if (mdoi.action == DoIAction::LIST) {
        //        DoIAction::LIST
        qt = FIX::QuoteType_TRADEABLE;
    }

    quote.setField(qt);

    cstr = elog::format("%d", mdoi.CfiCode);
    FIX::QuoteReqID rid = FIX::QuoteReqID(cstr);
    quote.setField(rid);

    FIX::OfferSize os = FIX::OfferSize(mdoi.count_down);
    quote.setField(os);

    try {
        FIX::Session::sendToTarget(quote, session);
    }
    catch (FIX::SessionNotFound& e) {
        elog::bug(e.what());
    }
} /* -----  end of function KfConsumeCb::Quote  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KfConsumeCb::SymbolExrd
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void KfConsumeCb::SymbolExrd(const char* p, int sz)
{
    std::size_t idx = 0;
    StockAXdxrMessage saxm;
    std::size_t mlen = 1;
    mlen += fldsiz(StockAXdxrMessage, CfiCode);
    mlen += fldsiz(StockAXdxrMessage, year);
    mlen += fldsiz(StockAXdxrMessage, month);
    mlen += fldsiz(StockAXdxrMessage, day);
    mlen += fldsiz(StockAXdxrMessage, category);
    mlen += fldsiz(StockAXdxrMessage, fenhong);
    mlen += fldsiz(StockAXdxrMessage, songzhuangu);
    mlen += fldsiz(StockAXdxrMessage, outstandend);
    mlen += fldsiz(StockAXdxrMessage, outstanding);
    mlen += fldsiz(StockAXdxrMessage, mrketCaping);
    mlen += fldsiz(StockAXdxrMessage, uint);

    if (sz != (int)mlen) {
        std::string err = elog::format("sz:%d  mlen:%ld \n", sz, mlen);

        elog::bug(err);
        return;
    }

    idx += parse_uint_t(p + idx, saxm.CfiCode);
    if (saxm.CfiCode > 0) {
        saxm.CfiCode += E2QCfiStart;
    }

    idx += parse_uint_t(p + idx, saxm.year);
    idx += parse_uint_t(p + idx, saxm.month);
    idx += parse_uint_t(p + idx, saxm.day);
    idx += parse_uint_t(p + idx, saxm.category);
    idx += parse_uint_t(p + idx, saxm.fenhong);
    idx += parse_uint_t(p + idx, saxm.songzhuangu);
    idx += parse_uint_t(p + idx, saxm.outstanding);
    idx += parse_uint_t(p + idx, saxm.outstandend);
    idx += parse_uint_t(p + idx, saxm.mrketCaping);
    idx += parse_uint_t(p + idx, saxm.uint);

    saxm.Aligned = *(p + idx);

    std::string ymd =
        elog::format("%d%02d%02d", saxm.year, saxm.month, saxm.day);

    ExRD node;
    node._ymd = atoll(ymd.c_str());
    node._extype = (ExType)saxm.category;

    saxm.fenhong /= saxm.uint;
    saxm.songzhuangu /= saxm.uint;

    double cash = NUMBERVAL(saxm.fenhong);
    double shares = NUMBERVAL(saxm.songzhuangu);

    node._cash = saxm.fenhong;
    // 因为是百分比
    node._share = saxm.songzhuangu;

    SeqType cfiCode = VALNUMBER(saxm.CfiCode);

    // e2l script broker.cpp 计算 adj 使用的
    if (FinFabr->_exrd.count(cfiCode) == 0) {
        std::vector<ExRD> nodes;
        nodes.push_back(node);
        FinFabr->_exrd.insert({cfiCode, nodes});
    }
    else {
        ExRD c_node = FinFabr->_exrd[cfiCode].back();

        if (c_node._ymd > node._ymd) {
#ifdef DEBUG
            elog::info("exist:", c_node._ymd, " now ymd:", node._ymd);
#endif
            return;
        }
        if (c_node._ymd == node._ymd && c_node._extype == node._extype) {
            // 小于日是线级别的时候，可能会重复传送这个数据
            // 在这儿过滤一下
#ifdef DEBUG
            elog::bug("exist:", c_node._ymd, " now ymd:", node._ymd);
#endif
            return;
        }
        FinFabr->_exrd.at(cfiCode).push_back(node);
    }

    std::size_t gidx = GlobalDBPtr->getId();
    Pgsql* gsql = GlobalDBPtr->ptr(gidx);
    std::string cfi_str = elog::format(
        "(SELECT id FROM stockinfo WHERE symbol=%d and verid=%d ORDER BY id "
        "DESC LIMIT "
        "1 )",
        saxm.CfiCode, FinFabr->_QuantVerId);
    if (gsql != nullptr) {
        gsql->insert_table("exdr");
        gsql->insert_field("symbol", cfi_str);
        gsql->insert_field("cash", cash);
        gsql->insert_field("shares", shares);
        gsql->insert_field("extype", (int)node._extype);
        gsql->insert_field("ymd", node._ymd);
        gsql->insert_field("outstanding", NUMBERVAL(node._mshare._Outstanding));
        gsql->insert_field("outstandend", NUMBERVAL(node._mshare._Outstanding));
        gsql->insert_field("marketcaping",
                           NUMBERVAL(node._mshare._MarketCaping));
        gsql->insert_field("marketcapend",
                           NUMBERVAL(node._mshare._MarketCapend));
        gsql->insert_return("id");
        InsertCommit(gsql);
    }
    std::vector<long> tickets;
    std::string stock = "";
    if (FinFabr->_fix_symbols.count(saxm.CfiCode) == 1) {
        stock = FinFabr->_fix_symbols.at(saxm.CfiCode).symbol;
    }
    // 现金收益，add trade_report
    if (stock.length() > 0 && (cash > 0 || shares > 0)) {
        std::string ymd = std::to_string(node._ymd);
        const char fmt[] = "%Y%m%d";
        UtilTime ut;
        std::size_t exrd_time = ut.strtostamp(ymd, fmt);

        char* field = nullptr;
        char* val = nullptr;

        std::string sql = elog::format(
            "SELECT ticket FROM trades WHERE symbol = (SELECT id FROM "
            "stockinfo "
            "WHERE symbol =%d AND verid=%d ORDER BY id DESC LIMIT 1)  AND  "
            "side = 1  "
            "AND "
            "stat = 2 AND ticket not IN (SELECT closetck FROM trades) "
            "ORDER "
            "BY id; ",
            saxm.CfiCode, FinFabr->_QuantVerId);

        bool r = SelectSQL(gsql, sql);

        std::string ret = "";
        if (r && gsql->tuple_size() > 0) {
            long ticket = 0;

            for (gsql->begin(); gsql->end(); gsql->next()) {
                int r = gsql->PGResult(&field, &val);
                if (r == -1) {
                    continue;
                }

                if (field != nullptr) {
                    ticket = std::stol(val);
                    tickets.push_back(ticket);
                    GlobalMatcher->ExdrChange(saxm.CfiCode, ticket, cash,
                                              shares, exrd_time);
                }
            }
        }
    }
    GlobalDBPtr->release(gidx);

    // 分红的时候，当前卖出去也有，或者以后再优化
    for (auto it = SessionSymList.begin(); it != SessionSymList.end(); it++) {
        QuoteStatusReport(it->first, node, saxm.CfiCode, tickets);
    }

    // 在这儿也可以设置一下
    e2q::FinFabr->_enable_exrd = e2::Bool::B_TRUE;
} /* -----  end of function KfConsumeCb::SymbolExrd  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KfConsumeCb::StopOrder
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  ','  size==1
 * ============================================
 */
void KfConsumeCb::StopOrder()
{
    // 晚一点再发布，免得还有事情没有处理完成
    TSleep(FinFabr->_offer_time);
    FinFabr->_StopOrder = true;

    FIX::ClOrdID cl0id;
    cl0id.setValue("1");
    FIX::Side side = convert(e2::Side::os_Buy);

    FIX44::OrderStatusRequest osr(cl0id, side);

    for (auto it = SessionSymList.begin(); it != SessionSymList.end(); it++) {
        try {
            FIX::Session::sendToTarget(osr, it->first);
        }
        catch (FIX::FieldNotFound& f) {
            elog::bug(f.what(), " field:", f.field);
            // elog::echo(msg.toXML());
        }
        catch (FIX::SessionNotFound& e) {
            elog::bug(e.what());
        }
    }
} /* -----  end of function KfConsumeCb::StopOrder  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KfConsumeCb::ExitOrder
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void KfConsumeCb::ExitOrder()
{
    _kafka_run = 0;
} /* -----  end of function KfConsumeCb::ExitOrder  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KfConsumeCb::Header
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void KfConsumeCb::Header(const RdKafka::Headers* headers)
{
    //  headers = message->headers();
    if (headers) {
        std::vector<RdKafka::Headers::Header> hdrs = headers->get_all();
        for (size_t i = 0; i < hdrs.size(); i++) {
            const RdKafka::Headers::Header hdr = hdrs[i];

            if (hdr.value() != NULL) {
                elog::bug(" Header: %s ", hdr.key().c_str(), "=",
                          (int)hdr.value_size(), ".", (const char*)hdr.value());
            }
            else {
                elog::bug(" Header:  ", hdr.key().c_str(), "= NULL");
            }
        }
    }
} /* -----  end of function KfConsumeCb::Header  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KfConsumeCb::Events
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void KfConsumeCb::Events(const char* p, int sz, int64_t now_offset)
{
    switch (*p) {
        case e2l_pro_t::INIT:
            SymbolInit(p + 1, sz);
            break;
        case e2l_pro_t::XDXR:
            SymbolExrd(p + 1, sz);
            break;
        case e2l_pro_t::TICK:
            TicketMsg(p + 1, sz, _lastoffset);
            break;
        case e2l_pro_t::SUSPEND:
            StopOrder();
            break;
        case e2l_pro_t::MARKETING:
            MarketIng(p + 1, sz);
            break;
        case e2l_pro_t::CUSTOM:
            CustomMsg(p + 1, sz, _lastoffset);
            break;
        case e2l_pro_t::EXIT:
            ExitOrder();
            break;
        default:
            std::string error = elog::format("%s sz:%d\n", p, sz);
            elog::bug("bad data! error:", error);
            break;
    }
} /* -----  end of function KfConsumeCb::Events  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KfConsumeCb::QuoteStatusReport
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void KfConsumeCb::QuoteStatusReport(const FIX::SessionID& session, ExRD node,
                                    std::size_t symbol,
                                    std::vector<long> tickets)
{
    FIN_FABR_IS_NULL();
    FIX44::QuoteStatusReport qsr =
        FIX44::QuoteStatusReport(FIX::QuoteID(UUidGen()));

    FIX::Symbol symobl;
    std::string codeId = std::to_string(symbol);
    symobl.setValue(codeId);
    qsr.setField(symobl);

    FIX::MaturityDate mdate;
    std::string day = std::to_string(node._ymd);
    mdate.setValue(day);
    qsr.setField(mdate);

    FIX::StrikePrice cash;
    cash.setValue(node._cash);
    qsr.setField(cash);

    FIX::MktBidPx share;
    share.setValue(node._share);
    qsr.setField(share);

    FIX::BidSize btype;
    btype.setValue(node._extype);
    qsr.setField(btype);

    for (auto it : tickets) {
        FIX44::QuoteStatusReport::NoPartyIDs relateGroup;
        FIX::PartyID bqid;
        bqid.setValue(std::to_string(it));
        relateGroup.setField(bqid);
        qsr.addGroup(relateGroup);
    }

    try {
        FIX::Session::sendToTarget(qsr, session);
    }
    catch (FIX::FieldNotFound& f) {
        elog::bug(f.what(), " field:", f.field);
        // elog::echo(msg.toXML());
    }
    catch (FIX::SessionNotFound& e) {
        elog::bug(e.what());
    }
} /* -----  end of function KfConsumeCb::QuoteStatusReport ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KfConsumeCb::CustomMsg
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void KfConsumeCb::CustomMsg(const char* ptr, int sz, int64_t moffset)
{
    PushGCM(ptr, sz);

    FixGuard fix;
    Base64 base64;
    std::string msg = base64.b64encode(ptr, sz);

    for (auto it = SessionSymList.begin(); it != SessionSymList.end(); it++) {
        fix.CustemRequest(it->first, msg, sz);
    }

} /* -----  end of function KfConsumeCb::CustomMsg  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KfConsumeCb::TicketMsg
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void KfConsumeCb::TicketMsg(const char* ptr, int sz, int64_t moffset)
{
    MarketTickMessage mtm;
    mtm.mtm(ptr, sz);

    // if (mtm.Aligned == Aligned_t::PULL) {
    //     if (_lastTime == mtm.unix_time) {

    //     }
    // }
    if (_lastTime > mtm.unix_time) {
        elog::bug("bug tick: lastTime:", _lastTime,
                  " _unix_time:", mtm.unix_time);
#ifdef DEBUG
        logs(_call_data, moffset);
#endif
        return;
    }
    else {
        _lastTime = mtm.unix_time;
    }

    _call_data[Trading::t_time] = mtm.unix_time;
    _call_data[Trading::t_frame] = mtm.frame;
    _call_data[Trading::t_side] = e2::Side::os_Buy;
    if (mtm.side != 'B') {
        _call_data[Trading::t_side] = e2::Side::os_Sell;
    }

    _call_data[Trading::t_qty] = mtm.qty;
    _call_data[Trading::t_price] = mtm.price;
    _call_data[Trading::t_adjprice] = mtm.price;
    _call_data[Trading::t_msg] = mtm.number;
    _call_data[Trading::t_stock] = mtm.CfiCode;

    if (_TunCall != nullptr) {
        _TunCall(_call_data);
    }

} /* -----  end of function KfConsumeCb::TicketMsg  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KfConsumeCb::logs
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void KfConsumeCb::logs(std::array<SeqType, trading_protocols> trad_data,
                       int64_t moffset)
{
    UtilTime ut;
    const char* fmt = "%Y-%m-%d %H:%M";

    bprinter::TablePrinter tp(&std::cout);
    tp.AddColumn("time", 20);
    tp.AddColumn("frame", 10);
    tp.AddColumn("side", 10);
    tp.AddColumn("qty", 10);
    tp.AddColumn("price", 10);
    tp.AddColumn("msg", 10);
    tp.AddColumn("stock", 10);
    tp.AddColumn("offset", 10);

    tp.PrintHeader();
    tp << ut.millitostr(trad_data[Trading::t_time], fmt)
       << trad_data[Trading::t_frame] << trad_data[Trading::t_side]
       << trad_data[Trading::t_qty] << trad_data[Trading::t_price]
       << trad_data[Trading::t_msg] << trad_data[Trading::t_stock] << moffset;
    tp.PrintFooter();

} /* -----  end of function KfConsumeCb::logs  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  KafkaFeed::handle
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void KafkaFeed::handle(TradType tradcall)
{
    if (tradcall == nullptr) {
        elog::bug("TradType is nullptr");
        return;
    }
    if (_bokers.length() == 0 || _topic.length() == 0) {
        elog::bug("bokers or topic is empty");
        return;
    }

    std::string errstr;
    int32_t partition = RdKafka::Topic::PARTITION_UA;
    /**
     * kafka-topics.sh --describe	Partition: 0
     *
     */
    partition = 0;
    // --from-beginning
    int64_t start_offset = RdKafka::Topic::OFFSET_END;

    int use_ccb = 1;

    /*
     * Create configuration objects
     */
    RdKafka::Conf* conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    if (!conf) {
        elog::bug("conf is null");
        return;
    }
    RdKafka::Conf* tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    if (!tconf) {
        delete conf;
        elog::bug("tconf");
        return;
    }
    /*
     * Set configuration properties
     */
    conf->set("metadata.broker.list", _bokers, errstr);

    KfEventCb ex_event_cb;
    ex_event_cb._bokers = _bokers;
    ex_event_cb._topic = _topic;
    conf->set("event_cb", &ex_event_cb, errstr);

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    /*
     * Consumer mode
     */

    conf->set("enable.partition.eof", "true", errstr);

    /*
     * Create consumer using accumulated global configuration.
     */
    RdKafka::Consumer* consumer = RdKafka::Consumer::create(conf, errstr);
    if (!consumer) {
        elog::bug("Failed to create consumer: ", errstr);
        delete conf;
        delete tconf;
        exit(1);
    }

    /*
     * Create topic handle.
     */
    RdKafka::Topic* topic =
        RdKafka::Topic::create(consumer, _topic, tconf, errstr);
    if (!topic) {
        elog::bug("Failed to create topic: ", errstr);

        delete consumer;
        delete tconf;
        delete conf;

        exit(1);
    }

    /*
     * Start consumer for topic+partition at start offset
     */
    RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);
    if (resp != RdKafka::ERR_NO_ERROR) {
        elog::bug("Failed to start consumer: ", RdKafka::err2str(resp));
        delete topic;
        delete consumer;
        delete tconf;
        delete conf;

        exit(1);
    }
    //    std::size_t kcount = 1;

    KfConsumeCb ex_consume_cb;
    ex_consume_cb.handle(tradcall);

    elog::echo(consumer->name(), " Created consumer ");

    /*
     * Consume messages
     */
    int timeout_ms = 1000;
    while (_kafka_run) {
        consumer->consume_callback(topic, partition, timeout_ms, &ex_consume_cb,
                                   &use_ccb);
        consumer->poll(0);
        // RdKafka::Message *msg = consumer->consume(topic, partition,
        // timeout_ms); ex_consume_cb.msg_consume(msg, nullptr); delete msg;
        // consumer->poll(FinFabr->_offer_time);
    }

    /*
     * Stop consumer
     */

    consumer->stop(topic, partition);

    consumer->poll(FinFabr->_offer_time);

    delete topic;
    delete consumer;

    delete conf;
    delete tconf;

    /*
     * Wait for RdKafka to decommission.
     * This is not strictly needed (when check outq_len() above), but
     * allows RdKafka to clean up all its resources before the application
     * exits so that memory profilers such as valgrind wont complain about
     * memory leaks.
     */
    RdKafka::wait_destroyed(5000);

} /* -----  end of function KafkaFeed::handle  ----- */
}  // namespace e2q
