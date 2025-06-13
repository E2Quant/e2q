/*
 * =====================================================================================
 *
 *       Filename:  FixGuard.cpp
 *
 *    Description:  FixGuard
 *
 *        Version:  1.0
 *        Created:  2024年05月29日 14时10分15秒
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
#include "OMSPack/FixGuard.hpp"

#include <string>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "OMSPack/SessionGlobal.hpp"
#include "Toolkit/Norm.hpp"
#include "assembler/BaseType.hpp"
#include "quickfix/Exceptions.h"
#include "quickfix/FixFields.h"
#include "quickfix/FixValues.h"
#include "quickfix/Session.h"
#include "quickfix/SessionID.h"
#include "quickfix/fix44/BidResponse.h"
#include "quickfix/fix44/ExecutionReport.h"
#include "quickfix/fix44/MessageCracker.h"
namespace e2q {
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixGuard::MarketMessage
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   MarketDataSnapshotFullRefresh
 * ============================================
 */
void FixGuard::MarketMessage(const FIX::SessionID& sid,
                             std::array<SeqType, trading_protocols>& rec_data)
{
    SeqType time = rec_data[Trading::t_time];
    SeqType frame = rec_data[Trading::t_frame];

    SeqType side = rec_data[Trading::t_side];
    SeqType qty = rec_data[Trading::t_qty];
    SeqType bidpx = rec_data[Trading::t_price];
    SeqType messType = rec_data[Trading::t_msg];
    SeqType stock = rec_data[Trading::t_stock];

    SeqType adjpx = rec_data[Trading::t_adjprice];

    FIX44::MarketDataSnapshotFullRefresh mdsfr =
        FIX44::MarketDataSnapshotFullRefresh();

    FIX44::MarketDataSnapshotFullRefresh::NoMDEntries md;

    FIX::MDEntryTime mdt = FIX::MDEntryTime();
    mdt.setString(std::to_string(time));
    md.setField(mdt);

    FIX::Factor fact = FIX::Factor(frame);
    md.setField(fact);

    md.set(FIX::MDEntryPx(bidpx));
    md.set(FIX::PriceDelta(adjpx));

    md.set(FIX::MDEntrySize(qty));

    md.set(FIX::OrderID(UUidGen()));

    FIX::MDEntryType mt = FIX::MDEntryType();
    mt.setString(std::to_string(side));
    md.setField(mt);

    mdsfr.addGroup(md);

    FIX::MDReqID mid = FIX::MDReqID();
    mid.setString(std::to_string(messType));
    mdsfr.setField(mid);

    FIX::Symbol symbol = FIX::Symbol();
    symbol.setString(std::to_string(stock));
    mdsfr.setField(symbol);

    try {
        FIX::Session::sendToTarget(mdsfr, sid);
    }
    catch (FIX::SessionNotFound& s) {
        log::bug(s.what());
    }

} /* -----  end of function FixGuard::MarketMessage  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixGuard::CustemRequest
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixGuard::CustemRequest(const FIX::SessionID& sid, std::string& data,
                             int sz)
{
    FIX44::BidResponse lss = FIX44::BidResponse();
    FIX44::BidResponse::NoBidComponents nbc;

    FIX::Price price;
    FIX::ListID lid;

    price.setValue(sz);
    lid.setValue(data);

    nbc.setField(price);
    nbc.setField(lid);

    lss.addGroup(nbc);

    try {
        FIX::Session::sendToTarget(lss, sid);
    }
    catch (FIX::SessionNotFound& e) {
        log::bug(e.what());
    }
} /* -----  end of function FixGuard::CustemRequest  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixGuard::updateOrder
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixGuard::updateOrder(const OrderLots& order, char status, double equity)
{
    if (order.clOrdId.length() == 0) {
        log::echo("clordis ie empty");
        return;
    }
    /**
     * 4. {sender compid process } risk log
     */
    FIX::SessionID sid = order.owner;

    FIX::OrdStatus ostat;
    ostat.setValue(status);
    FIX::ExecType exec;
    std::size_t deci = 3;

    if (status == FIX::OrdStatus_FILLED ||
        status == FIX::OrdStatus_PARTIALLY_FILLED) {
        exec.setValue(FIX::ExecType_TRADE);
    }
    else if (status == FIX::OrdStatus_CANCELED) {
        exec.setValue(FIX::OrdStatus_CANCELED);
    }
    else {
        exec.setValue(FIX::ExecType_NEW);
    }

    FIX44::ExecutionReport executionReport = FIX44::ExecutionReport(
        order.clOrdId, genExecID(), exec, ostat, convert(order.side),
        FIX::LeavesQty(order.leavesQty), FIX::CumQty(order.executedQuantity),
        FIX::AvgPx(order.avgExecutedPrice));

    FIX::ComplianceID fquantId;
    fquantId.setValue(std::to_string(order.quantId));
    executionReport.setField(fquantId);

    FIX::OrderID oid = FIX::OrderID(std::to_string(order.ticket));

    executionReport.setField(oid);
    executionReport.set(FIX::ClOrdID(order.clOrdId));
    FIX::Symbol sym;
    sym.setString(order.symbol);
    executionReport.set(sym);
    executionReport.set(ostat);
    FIX::OrdType otype = convert(order.type);
    executionReport.setField(otype);

    FIX::TradeDate tradeDate;
    tradeDate.setValue(std::to_string(order.ctime));
    executionReport.setField(tradeDate);

    executionReport.set(FIX::OrderQty(order.quantity));

    // 权益
    FIX::SettlCurrFxRate sfr;
    sfr.setValue(equity);
    executionReport.setField(sfr);

    if (order.TradeTicket > 0 && order.executedQuantity > 0) {
        FIX::SecondaryOrderID soid;
        soid.setValue(std::to_string(order.TradeTicket));
        executionReport.setField(soid);
    }

    if (status != FIX::OrdStatus_CANCELED) {
        std::size_t idx = GlobalDBPtr->getId();
        Pgsql* gsql = GlobalDBPtr->ptr(idx);
        if (gsql == nullptr) {
            GlobalDBPtr->release(idx);
            return;
        }
        int cfi = 0;
        for (auto it : FinFabr->_fix_symbols) {
            if (it.second == order.symbol) {
                cfi = it.first;
                break;
            }
        }
        std::string cfi_str = log::format(
            "(SELECT id FROM stockinfo WHERE symbol=%d ORDER BY id DESC LIMIT "
            "1 )",
            cfi);
        gsql->insert_table("trades");
        gsql->insert_field("symbol", cfi_str);
        gsql->insert_field("ticket", order.ticket);
        gsql->insert_field("stat", (int)convert(ostat));
        gsql->insert_field("side", (int)order.side);
        gsql->insert_field("qty", order.quantity);

        /**
         *  目前先一笔交易，到时候再优化多笔的交易
         */
        if (order.side == e2::Side::os_Buy) {
            if (status != FIX::OrdStatus_NEW) {
                gsql->insert_field("openqty", order.openQuantity);
            }

            if (status == FIX::OrdStatus_NEW) {
                // log::info("new price:", order.price);
                gsql->insert_field("price", order.price, deci);
            }
            else {
                // log::info("trade price:", order.lastExecutedPrice);

                gsql->insert_field("price", order.lastExecutedPrice, deci);
            }

            gsql->insert_field("stoppx", 0);
        }
        else {
            gsql->insert_field("price", 0);
            gsql->insert_field("openqty", 0);
            if (status == FIX::OrdStatus_NEW) {
                gsql->insert_field("stoppx", order.price, deci);
            }
            else {
                gsql->insert_field("stoppx", order.lastExecutedPrice, deci);
            }
        }

        gsql->insert_field("slippage", 0);
        gsql->insert_field("ordtype", (int)order.type);

        if (status == FIX::OrdStatus_FILLED ||
            status == FIX::OrdStatus_PARTIALLY_FILLED) {
            executionReport.set(FIX::LastQty(order.lastExecutedQuantity));

            executionReport.set(FIX::LastPx(order.lastExecutedPrice));
            gsql->insert_field("cumqty", order.executedQuantity);
            gsql->insert_field("avgpx", order.avgExecutedPrice, deci);

            gsql->insert_field("leavesqty", order.leavesQty);
            gsql->insert_field("amount", order.trade_amount);
        }
        else {
            gsql->insert_field("cumqty", 0);
            gsql->insert_field("avgpx", 0);

            gsql->insert_field("leavesqty", 0);
        }

        gsql->insert_field("closetck", order.TradeTicket);
        gsql->insert_field("ctime", order.ctime);
        gsql->insert_field("otime", order.otime);

        gsql->insert_field("quantid", order.quantId);

        executionReport.set(FIX::LastParPx(order.adjprice));
        gsql->insert_field("adjpx", order.adjprice, deci);

        gsql->insert_commit();

        if (order.TradeTicket > 0 && order.lastExecutedQuantity > 0) {
            // sell update buy ticket openqty
            gsql->update_table("trades");

            std::string incqty =
                " openqty - " + std::to_string(order.lastExecutedQuantity);
            gsql->update_field("openqty", incqty);

            gsql->update_condition("ticket", order.TradeTicket);
            gsql->update_condition("stat", 2);

            gsql->update_commit();

            /**
             * 如果这个订单是在分红之前的话，就得加一个 分红的值
             */
        }

        GlobalDBPtr->release(idx);
    }
    try {
        FIX::Session::sendToTarget(executionReport, sid);
    }
    catch (FIX::SessionNotFound& e) {
        log::bug(e.what());
    }
} /* -----  end of function FixGuard::updateOrder  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixGuard::convert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Side FixGuard::convert(const FIX::Side& side)
{
    switch (side) {
        case FIX::Side_BUY:
            return e2::Side::os_Buy;
        case FIX::Side_SELL:
            return e2::Side::os_Sell;
        default:
            return e2::Side::os_Buy;
            // throw std::logic_error("Unsupported Side, use buy or sell");
    }
}
FIX::Side FixGuard::convert(e2::Side side)
{
    switch (side) {
        case e2::Side::os_Buy:
            return FIX::Side(FIX::Side_BUY);
        case e2::Side::os_Sell:
            return FIX::Side(FIX::Side_SELL);
        default:
            return FIX::Side(FIX::Side_BUY);
            // throw std::logic_error("Unsupported Side, use buy or sell");
    }
}

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixGuard::convert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::OrdType FixGuard::convert(const FIX::OrdType& type)
{
    switch (type) {
        case FIX::OrdType_LIMIT:
            return e2::OrdType::ot_limit;
        case FIX::OrdType_MARKET:
            return e2::OrdType::ot_market;
        case FIX::OrdType_STOP:
            return e2::OrdType::ot_stop;
        case FIX::OrdType_STOP_LIMIT:
            return e2::OrdType::ot_stop_limit;
        default:
            return e2::OrdType::ot_limit;
            // throw std::logic_error("Unsupported Order Type, use limit");
    }
}
FIX::OrdType FixGuard::convert(e2::OrdType type)
{
    switch (type) {
        case e2::OrdType::ot_limit:
            return FIX::OrdType(FIX::OrdType_LIMIT);
        case e2::OrdType::ot_market:
            return FIX::OrdType(FIX::OrdType_MARKET);
        case e2::OrdType::ot_stop:
            return FIX::OrdType(FIX::OrdType_STOP);
        case e2::OrdType::ot_stop_limit:
            return FIX::OrdType(FIX::OrdType_STOP_LIMIT);
        default:
            return FIX::OrdType(FIX::OrdType_LIMIT);
            //            throw std::logic_error("Unsupported Order Type, use
            //            limit");
    }
}
/* -----  end of function FixGuard::convert  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixGuard::convert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::OrdStatus FixGuard::convert(const FIX::OrdStatus& st)
{
    switch (st) {
        case FIX::OrdStatus_FILLED:
            return e2::OrdStatus::ost_Filled;
        case FIX::OrdStatus_PARTIALLY_FILLED:
            return e2::OrdStatus::ost_Partially_filled;
        default:
            return e2::OrdStatus::ost_New;
    }
}

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixGuard::convert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
FIX::OrdStatus FixGuard::convert(e2::OrdStatus ost)
{
    switch (ost) {
        case e2::OrdStatus::ost_Filled:
            return FIX::OrdStatus_FILLED;
        case e2::OrdStatus::ost_Partially_filled:
            return FIX::OrdStatus_PARTIALLY_FILLED;
        default:
            return FIX::OrdStatus_NEW;
    }
}
/* -----  end of function FixGuard::convert  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixGuard::convert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::TimeInForce FixGuard::convert(const FIX::TimeInForce& tif)
{
    switch (tif) {
        case FIX::TimeInForce_DAY:
            return e2::TimeInForce::tif_day;
        case FIX::TimeInForce_FILL_OR_KILL:
            // FOK
            return e2::TimeInForce::tif_fill_or_kill;
        case FIX::TimeInForce_IMMEDIATE_OR_CANCEL:
            // IOC
            return e2::TimeInForce::tif_immediate_or_cancel;
        default:
            return e2::TimeInForce::tif_day;
    }
} /* -----  end of function FixGuard::convert  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixGuard::convert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
FIX::TimeInForce FixGuard::convert(e2::TimeInForce tf)
{
    switch (tf) {
        case e2::TimeInForce::tif_day:
            return FIX::TimeInForce_DAY;
        case e2::TimeInForce::tif_fill_or_kill:
            // FOK
            return FIX::TimeInForce_FILL_OR_KILL;
        case e2::TimeInForce::tif_immediate_or_cancel:
            // IOC
            return FIX::TimeInForce_IMMEDIATE_OR_CANCEL;
        default:
            return FIX::TimeInForce_DAY;
    }
} /* -----  end of function FixGuard::convert  ----- */

}  // namespace e2q
