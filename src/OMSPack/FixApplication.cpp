/*
 * =====================================================================================
 *
 *       Filename:  FixApplication.cpp
 *
 *    Description:  FixApplication
 *
 *        Version:  1.0
 *        Created:  2024年01月16日 09时30分19秒
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
#include "OMSPack/FixApplication.hpp"

#include <unistd.h>

#include <cstddef>
#include <cstdlib>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "OMSPack/IDGenerator.hpp"
#include "OMSPack/OrderBook/Order.hpp"
#include "OMSPack/SessionGlobal.hpp"
#include "Toolkit/Norm.hpp"
#include "Toolkit/eLog.hpp"
#include "assembler/BaseType.hpp"
#include "quickfix/Exceptions.h"
#include "quickfix/Field.h"
#include "quickfix/FieldTypes.h"
#include "quickfix/FixFields.h"
#include "quickfix/FixValues.h"
#include "quickfix/Message.h"
#include "quickfix/Session.h"
#include "quickfix/SessionID.h"
#include "quickfix/fix44/MessageCracker.h"
#include "quickfix/fix44/OrderCancelReject.h"
#include "quickfix/fix44/QuoteResponse.h"
namespace e2q {
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::toFeedData
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::toFeedData(_Resource_ptr ptr,
                                std::shared_ptr<BeamData> beam)
{
    _fdata.resource(ptr);
    beam->assign<FeedData, Connect_beam>(_fdata);

} /* -----  end of function FixApplication::toFeedData  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::onLogon
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::onLogon(const FIX::SessionID& sid)
{
    std::vector<std::size_t> symbols;
    if (SessionSymList.count(sid) == 0) {
        SessionSymList[sid] = symbols;
    }
    if (SessionSymList[sid].size() == 0) {
        elog::info("sid is login ok, SessionId:",
                   sid.getTargetCompID().getValue());
    }

} /* -----  end of function FixApplication::onLogon  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::onLogout
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::onLogout(const FIX::SessionID& sid)
{
    if (!_is_end) {
        elog::info("logout:", sid.getTargetCompID());
    }

} /* -----  end of function FixApplication::onLogout  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::toAdmin
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::toAdmin(FIX::Message& msg, const FIX::SessionID&)
{
    //  elog::echo(msg.toXML());
} /* -----  end of function FixApplication::toAdmin  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::toApp
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::toApp(FIX::Message& msg, const FIX::SessionID&)
{
#ifdef DEBUG
    try {
        FIX::PossDupFlag possDupFlag;
        if (msg.getHeader().isSetField(possDupFlag)) {
            msg.getHeader().getField(possDupFlag);
            if (possDupFlag) {
                throw FIX::DoNotSend();
            }
        }
    }
    catch (FIX::FieldNotFound& f) {
        elog::bug(f.what(), " field:", f.field);
        elog::echo(msg.toXML());
    }

#endif

} /* -----  end of function FixApplication::toApp  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::fromAdmin
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::fromAdmin(const FIX::Message& msg, const FIX::SessionID&)
{
    // elog::info(msg.toXML());
} /* -----  end of function FixApplication::fromAdmin  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::fromApp
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::fromApp(const FIX::Message& msg, const FIX::SessionID& sid)
{
    // elog::bug("message come in....");
    try {
        crack(msg, sid);
    }
    catch (FIX::FieldNotFound& f) {
        elog::bug(f.what(), " field:", f.field);
    }
    catch (FIX::IncorrectDataFormat& i) {
        elog::bug(i.what());
        elog::bug(msg.toXML());
    }
    catch (FIX::IncorrectTagValue& v) {
        elog::bug(v.what());
        elog::info(msg.toXML());
    }
    catch (FIX::UnsupportedMessageType& t) {
        elog::bug(t.what());
    }

} /* -----  end of function FixApplication::fromApp  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::onMessage(const FIX44::Heartbeat& message,
                               const FIX::SessionID&)
{
    elog::echo("11");
} /* -----  end of function FixApplication::onMessage  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::onMessage(const FIX44::QuoteRequest& message,
                               const FIX::SessionID& sid)
{
#ifdef DEBUG
    elog::info("QuoteRequest");
#endif
    FIX44::QuoteRequest::NoRelatedSym relateGroup;

    int num = message.groupCount(relateGroup.field());

    std::vector<std::size_t> symbols;
    std::size_t sym_id = 0;
    double cash = 0;
    int isRet = 1;

    for (int m = 1; m <= num; m++) {
        message.getGroup(m, relateGroup);
        if (relateGroup.isEmpty()) {
            continue;
        }

        FIX::Symbol symbol;
        if (relateGroup.isSetField(symbol)) {
            relateGroup.getField(symbol);
            sym_id = atol(symbol.getString().c_str());
            symbols.push_back(sym_id);
        }
        FIX::CashOrderQty coq;
        if (relateGroup.isSetField(coq)) {
            relateGroup.getField(coq);
            cash = coq.getValue();
        }

        FIX::PutOrCall pc;

        if (relateGroup.isSetField(pc)) {
            relateGroup.getField(pc);

            isRet = pc.getValue();
        }
    }

    bool onlyone = false;
    for (auto it : FinFabr->_fix_symbols) {
        if (it.second.only_ea == OnlyEA::FORANLYONE) {
            onlyone = true;
            break;
        }
    }

    if (onlyone == false) {
        // 1. 在收到新的上市股票 前 登录的
        // 如果不处理的话，就那是由 MarketIng 来处理了
#ifdef DEBUG
        elog::info("onlyone false, fix_symbols:", FinFabr->_fix_symbols.size());
#endif
        return;
    }

    if (symbols.size() > 0) {
        // 如果是 ea 自己定义的 symoble 有可能是错误的
        // 不过不验证了， BeamObj 这儿处理吧
        // 确定订阅及分配资金
        if (SessionSymList.count(sid) == 0) {
            SessionSymList.insert({sid, symbols});
        }
        else {
            SessionSymList[sid] = symbols;
        }
    }

    // onlyone > 0
    // FinFabr->_fix_symbols.size()>0  这个时候， 应该是有新的账号登录了

    //        if (onlyone) {
    // 2. 在收到新的上市股票 后 登录的
    // MassQuote 否则 ea 就没有办法初始化了

    if (isRet) {
        MassQuote(sid);
    }

    if (cash > 0) {
        // 实际申请到的值
        cash = GlobalMatcher->traders(sid, cash);

        // if cash == 0 set ea total_cash == 0
        FIX::QuoteRespID respid;
        respid.setValue(UUidGen());
        FIX::QuoteRespType resptype;
        resptype.setValue(1);

        FIX44::QuoteResponse qr(respid, resptype);

        FIX::MidPx px;
        px.setValue(cash);
        qr.setField(px);
        try {
            FIX::Session::sendToTarget(qr, sid);
        }
        catch (FIX::SessionNotFound&) {
        }
    }

} /* -----  end of function FixApplication::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::onMessage(const FIX44::QuoteStatusReport& message,
                               const FIX::SessionID& sid)
{
    FIX::QuoteStatus stat;
    message.getFieldIfSet(stat);

    if (stat.getValue() == 1) {
        elog::echo("QuoteStatusReport sid:", sid.getTargetCompID().getValue(),
                   " size:", SessionSymList.size());
    }
    else {
        if (_is_logout) {
            GlobalMatcher->SessionLogout(sid);

            if (SessionSymList.count(sid) == 1) {
                SessionSymList[sid].clear();
            }

            // elog::info("clear:", sid.getTargetCompID());
        }
    }

} /* -----  end of function FixApplication::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   callback risk fun
 * ============================================
 */
void FixApplication::onMessage(const FIX44::NewOrderSingle& message,
                               const FIX::SessionID& sessionID)
{
    switch (FinFabr->_BookType) {
        case e2::BookType::BBook:
            onMess(message);
            break;
        case e2::BookType::ABook:
            elog::info("ABook new order");
            onMess(message);
            break;
        default:
            onMess(message);
            break;
    }

} /* -----  end of function FixApplication::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::onMessage(const FIX44::OrderCancelRequest& message,
                               const FIX::SessionID&)
{
    elog::echo("222");

    FIX::OrigClOrdID origClOrdID;
    FIX::Symbol symbol;
    FIX::Side side;

    message.get(origClOrdID);
    message.get(symbol);
    message.get(side);

    try {
        processCancel(origClOrdID, symbol, convert(side));
    }
    catch (std::exception&) {
    }

} /* -----  end of function FixApplication::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::onMessage(const FIX44::OrderCancelReject&,
                               const FIX::SessionID&)
{
    elog::echo("222");

} /* -----  end of function FixApplication::onMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::onMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::onMessage(const FIX44::OrderCancelReplaceRequest& message,
                               const FIX::SessionID& sessionID)
{
    // elog::info("OrderCancelReplaceRequest");

    switch (FinFabr->_BookType) {
        case e2::BookType::BBook:
            onMess(message);
            break;
        case e2::BookType::ABook:
            onMess(message);
            break;
        default:
            onMess(message);
            break;
    }
} /* -----  end of function FixApplication::onMessage  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::Message
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
FIX44::Quote FixApplication::Message()
{
    FIX::Message mess;
    FIX::QuoteID id = UUidGen();
    mess.setField(id);

    FIX44::Quote::NoEvents event;

    FIX::Symbol symbol("00686");
    event.setField(symbol);

    FIX::InstrumentPartyID idsss;

    FIX44::Quote::NoEvents event2;

    FIX::Symbol symbol2("00687");
    event2.setField(symbol2);

    mess.addGroup(event);
    mess.addGroup(event2);

    FIX::QuoteType qt = FIX::QuoteType_TRADEABLE;
    mess.setField(qt);

    FIX::QuoteReqID rid = UUidGen();
    mess.setField(rid);

    FIX44::Quote quote(mess);

    return quote;
} /* -----  end of function FixApplication::Message  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::processOrder
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  这儿是新订单
 * ============================================
 */
bool FixApplication::processOrder(OrderItem* order)
{
    acceptOrder(order->Lots());

    bool ret = GlobalMatcher->insert(order);
    if (ret == false) {
        elog::bug("insert bug:", order->Pending()->getTicket());
        return false;
    }

    return true;
} /* -----  end of function FixApplication::processOrder  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::matcher
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::matcher(std::string symbol, e2::Int_e now, e2::Int_e price,
                             e2::Int_e adjprice)
{
    std::vector<OrderLots> ret =
        GlobalMatcher->matcher(symbol, now, price, adjprice);

    for (auto lot : ret) {
        if (lot.isCancel) {
            cancelOrder(lot);
        }
        else {
            fillOrder(lot);
        }
    }
} /* -----  end of function FixApplication::matcher  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::processCancel
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::processCancel(const std::string& id,
                                   const std::string& symbol,
                                   const e2::Side& side)
{
    /* Order& order = _orderMatcher.find(symbol, side, id); */
    /* order.cancel(); */
    /* cancelOrder(order); */
    /* _orderMatcher.erase(order); */

} /* -----  end of function FixApplication::processCancel  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::rejectOrder
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::rejectOrder(const FIX::SessionID& sid,
                                 const FIX::ClOrdID& clOrdID,
                                 const FIX::Symbol& symbol,
                                 const FIX::Side& side,
                                 const std::string& message, e2::Int_e ticket,
                                 e2::Int_e quantId, e2::Int_e qty,
                                 e2::Int_e price)
{
    FIX::OrderID oid(std::to_string(ticket));

    e2::Side _side = convert(side);

    FIX::OrigClOrdID aOrigClOrdID(std::to_string(quantId));

    FIX::CxlRejResponseTo aCxlRejResponseTo('2');
    if (_side == e2::Side::os_Buy) {
        aCxlRejResponseTo.setValue('1');
    }

    FIX44::OrderCancelReject reject = FIX44::OrderCancelReject(
        oid, clOrdID, aOrigClOrdID, FIX::OrdStatus(FIX::OrdStatus_REJECTED),
        aCxlRejResponseTo);

    FIX::Text rejtext;
    rejtext.setValue(message + " symbol:" + symbol.getValue());
    reject.setField(rejtext);

    std::size_t idx = GlobalDBPtr->getId();

    // 这儿是平仓出现的
    e2::OrdStatus stat = e2::OrdStatus::ost_Canceled;
    if (ticket == 0) {
        // 开仓出现的
        stat = e2::OrdStatus::ost_Rejected;
    }

    Pgsql* gsql = GlobalDBPtr->ptr(idx);
    if (gsql != nullptr) {
        int cfi = 0;
        for (auto it : FinFabr->_fix_symbols) {
            if (it.first == 0) {
                continue;
            }
            // 这儿以后优化吧
            if (it.second.symbol == symbol.getValue()) {
                if (it.first > cfi) {
                    cfi = it.first;
                }
            }
        }
        std::string cfi_str = elog::format(
            "(SELECT id FROM stockinfo WHERE symbol=%d ORDER BY id DESC "
            "LIMIT "
            "1 )",
            cfi);
        gsql->insert_table("trades");
        gsql->insert_field("symbol", cfi_str);
        gsql->insert_field("ticket", ticket);
        gsql->insert_field("stat", int(stat));
        gsql->insert_field("ctime", ticket_now);
        gsql->insert_field("otime", ticket_now);

        gsql->insert_field("side", int(_side));
        gsql->insert_field("quantid", quantId);
        gsql->insert_field("qty", qty);
        gsql->insert_field("price", NUMBERVAL(price));
        InsertCommit(gsql);
    }

    GlobalDBPtr->release(idx);

    try {
        FIX::Session::sendToTarget(reject, sid);
    }
    catch (FIX::SessionNotFound&) {
    }

} /* -----  end of function FixApplication::rejectOrder  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::FeedDataHandle
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::FeedDataHandle()
{
    /**
     * 等有数据 源再详细做，现在只是简单发个信息先
     */

    FIN_FABR_IS_NULL();

    /**
     * ordtype:
     * limit
     * 1. reject
     * 2. pending loop
     */
    auto fix_call = [this](SeqType cfi, SeqType now, SeqType price,
                           SeqType qty) {
        SeqType adj_ret = -1;
        if (price == 0) {
            return adj_ret;
        }

        std::string sym = "";
        //&& it.second.dia == DoIAction::LIST
        for (auto it : FinFabr->_fix_symbols) {
            if (it.first == cfi) {
                sym = it.second.symbol;
                break;
            }
        }

        if (GlobalMatcher->OrdTypePending()) {
            GlobalMatcher->TopLevelPrice(sym, price);
        }

        if (this->_program != nullptr) {
            //  先这样吧，以后再优化
            if (ticket_now == 0) {
                global_id_class[0] = this_thread::get_id();
#ifndef KAFKALOG
                log.init(global_id_class[0]);
#endif
            }
            ticket_now = now;
            adj_ret = price;

            if (FinFabr->_enable_exrd == e2::Bool::B_TRUE) {
                double cfi_d = VALNUMBER(cfi);
                ExdiSymList.addPrice(cfi_d, price);

                adj_ret = this->_program->toScript(e2::OMSRisk::I_OMS, cfi);
            }
        }

        if (adj_ret <= 0) {
            adj_ret = price;
        }

        // 涨跌停，由 kafka 发送价格端来控制，有量的话，就交易,否则不进行交易
        if (qty > 0) {
            this->matcher(sym, now, price, adj_ret);
        }

        return adj_ret;
    };  // -----  end lambda  -----

    _fdata.ctrl(fix_call);

    _is_logout = false;

    for (auto session : SessionSymList) {
        QuoteCancel(session.first);
    }

    GlobalMatcher->exist();

    _is_end = true;
#ifdef KAFKALOG
    log.exist();
#endif
} /* -----  end of function FixApplication::FeedDataHandle  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::QuoteCancel
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::QuoteCancel(const FIX::SessionID& sid)
{
    FIX44::QuoteCancel qc = FIX44::QuoteCancel();

    FIX::QuoteID qid = FIX::QuoteID(UUidGen());
    qc.setField(qid);

    FIX::QuoteReqID qrid = FIX::QuoteReqID(UUidGen());
    qc.setField(qrid);

    FIX::QuoteCancelType qct = FIX::QuoteCancelType(1);
    qc.setField(qct);

    try {
        FIX::Session::sendToTarget(qc, sid);
    }
    catch (FIX::SessionNotFound& s) {
        elog::bug(s.what());
    }
} /* -----  end of function FixApplication::QuoteCancel  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::end
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool FixApplication::end()
{
    return _is_end;
} /* -----  end of function FixApplication::end  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::E2LScript
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int FixApplication::E2LScript(e2::OrdType ordType, e2::Side side,
                              double bot_qty, e2::Int_e symbol)
{
    int risk = -1;

    if (_program != nullptr) {
        /**
         * 2. {sender compid process } risk
         * bse -> supply and demand schedule (SDS)
         */
        global_id_class[1] = std::this_thread::get_id();

#ifndef KAFKALOG
        log.init(global_id_class[1]);
#endif
        risk = _program->toScript(e2::OMSRisk::I_BROKER, symbol);

        if (FinFabr->_BookType == e2::BookType::BBook && risk == 0) {
            FIX::SessionID botsid;

            SeqType ticket = GlobalMatcher->sequence(botsid, side);

            int ret = GlobalMatcher->AddBotTicket(ticket, ordType, side,
                                                  bot_qty, symbol);
            if (ret == -1) {
                elog::bug("AddBotTicket == -1");
                return -1;
            }
        }
    }
    else {
        elog::bug("program is not run");
    }
    return risk;
} /* -----  end of function FixApplication::E2LScript  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixApplication::lob
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixApplication::lob(const FIX::SessionID& sid, const FIX::Symbol& symbol,
                         const FIX::Side& side, const FIX::OrdType& ordType,
                         const FIX::OrderQty& orderQty, const FIX::Price& price,
                         const FIX::StopPx& stopx, const FIX::ClOrdID& clOrdID,
                         const FIX::OrderID& oid,
                         const FIX::ComplianceID& quantId,
                         FIX::TradeDate tradeDate)
{
    FIX::TimeInForce timeInForce(FIX::TimeInForce_DAY);
    if (FinFabr->_tif != e2::TimeInForce::tif_day) {
        timeInForce = convert(FinFabr->_tif);
    }

    double order_price = 0;
    double order_qty = orderQty.getValue();
    e2::OrdType oType = convert(ordType);
    e2::Side oside = convert(side);
    int risk = -1;

    SeqType ticket = 0;
    SeqType ticket_close = 0;
    double margin = 0;
    e2::Int_e qid = std::stoll(quantId.getValue().c_str());

    e2::Int_e sym = 0;

    /**
     *  以后再优化把 symbol 全部转为 int 值
     *
     *  && syms.second.dia == DoIAction::LIST
     */
    for (auto syms : FinFabr->_fix_symbols) {
        if (syms.second.symbol == symbol.getValue()) {
            sym = syms.first;
        }
    }
    if (FinFabr->_stock.count(sym) == 0) {
        elog::bug("bug stock:", sym, " side:", oside,
                  " ticket:", oid.getValue(), " date:", tradeDate.getValue());
        if (price.getLength() > 0) order_price = price.getValue();
        rejectOrder(sid, clOrdID, symbol, side, "symbol not exist", ticket, qid,
                    order_qty, order_price);
        return;
    }
    SeqType ctime = FinFabr->_stock.at(sym)[Trading::t_time];
    long otime = atol(tradeDate.getValue().c_str());

    // long or short order
    if (ordType == FIX::OrdType_LIMIT || ordType == FIX::OrdType_MARKET) {
        // ctime > otime 24h
        // history

        if (FinFabr->_StopOrder) {
            // 不接受新的订单
            rejectOrder(sid, clOrdID, symbol, side, "stop order now", ticket,
                        qid, order_qty, price.getValue());
            return;
        }

        if (price.getLength() > 0) order_price = price.getValue();
    }
    else {
        // close order
        if (stopx.getLength() > 0) order_price = stopx.getValue();

        // close ticket
        if (oid.getLength() > 0) {
            ticket_close = std::stol(oid.getValue());
        }
        /**
         * 1. check price , side , symobl, qty
         */

        order_qty = GlobalMatcher->CheckClose(ticket_close, symbol.getValue(),
                                              order_qty);
        if (order_qty <= 0) {
            std::string error = "CheckClose order_qty == 0";
            if (order_qty == -1) {
                error = "settlement T + x";
            }
            rejectOrder(sid, clOrdID, symbol, side, error, ticket_close, qid,
                        order_qty, order_price);
            return;
        }
    }
    if (order_price <= 0 || order_qty <= 0) {
        elog::echo("order_price == 0");
        rejectOrder(sid, clOrdID, symbol, side, "order_price == 0", ticket, qid,
                    order_qty, order_price);
        return;
    }

    risk = E2LScript(oType, oside, order_qty, sym);
    /**
     * 3.limit order book
     */
    ticket = GlobalMatcher->sequence(sid, oside);

    try {
        /* if (timeInForce != FIX::TimeInForce_DAY) */
        /*     throw std::logic_error("Unsupported TIF, use Day"); */

        bool ret = false;
        OrderItem* order =
            MALLOC(OrderItem, ticket, clOrdID, symbol, sid, oside, oType,
                   order_price, order_qty, qid, ctime, otime);
        order->TradeTicket(ticket_close);

        if (ticket_close == 0) {
            // 开仓的时候才会设置,平仓就得全部 平掉
            margin = GlobalMatcher->CheckMargin(sid, order_price, order_qty);

            if (margin > 0 &&
                FinFabr->_tif == e2::TimeInForce::tif_immediate_or_cancel) {
                order->hasMargin(margin);
            }
        }

        if (risk == 0 && margin != -1) {
            GlobalMatcher->Margin(sid, ticket, margin, order_qty);

            ret = processOrder(order);
        }
        else {
            cancelOrder(order->Lots());
        }
        if (ret == false) {
            GlobalMatcher->freeMargin(sid, ticket, margin);

            rejectOrder(sid, clOrdID, symbol, side, "risk == -1, process order",
                        (ticket_close > 0 ? ticket_close : ticket), qid,
                        order_qty, order_price);
        }
    }
    catch (std::exception& e) {
        rejectOrder(sid, clOrdID, symbol, side,
                    "rejectOrder execption:" + std::string(e.what()),
                    (ticket_close > 0 ? ticket_close : ticket), qid, order_qty,
                    order_price);
    }

} /* -----  end of function FixApplication::lob  ----- */
}  // namespace e2q
