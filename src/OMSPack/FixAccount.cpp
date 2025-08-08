/*
 * =====================================================================================
 *
 *       Filename:  FixAccount.cpp
 *
 *    Description:  FixAccount
 *
 *        Version:  1.0
 *        Created:  2024年01月16日 09时07分06秒
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
#include "OMSPack/FixAccount.hpp"

#include <unistd.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "Toolkit/GlobalConfig.hpp"
#include "Toolkit/Norm.hpp"
#include "assembler/BaseType.hpp"
#include "quickfix/FixFields.h"
#include "quickfix/FixValues.h"
#include "quickfix/fix44/BidResponse.h"
#include "quickfix/fix44/MessageCracker.h"
#include "utility/Log.hpp"

namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::FixAccount
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
FixAccount::FixAccount() {
} /* -----  end of function FixAccount::FixAccount  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::onMessage(const FIX44::Heartbeat& message,
                           const FIX::SessionID&)
{
    log::echo("2");

} /* -----  end of function FixAccount::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::onMessage(const FIX44::Quote& message, const FIX::SessionID&)
{
    FIX::QuoteID id;
    message.getField(id);

    FIX44::Quote::NoEvents event;
    int i = message.groupCount(event.field());
    log::echo("i:", i);

    for (int m = 1; m <= i; m++) {
        message.getGroup(m, event);
        if (event.isEmpty()) {
            log::echo("empty");
        }
        else {
            FIX::Symbol symbol;

            event.getField(symbol);

            log::echo("symbol:", symbol);
        }
    }
    log::echo(message.toXML());
    log::echo(message.toString());

} /* -----  end of function FixAccount::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::onMessage(const FIX44::QuoteCancel&, const FIX::SessionID&)
{
    _fq.quit();

} /* -----  end of function FixAccount::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  kafka to MassQuote
 * ============================================
 */
void FixAccount::onMessage(const FIX44::MassQuote& msg, const FIX::SessionID&)
{
    FIX_PTR_IS_NULL();
    // log::echo(msg.toXML());
    FIX44::MassQuote::NoQuoteSets nqs;
    FIX44::MassQuote::NoQuoteSets::NoQuoteEntries pid;
    FIX44::MassQuote::NoQuoteSets::NoQuoteEntries::NoEvents pdate;

    FIX::Symbol symbol;
    FIX::DefOfferSize offsize;
    FIX::QuoteEntryID qeid;
    FIX::UnderlyingQty qty;

    msg.getFieldIfSet(offsize);
    // AnalyBeam 在这儿使用的
    FixPtr->_offer_time = offsize.getValue();

    // 保证金率
    FIX::DefBidSize dbs;
    msg.getFieldIfSet(dbs);
    FixPtr->_margin_rate = dbs.getValue();

    int count = 0, pids = 0, num;
    if (!msg.hasGroup(nqs)) {
        return;
    }

    count = msg.groupCount(nqs.field());
    FIX::EventDate edate;
    std::string dates = "";

    // 如果不存在，就在这儿加上吧
    std::size_t number_symbols = FixPtr->_symbols.size();

    for (int m = 1; m <= count; m++) {
        msg.getGroup(m, nqs);

        bool b = nqs.getFieldIfSet(qty);
        if (b) {
            FixPtr->_lot_and_share = qty.getValue();
        }
        if (!nqs.hasGroup(pid)) {
            continue;
        }
        pids = nqs.groupCount(pid.field());

        for (int n = 1; n <= pids; n++) {
            nqs.getGroup(n, pid);
            pid.getFieldIfSet(symbol);
            pid.getFieldIfSet(qeid);

            std::size_t cfi_code = atoll(qeid.getValue().c_str());

            FixPtr->_fix_symbols.insert({cfi_code, symbol.getValue()});

            if (number_symbols == 0 && cfi_code > 0) {
                if (std::find(FixPtr->_symbols.begin(), FixPtr->_symbols.end(),
                              cfi_code) == FixPtr->_symbols.end()) {
                    FixPtr->_symbols.push_back(cfi_code);
                }
            }

            num = pid.groupCount(pdate.field());
            TradeTime tt;

            for (int m = 1; m <= num; m++) {
                pid.getGroup(m, pdate);
                pdate.getField(edate);
                dates = edate.getValue();
                for (std::size_t l = 0; l < dates.length(); l += 2) {
                    int ll = atoi(dates.substr(l, 2).c_str());
                    //    log::echo(ll);
                    if (l == 0) {
                        tt.open_hour = ll;
                    }
                    else if (l == 2) {
                        tt.open_min = ll;
                    }
                    else if (l == 4) {
                        tt.close_hour = ll;
                    }
                    else {
                        tt.close_min = ll;
                    }
                }

                FixPtr->_tradetime.push_back(tt);
            }
        }
    }

    if (number_symbols == 0) {
        // 动态才需要再处理一次
        QuoteRequest(FixPtr->_symbols);
    }

    ContainerStashSharePtr _cnt_ptr = nullptr;
    if (_source_ptr != nullptr) {
        size_t type = typeid(ContainerStash).hash_code();

        _cnt_ptr = _source_ptr->fetch<ContainerStash>(type);
    }
    if (_cnt_ptr == nullptr) {
        log::bug("OHLCBeam ContainerStash ptr is nullptr!");
        return;
    }

    _cnt_ptr->data_ptr->InitCell();

} /* -----  end of function FixAccount::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::onMessage(const FIX44::MarketDataSnapshotFullRefresh& message,
                           const FIX::SessionID&)
{
    // log::echo("Maret refresh");
    FIX::MDEntryPx px;
    FIX::Factor fact;
    FIX::MDEntryTime time;
    FIX::MDEntryType side;
    FIX::MDEntrySize qty;
    FIX::Symbol symbol;
    FIX::MDReqID mid;
    FIX::PriceDelta adjpx;

    std::array<SeqType, trading_protocols> data;
    FIX44::MarketDataSnapshotFullRefresh::NoMDEntries md;
    if (message.hasGroup(md)) {
        try {
            int num = message.groupCount(md.field());
            message.getField(symbol);

            message.getField(mid);
            message.getField(fact);

            for (int m = 1; m <= num; m++) {
                message.getGroup(m, md);

                md.getField(px);
                md.getField(time);
                md.getField(side);
                md.getField(qty);
                md.getField(adjpx);

                data[Trading::t_time] =
                    (SeqType)atoll(time.getString().c_str());
                data[Trading::t_frame] =
                    (SeqType)atoll(fact.getString().c_str());

                data[Trading::t_side] =
                    (SeqType)atoll(side.getString().c_str());
                data[Trading::t_qty] = (SeqType)atoll(qty.getString().c_str());
                data[Trading::t_price] = (SeqType)atoll(px.getString().c_str());
                data[Trading::t_adjprice] =
                    (SeqType)atoll(adjpx.getString().c_str());

                data[Trading::t_msg] = (SeqType)atoll(mid.getString().c_str());
                data[Trading::t_stock] =
                    (SeqType)atoll(symbol.getString().c_str());

                _fq.handle(data);
            }
        }
        catch (FIX::FieldNotFound& f) {
            log::bug(f.what(), " field:", f.field);
        }
        catch (std::exception& e) {
            log::bug(e.what());
        }
    }
    else {
        log::echo("not md");
    }
} /* -----  end of function FixAccount::onMessage  ----- */

/*
 * ===  FUNCTION  fsafdsa
 *
 *         Name:  FixAccount::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::onMessage(const FIX44::MarketDataRequestReject& message,
                           const FIX::SessionID&)
{
    log::echo("11");

} /* -----  end of function FixAccount::onMessage  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::onMessage(const FIX44::QuoteStatusReport& message,
                           const FIX::SessionID&)
{
    // log::echo(message.toXML());
    FIX::Symbol symobl;
    FIX::MaturityDate ymd;
    FIX::StrikePrice cash;
    FIX::MktBidPx share;
    FIX::BidSize etype;
    FIX44::QuoteStatusReport::NoPartyIDs relateGroup;

    message.getFieldIfSet(symobl);
    message.getFieldIfSet(ymd);
    message.getFieldIfSet(cash);
    message.getFieldIfSet(share);
    message.getFieldIfSet(etype);

    exdi_type et;
    et._ymd = atoll(ymd.getValue().c_str());
    et._cash = cash.getValue();

    et._share = share.getValue();
    et._extype = (ExType)etype.getValue();
    SeqType sym = atoll(symobl.getValue().c_str());

    ExdiSymList.add(sym, et);

    int count = message.groupCount(relateGroup.field());
    FIX::PartyID pid;
    std::vector<long> ticks;

    char* field = nullptr;
    char* val = nullptr;
    std::string sql = "SELECT id FROM trade_info WHERE active=1 LIMIT 1;";
    std::size_t idx = e2q::GlobalDBPtr->getId();
    e2q::Pgsql* gsql = e2q::GlobalDBPtr->ptr(idx);
    if (gsql != nullptr) {
        bool r = gsql->select_sql(sql);

        if (r && gsql->tuple_size() > 0) {
            gsql->OneHead(&field, &val);
            if (val != nullptr) {
                e2q::FixPtr->_QuantVerId = stoi(val);
            }
        }
        else {
            log::info("trade info is empty");
        }
    }
    e2q::GlobalDBPtr->release(idx);

    if (count == 0) {
        return;
    }
    for (int m = 1; m <= count; m++) {
        message.getGroup(m, relateGroup);
        relateGroup.getField(pid);
        long tick = atol(pid.getValue().c_str());
        ticks.push_back(tick);
    }
    e2::Int_e quantId = 0;
    std::string cl0id = "";
    for (auto it : FixPtr->_OrderIds) {
        for (auto oi : it.second) {
            if (oi.second.side == e2::Side::os_Sell ||
                oi.second.symbol != sym) {
                continue;
            }
            auto result1 =
                std::find(std::begin(ticks), std::end(ticks), oi.second.ticket);

            if (result1 == std::end(ticks)) {
                continue;
            }
            if (oi.second.trading == TradeStatus::MARKET) {
                // 分红的时候
                cl0id = oi.first;
                long qty = oi.second.qty;
                double all_cash = FixPtr->exdr_cash(qty, NUMBERVAL(et._cash));

                std::size_t thread_number = 0;
                if (FixPtr->_cash._tsize > 0) {
                    if (FixPtr->_cash.cl_thread.count(cl0id) > 0) {
                        thread_number = FixPtr->_cash.cl_thread.at(cl0id);
                    }
                    else {
                        log::bug("error cl0id:", cl0id,
                                 " ticket:", oi.second.ticket);
                    }
                }
                FixPtr->_cash.append(thread_number, all_cash);

                quantId = it.first;

                FixPtr->_OrderIds[quantId].at(cl0id).qty =
                    FixPtr->_OrderIds[quantId].at(cl0id).openqty =
                        qty * (1.0 + NUMBERVAL(et._share));
            }
        }
    }

} /* -----  end of function FixAccount::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::onMessage(const FIX44::QuoteResponse& message,
                           const FIX::SessionID&)
{
    // log::info(message.toXML());
    FIX::MidPx px;
    message.getFieldIfSet(px);

    double cash = px.getValue();
    FixPtr->_cash.total_cash = cash;

    double sub_cash = 0;
    for (std::size_t m = 0; m < FixPtr->_cash._tsize; m++) {
        sub_cash = cash * FixPtr->_cash._thread_pos.at(m)._postion;
        FixPtr->_cash.add(m, sub_cash);

        FixPtr->_cash.total_cash -= sub_cash;
    }
} /* -----  end of function FixAccount::onMessage  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::onMessage(const FIX44::ExecutionReport& message,
                           const FIX::SessionID&)
{
    // log::echo("ExecutionReport");
    //  log::echo(message.toXML());

    FIX::OrderID ticket;
    FIX::ExecType exec;
    FIX::OrdStatus stat;
    FIX::Side side;
    FIX::LeavesQty leaveqty;
    FIX::CumQty cumqty;
    FIX::AvgPx avgpx;
    FIX::ClOrdID cl0id;
    FIX::Symbol symbol;
    FIX::OrderQty oqty;
    FIX::RejectText rejtxt;
    FIX::OrdType ordtype;
    FIX::SecondaryOrderID soid;
    FIX::TradeDate tradeDate;
    FIX::ComplianceID fquantId;
    // 权益 或保证金
    FIX::SettlCurrFxRate sfr;
    // adj px
    FIX::LastParPx adjpx;

    message.getFieldIfSet(exec);
    message.getFieldIfSet(stat);
    message.getFieldIfSet(side);
    message.getFieldIfSet(cumqty);
    message.getFieldIfSet(leaveqty);
    message.getFieldIfSet(avgpx);
    message.getFieldIfSet(cl0id);
    message.getFieldIfSet(symbol);
    message.getFieldIfSet(oqty);
    message.getFieldIfSet(ordtype);
    message.getFieldIfSet(soid);
    message.getFieldIfSet(tradeDate);
    message.getFieldIfSet(fquantId);
    message.getFieldIfSet(sfr);
    message.getFieldIfSet(adjpx);

    e2::Int_e quantId = stol(fquantId.getValue().c_str());
    double margin = sfr.getValue();

    std::string key = cl0id.getValue();
    e2::OrdStatus ord_status = convert(stat);

    if (FixPtr->_OrderIds.count(quantId) == 0) {
        log::echo("canceled :", quantId, " status:", ord_status);
        return;
    }

    if (!message.isSetField(ticket)) {
        // reject
        message.getFieldIfSet(rejtxt);
        FixPtr->_OrderIds.erase(quantId);
        log::echo("reject and earse:", quantId);

        return;
    }
    message.getField(ticket);
    std::size_t tk = atoll(ticket.getValue().c_str());
    e2::Int_e closetck = atoll(soid.getValue().c_str());

    std::size_t thread_number = 0;
    if (FixPtr->_cash.cl_thread.count(key) > 0) {
        thread_number = FixPtr->_cash.cl_thread.at(key);
    }

    if (exec.getValue() == FIX::OrdStatus_CANCELED) {
        // 1.只有在 match 状态下发生的, 用户主动删除订单还没有做
        // 所以这儿开平仓的 match 都有可能
        RejectOrCancelNewOrder(quantId, key, tk, closetck, thread_number,
                               false);

        return;
    }

    std::size_t oid_m = FixPtr->_OrderIds[quantId].count(key);
    double cumq = cumqty.getValue();
    double leave = leaveqty.getValue();
    long ctime = atol(tradeDate.getValue().c_str());
    if (oid_m != 1) {
        log::bug("not found cl0id:", key, " quantId:", quantId, " ticket",
                 ticket);
        return;
    }
    if (tk == 0) {
        log::bug("ticket == 0: ", ticket.getValue());
        return;
    }

    if (margin > 0) {
        switch (ord_status) {
            case e2::OrdStatus::ost_New: {
                FixPtr->_cash.inc(thread_number, margin);

                if (FixPtr->_cash.order_cash.count(tk) == 0) {
                    TraderData_t dtt;
                    dtt.adj_price = 0;
                    dtt.margin = margin;
                    dtt.equity = 0;
                    dtt.thread_number = thread_number;

                    FixPtr->_cash.order_cash.insert({tk, dtt});
                }
                else {
                    log::bug("ticket error:", tk);
                }

                e2::Int_e price = FixPtr->_OrderIds[quantId].at(key).price;
                e2::Int_e qty = FixPtr->_OrderIds[quantId].at(key).qty;

                double expenditure = e2q::FixPtr->equity(price, qty);
                e2q::FixPtr->_cash.inc_freeze(thread_number, expenditure);

                break;
            }
            case e2::OrdStatus::ost_Filled:

                if (closetck > 0) {
                    // close order
                    FixPtr->_cash.append(thread_number, margin);
                    // std::string balan = log::format(
                    //     "total cash:%.3f, clostck:%ld, margin:%.3f",
                    //     e2q::FixPtr->_cash.total_cash, closetck, margin);
                    // log::info(balan);
                    if (FixPtr->_cash.order_cash.count(closetck) == 1) {
                        FixPtr->_cash.order_cash.erase(closetck);
                    }
                    else {
                        log::bug("closetck is error:", closetck);
                    }
                }
                else {
                    // fille open order

                    if (FixPtr->_cash.order_cash.count(tk) == 0) {
                        std::string equity = log::format("%.2f", margin);
                        log::bug("filled ticket:", tk, " margin:", equity,
                                 " stats:", stat.getValue(),
                                 " close:", closetck);
                    }
                    else {
                        double zemargin =
                            FixPtr->_cash.order_cash.at(tk).margin;
                        zemargin -= margin;
                        FixPtr->_cash.append(thread_number, zemargin);
                        //                        FixPtr->_cash.total_cash +=
                        //                        zemargin;
                        FixPtr->_cash.order_cash.at(tk).margin = margin;
                    }
                }

                break;

            case e2::OrdStatus::ost_Partially_filled: {
                break;
            }
            default:
                log::bug("default ticket:", tk, " stats:", stat.getValue(),
                         " close:", closetck);

                break;
        }
    }

    FixPtr->_OrderIds[quantId][key].ticket = tk;

    FixPtr->_OrderIds[quantId][key].stat = ord_status;

    FixPtr->_OrderIds[quantId][key].cumqty = cumq;
    FixPtr->_OrderIds[quantId][key].leavesqty = leave;
    FixPtr->_OrderIds[quantId][key].avgpx = VALNUMBER(avgpx.getValue());
    FixPtr->_OrderIds[quantId][key].tdate = ctime;
    FixPtr->_OrderIds[quantId][key].adjpx = VALNUMBER(adjpx.getValue());

    FixPtr->_OrderTicket.insert({tk, key});

    if (closetck > 0) {
        // is close order
        FixPtr->_OrderIds[quantId][key].closeTck = closetck;

        if (cumq == 0) {
            FixPtr->_OrderIds[quantId][key].trading = TradeStatus::PENDING;
        }
        else {
            FixPtr->_OrderIds[quantId][key].openqty -= cumq;

            if (FixPtr->_OrderIds[quantId][key].leavesqty == 0 ||
                stat == TradeStatus::FILLED) {
                FixPtr->_OrderIds[quantId][key].trading = TradeStatus::FILLED;

                // 修改 开仓的状态
                if (FixPtr->_OrderTicket.count(closetck) == 0) {
                    return;
                }
                key = FixPtr->_OrderTicket[closetck];
                FixPtr->_OrderIds[quantId][key].trading = TradeStatus::CLOSED;
            }
            else {
                FixPtr->_OrderIds[quantId][key].trading =
                    TradeStatus::PARTIALLY_FILLED;
            }
        }

        UpdateQuantProfit();
    }
    else {
        if (cumq == 0) {
            FixPtr->_OrderIds[quantId][key].trading = TradeStatus::PENDING;
        }
        else {
            FixPtr->_OrderIds[quantId][key].openqty += cumq;

            if (FixPtr->_OrderIds[quantId][key].leavesqty == 0) {
                FixPtr->_OrderIds[quantId][key].trading = TradeStatus::MARKET;
            }
            else {
                FixPtr->_OrderIds[quantId][key].trading =
                    TradeStatus::PARTIALLY_FILLED;
            }
        }
    }

} /* -----  end of function FixAccount::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  1. 开仓的时候，不允许下单 stoporder ticket == 0
 *  2. 平仓的时候，不允许平仓 order_qyt ==0 or == -1 ticket > 0
 *  3. 开仓的时候， price or qty <=0 ticket == 0
 *  4. 已经订单完成，但 match 的时候出错了 ( buy or sell)
 *  5. 有可能订单完成，也有可能在 match 状态，catch 的error ,
 * ============================================
 */
void FixAccount::onMessage(const FIX44::OrderCancelReject& message,
                           const FIX::SessionID&)
{
    FIX::Text rejtext;
    FIX::OrderID ticket;
    FIX::OrigClOrdID aOrigClOrdID;
    FIX::CxlRejResponseTo aCxlRejResponseTo;
    // 如果是平仓的话， 这个是平仓的值
    FIX::ClOrdID cl0id;
    std::size_t quantId = 0;
    message.getIfSet(rejtext);
    message.getIfSet(ticket);
    message.getIfSet(aOrigClOrdID);
    message.getIfSet(cl0id);
    message.getIfSet(aCxlRejResponseTo);

    e2::Side _side = e2::Side::os_Buy;

    if (aCxlRejResponseTo.getValue() == '2') {
        // 平仓
        _side = e2::Side::os_Sell;
    }

    quantId = atoll(aOrigClOrdID.getValue().c_str());

    std::string key = cl0id.getValue();

    e2::Int_e tk = atoll(ticket.getValue().c_str());
    std::size_t thread_number = 0;
    if (FixPtr->_cash.cl_thread.count(key) > 0) {
        thread_number = FixPtr->_cash.cl_thread.at(key);
    }
    if (_side == e2::Side::os_Buy) {
        // 开仓
        // if (tk == 0) {
        // 开仓出现的
        // 1,3
        // tk > 0 这儿是 4, buy match出现的
        RejectOrCancelNewOrder(quantId, key, tk, 0, thread_number, true);
        log::bug("quantid:", quantId, "ticket == 0: ", tk,
                 " text:", rejtext.getValue());
    }
    else {
        // 平仓
        // 2, 4(sell)

        RejectOrCancelNewOrder(quantId, key, 0, tk, thread_number, true);
        log::bug("quantid:", quantId, "ticket == 0: ", tk,
                 " text:", rejtext.getValue());
    }

} /* -----  end of function FixAccount::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::onMessage(const FIX44::BidResponse& message,
                           const FIX::SessionID& sid)
{
    //  log::info(message.toXML());

    FIX44::BidResponse::NoBidComponents nbc;

    FIX::Price price;
    FIX::ListID lid;

    Base64 base64;
    int counts = message.groupCount(nbc.field());
    std::string custem;
    size_t sz = 0;
    for (int m = 1; m <= counts; m++) {
        message.getGroup(m, nbc);
        if (nbc.isEmpty()) {
            continue;
        }
        nbc.getField(price);
        nbc.getField(lid);

        custem = base64.b64decode(lid.getValue());
        const char* ptr = custem.c_str();
        sz = price.getValue();
        if (sz != custem.size()) {
            log::info("sz:", sz, " base64 len:", custem.size());
        }
        PushGCM(ptr, sz);
    }
} /* -----  end of function FixAccount::onMessage  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::onMessage(const FIX44::OrderStatusRequest& message,
                           const FIX::SessionID&)
{
    log::echo("11");

} /* -----  end of function FixAccount::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::QuoteRequest
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::QuoteRequest(std::vector<std::size_t>& symbols)
{
    FIX44::QuoteRequest quoteReq =
        FIX44::QuoteRequest(FIX::QuoteReqID(UUidGen()));
    quoteReq.setField(genClOrdID());

    FIX44::QuoteRequest::NoRelatedSym relateGroup;
    int index = 0;

    std::size_t size_sym = symbols.size();
    if (size_sym == 0) {
        log::bug("not request symbol");
    }

    for (auto _sym : symbols) {
        FIX::Symbol sym(std::to_string(_sym));
        relateGroup.setField(sym);
        FIX::SymbolSfx spn(std::to_string(index));
        relateGroup.setField(spn);

        quoteReq.addGroup(relateGroup);
        index++;
    }

    if (index == 0) {
        FIX::Product pro = FIX::Product(FIX::Product_INDEX);
        relateGroup.setField(pro);
        quoteReq.addGroup(relateGroup);
    }

    /**
     * 作为向 broker 申请 的现金字段
     */
    FIX::CashOrderQty coq;
    coq.setValue(FixPtr->_cash.total_cash);
    quoteReq.setField(coq);

    defHeader(quoteReq.getHeader());

    try {
        FIX::Session::sendToTarget(quoteReq);
    }
    catch (FIX::SessionNotFound& s) {
        log::bug(s.what());
    }

} /* -----  end of function FixAccount::QuoteRequest  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::NewOrderSingle
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::size_t FixAccount::NewOrderSingle(Int_e id, Int_e side, Int_e qty,
                                       Int_e price, Int_e slippage, Int_e otype,
                                       std::size_t quantId,
                                       std::size_t order_time,
                                       std::size_t thread_number)
{
    FIX::OrdType ordType = convert((e2::OrdType)otype);
    FIX::ClOrdID cl0id = genClOrdID();
    FIX44::NewOrderSingle newOrderSingle(cl0id, convert((e2::Side)side),
                                         FIX::TransactTime(), ordType);
    if (FixPtr->_fix_symbols.count(id) == 0) {
        log::bug(" symbols id error , id:", id);
        return -1;
    }

    FIX::TradeDate tradeDate;
    tradeDate.setValue(std::to_string(order_time));

    std::string symbol = e2q::FixPtr->_fix_symbols.at(id);
    newOrderSingle.set(FIX::HandlInst('3'));
    /**
     * 下次优化为 int 值
     */
    newOrderSingle.set(genSymbol(symbol));
    newOrderSingle.set(genOrderQty(qty));
    newOrderSingle.set(tradeDate);

    FIX::ComplianceID fquantId;
    fquantId.setValue(std::to_string(quantId));
    newOrderSingle.setField(fquantId);

    // long or short order
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_MARKET) {
        newOrderSingle.set(genPrice(price));
    }

    defHeader(newOrderSingle.getHeader());

    OrderInfo oi;
    oi.symbol = id;
    oi.side = (e2::Side)side;
    oi.qty = qty;
    oi.price = price;
    oi.stoppx = 0;
    oi.slippage = slippage;
    oi.ordtype = (e2::OrdType)otype;
    oi.stat = e2::OrdStatus::ost_New;
    oi.ticket = 0;
    oi.closeTck = 0;
    oi.openqty = 0;
    oi.trading = TradeStatus::PENDING;

    std::string key = cl0id.getValue();

    if (FixPtr->_cash._tsize > 0) {
        FixPtr->_cash.addClThread(key, thread_number);
    }

    if (FixPtr->_OrderIds.count(quantId) == 0) {
        OrderInfoMap oim;
        oim.insert({key, oi});
        FixPtr->_OrderIds.insert({quantId, oim});
    }
    else {
        FixPtr->_OrderIds[quantId].insert({key, oi});
    }
    try {
        FIX::Session::sendToTarget(newOrderSingle);
    }
    catch (FIX::SessionNotFound& s) {
        log::bug(s.what());
    }

    return 0;
} /* -----  end of function FixAccount::NewOrderSingle  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::OrderReplaceRequest
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::OrderReplaceRequest(Int_e id, Int_e side, Int_e qty,
                                     Int_e stoppx, Int_e slippage, Int_e otype,
                                     Int_e ticket, std::size_t quantId,
                                     std::size_t order_time,
                                     std::size_t thread_number)
{
    FIX::OrdType ordType = convert((e2::OrdType)otype);
    FIX::ClOrdID cl0id = genClOrdID();

    FIX::OrigClOrdID ocid = genOrigClOrdID();
    FIX44::OrderCancelReplaceRequest ocrr(ocid, cl0id, convert((e2::Side)side),
                                          FIX::TransactTime(), ordType);
    if (FixPtr->_fix_symbols.count(id) == 0) {
        log::bug("symobls id error, id:", id);
        return;
    }
    std::string symbol = e2q::FixPtr->_fix_symbols.at(id);
    //    FIX::HandlInstCodeSet
    ocrr.set(FIX::HandlInst('3'));
    ocrr.set(genSymbol(symbol));
    ocrr.set(genOrderQty(qty));
    //    ocrr.set(convert(e2::TimeInForce::tif_day));

    FIX::ComplianceID fquantId;
    fquantId.setValue(std::to_string(quantId));
    ocrr.setField(fquantId);

    // close order
    if (ordType == FIX::OrdType_STOP || ordType == FIX::OrdType_STOP_LIMIT) {
        FIX::OrderID oid = FIX::OrderID(std::to_string(ticket));
        ocrr.setField(oid);
        ocrr.set(genStopPx(stoppx));
    }

    FIX::TradeDate tradeDate;
    tradeDate.setValue(std::to_string(order_time));
    ocrr.setField(tradeDate);

    defHeader(ocrr.getHeader());

    OrderInfo oi;
    oi.symbol = id;
    oi.side = (e2::Side)side;
    oi.qty = qty;
    oi.price = 0;
    oi.stoppx = stoppx;
    oi.slippage = slippage;
    oi.ordtype = (e2::OrdType)otype;
    oi.stat = e2::OrdStatus::ost_New;
    oi.closeTck = ticket;
    oi.ticket = 0;
    oi.openqty = qty;
    oi.trading = TradeStatus::PENDING;

    std::string key = cl0id.getValue();

    if (FixPtr->_cash._tsize > 0) {
        FixPtr->_cash.addClThread(key, thread_number);
    }
    if (FixPtr->_OrderIds.count(quantId) == 0) {
        OrderInfoMap oim;
        oim.insert({key, oi});
        FixPtr->_OrderIds.insert({quantId, oim});
    }
    else {
        FixPtr->_OrderIds[quantId].insert({key, oi});
    }

    try {
        FIX::Session::sendToTarget(ocrr);
    }
    catch (FIX::SessionNotFound& s) {
        log::bug(s.what());
    }

    return;
} /* -----  end of function FixAccount::OrderReplaceRequest  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::defHeader
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::defHeader(FIX::Header& header)
{
    header.setField(_sender);
    header.setField(_target);
    header.setField(_begin);
} /* -----  end of function FixAccount::defHeader  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::genClOrdID
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
FIX::ClOrdID FixAccount::genClOrdID()
{
    std::string val = UUidGen();
    return FIX::ClOrdID(val);
} /* -----  end of function FixAccount::genClOrdID  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::genOrigClOrdID
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
FIX::OrigClOrdID FixAccount::genOrigClOrdID()
{
    std::string val = UUidGen();
    return FIX::OrigClOrdID(val);
} /* -----  end of function FixAccount::genOrigClOrdID ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::genSymbol
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
FIX::Symbol FixAccount::genSymbol(std::string& sym) { return FIX::Symbol(sym); }
FIX::Symbol FixAccount::genSymbol(e2::Int_e sym)
{
    return FIX::Symbol(std::to_string(sym));
}
/* -----  end of function FixAccount::genSymbol  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::genOrderQty
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
FIX::OrderQty FixAccount::genOrderQty(Int_e q)
{
    return FIX::OrderQty(q);
} /* -----  end of function FixAccount::genOrderQty  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::genPrice
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
FIX::Price FixAccount::genPrice(Int_e e)
{
    return FIX::Price(e);
} /* -----  end of function FixAccount::genPrice  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::genStopPx
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
FIX::StopPx FixAccount::genStopPx(Int_e e)
{
    return FIX::StopPx(e);
} /* -----  end of function FixAccount::genStopPx  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::Init
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::Init(_Resource_ptr ptr, std::shared_ptr<BeamData> beam_data,
                      std::shared_ptr<Shuttle> shu_ptr)
{
    if (ptr == nullptr || beam_data == nullptr) {
        log::bug("sourece ptr is nullptr!!!");
    }
    else {
        _beam_data = std::move(beam_data);
    }
    _source_ptr = ptr;
    _fq.resource(ptr, FixPtr->_offer_time);
    _beam_data->assign<FixQuote, Connect_beam>(_fq);

} /* -----  end of function FixAccount::Init  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::CallBack
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::CallBack(fixType call)
{
    _call = std::move(call);
} /* -----  end of function FixAccount::CallBack  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::wait
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::wait()
{
    while (_is_end == false) {
        FIX::process_sleep(2);
    }
} /* -----  end of function FixAccount::wait  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::quit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixAccount::quit()
{
    UpdateQuantProfit();

    _is_end = true;
} /* -----  end of function FixAccount::quit  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::RejectOrCancelNewOrder
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  开仓的时候，会扣掉一些资金，如果下单不成功，在这儿统一返还
 * ============================================
 */
void FixAccount::RejectOrCancelNewOrder(e2::Int_e quantId, std::string key,
                                        std::size_t ticket, e2::Int_e closetck,
                                        std::size_t thread_number, bool rc)
{
    if (key.length() == 0 || FixPtr->_OrderIds[quantId].count(key) == 0) {
        log::bug("quantId:", quantId, " ticket:", ticket);
    }
    else {
        TradeStatus tstat = TradeStatus::CANCEL;
        if (rc) {
            tstat = TradeStatus::REJECT;
        }
        FixPtr->_OrderIds[quantId][key].trading = tstat;
    }
    // 平仓直接退出就可以了
    // OrderClose 的时候，设置状态为 closeing 了，现在需要转回来
    if (closetck > 0) {
        std::string bkey = FixPtr->_OrderTicket[closetck];
        if (FixPtr->_OrderIds[quantId].count(bkey) == 0) {
            log::bug("key is error:", bkey, " quantId:", quantId);

            return;
        }
        FixPtr->_OrderIds[quantId][bkey].trading = TradeStatus::MARKET;
        return;
    }

    // 开仓需要退资金
    // 还没有返回 e2::OrdStatus::ost_New 状态
    if (ticket == 0) {
        e2::Int_e price = FixPtr->_OrderIds[quantId].at(key).price;
        e2::Int_e qty = FixPtr->_OrderIds[quantId].at(key).qty;

        double expenditure = e2q::FixPtr->equity(price, qty);
        e2q::FixPtr->_cash.inc_freeze(thread_number, expenditure);

        return;
    }

    // cancel 状态,正常下单，但由于超时，隔夜或者用户删除订单而退回来的
    //  如果是 reject 的话，那可能是 processOrder == false
    //  insert 订单出错
    for (auto it : e2q::FixPtr->_cash.order_cash) {
        if (ticket == it.first) {
            // log::bug("margin:", it.second.margin);
            e2q::FixPtr->_cash.append(thread_number, it.second.margin);
            FixPtr->_cash.order_cash.erase(it.first);

            break;
        }
    }
    // log::info("quantId:", quantId, " ticket:", ticket, " closetck:",
    // closetck,
    //           " soid:", key);
} /* -----  end of function FixAccount::RejectOrCancelNewOrder  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::ChangeTradingStatus
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  好像没有用
 * ============================================
 */
void FixAccount::ChangeTradingStatus(e2::Int_e quantId, std::string& key,
                                     double cumq)
{
    if (cumq == 0) {
        FixPtr->_OrderIds[quantId][key].trading = TradeStatus::PENDING;
    }
    else {
        if (FixPtr->_OrderIds[quantId][key].leavesqty == 0) {
            FixPtr->_OrderIds[quantId][key].trading = TradeStatus::FILLED;
        }

        FixPtr->_OrderIds[quantId][key].openqty += cumq;
    }

} /* -----  end of function FixAccount::ChangeTradingStatus  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixAccount::UpdateQuantProfit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  更新数据库关于 profit 的值
 * ============================================
 */
void FixAccount::UpdateQuantProfit()
{
    std::size_t size_qid = FixPtr->_quantId.size();
    double total_cash = 0;

    if (FixPtr->_cash._tsize == 0 && size_qid > 1) {
        // 多个 quantid ,tsize == 0
        // 这个时候是不是 quantid 划分仓位的情况，就不需要写入profit 了
        // 需要采用另一种方式计算
        // 也就是多线程(不管是不是多进程)的时候，如果没有对单独 quantid
        // 分配仓位，也就是共用仓位，这个时候，就不单独计算每个 quantid
        // 的收益，这个时间收益和其实是单独一个账号的收益了

        return;
    }

    std::size_t idx = e2q::GlobalDBPtr->getId();

    e2q::Pgsql* gsql = e2q::GlobalDBPtr->ptr(idx);
    if (gsql != nullptr) {
        for (auto it : FixPtr->_quantId) {
            // 多模型，多仓位的情况
            total_cash = 0;
            for (auto oc : FixPtr->_cash.order_cash) {
                if (oc.second.thread_number == it.second.second) {
                    total_cash += oc.second.margin;
                    break;
                }
            }
            if (FixPtr->_cash._tsize > 0 &&
                FixPtr->_cash._thread_pos.count(it.second.second) > 0) {
                total_cash +=
                    FixPtr->_cash._thread_pos.at(it.second.second)._total_cash;
            }

            if (FixPtr->_cash._tsize == 0 && size_qid == 1) {
                // 等于 1 的时候，单个模型的情况
                total_cash += FixPtr->_cash.TotalCash(0);
            }
            gsql->update_table("analse");
            gsql->update_field("profit", total_cash, 3);
            gsql->update_condition("quantid", it.second.first);
            gsql->update_commit();
        }
    }
    e2q::GlobalDBPtr->release(idx);
} /* -----  end of function FixAccount::UpdateQuantProfit  ----- */
}  // namespace e2q
