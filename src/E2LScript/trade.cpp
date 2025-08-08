/*
 * =====================================================================================
 *
 *       Filename:  trade.cpp
 *
 *    Description:  trade
 *
 *        Version:  1.0
 *        Created:  2023年12月25日 14时22分58秒
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

#include "E2L/trade.hpp"

#include <cstddef>
#include <string>
#include <thread>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "E2LScript/e2lLead.hpp"
#include "E2LScript/foreign.hpp"
#include "E2LScript/util_inline.hpp"
#include "OMSPack/FixAccount.hpp"
#include "TradePack/STLog.hpp"
#include "assembler/BaseType.hpp"
#include "utility/Log.hpp"
namespace e2l {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderClose
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  Closes opened order
 *  NewOrderSingle
 *
 *  Side
 *  if ticket is long now it's short
 *  if ticket is short now it's long
 * ============================================
 */
e2::Bool OrderClose(
    e2::Int_e ticket,  // ticket   Unique number of the order ticket.
    e2::Int_e lots,    // volume Number of lots.
    e2::Int_e stoppx,  // Required for OrdType = Stop or OrdType = Stop limit.
    e2::Int_e
        slippage  // slippage  Value of the maximum price slippage in points.
)
{
    ticket = NUMBERVAL(ticket);
    lots = NUMBERVAL(lots);

    std::size_t exit = e2q::FixPtr->_OrderTicket.count(ticket);
    if (exit == 0) {
        log::bug("ticket not found:", ticket);
        return e2::Bool::B_FALSE;
    }

    std::string cl0id = e2q::FixPtr->_OrderTicket[ticket];
    std::thread::id _id = std::this_thread::get_id();

    std::size_t quantid = 0;
    if (e2q::FixPtr->_quantId.count(_id) == 1) {
        quantid = e2q::FixPtr->_quantId[_id].first;
    }
    if (e2q::FixPtr->_OrderIds.count(quantid) == 0 ||
        e2q::FixPtr->_OrderIds[quantid].count(cl0id) == 0) {
        log::bug("ticket:", ticket, " quantid ==0");

        return e2::Bool::B_FALSE;
    }

    e2q::OrderInfo oi = e2q::FixPtr->_OrderIds[quantid][cl0id];

    if (oi.closeTck > 0 || stoppx <= 0) {
        // 防止 传入 平仓的 ticket ID
        // log::bug("it'is close ticket:", ticket, " stoppx:", stoppx);
        return e2::Bool::B_FALSE;
    }

    if (oi.trading != e2q::TradeStatus::MARKET) {
        return e2::Bool::B_FALSE;
    }
    if (oi.trading == e2q::TradeStatus::CLOSEING) {
        return e2::Bool::B_FALSE;
    }

    if (oi.openqty == 0) {
        return e2::Bool::B_FALSE;
    }
    /* if (oi.ordtype == e2::OrdType::ot_stop || */
    /*     oi.ordtype == e2::OrdType::ot_stop_limit) { */
    /*     // close order can't close again */
    /*     log::bug("close order can't close again, ticket:", ticket); */
    /*     return e2::Bool::B_FALSE; */
    /* } */

    e2::Side side = oi.side;
    if (side == e2::Side::os_Buy) {
        side = e2::Side::os_Sell;
    }
    else {
        side = e2::Side::os_Buy;
    }
    if (lots > oi.qty) {
        lots = oi.qty;
    }
    e2q::FixPtr->_OrderIds[quantid][cl0id].trading = e2q::TradeStatus::CLOSEING;
    std::size_t number = e2q::e2l_thread_map.number(_id);

    /**
     * ot_stop == buy buystop
     */

    e2q::fix_application.OrderReplaceRequest(
        oi.symbol, side, lots, stoppx, slippage, e2::OrdType::ot_stop, ticket,
        quantid, e2q::ticket_now, number);

    // log::echo("close tick:", ticket, " sym:", oi.symbol);

    return e2::Bool::B_TRUE;
}
/* -----  end of function OrderClose  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderSend
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  The main function used to open an order or place a pending order
 *  fix NewOrderSingle
 *
 *  return order no
 * ============================================
 */

e2::Bool OrderSend(e2::Int_e symbol,    // symbol  Symbol for trading.
                   e2::Side side,       // operation Operation type.
                   e2::Int_e qty,       // volume Number of lots.
                   e2::Int_e price,     // price Order price.
                   e2::Int_e slippage,  // slippage   Maximum price slippage for
                                        // buy or sell orders.
                   e2::OrdType ordtype  // stop loss Stop loss level.

)
{
    if (symbol < 0 || price <= 0) {
        log::bug("symbol == 0");
        return e2::Bool::B_FALSE;
    }

    symbol = NUMBERVAL(symbol);
    qty = NUMBERVAL(qty);
    std::thread::id _id = std::this_thread::get_id();

    if (e2q::FixPtr->_fix_symbols.count(symbol) == 0) {
        log::bug("id == 0, id:", id);

        return e2::Bool::B_FALSE;
    }

    /**
     * 简单计算 一下钱够不够
     */
    double expenditure = e2q::FixPtr->equity(price, qty);
    std::size_t number = e2q::e2l_thread_map.number(_id);

    double free_cash = e2q::FixPtr->_cash.TotalCash(number) -
                       e2q::FixPtr->_cash.FreezeCash(number);

    // 防止一下子下多笔订单，因为 oms 还没有返回确认是不是下单成功，
    // 所以先扣一笔资金
    if (expenditure > free_cash) {
        std::string cond = log::format(
            "expenditure: %.2f total cash:%.2f, freeze:%.2f number:%ld",
            expenditure, e2q::FixPtr->_cash.TotalCash(number),
            e2q::FixPtr->_cash.FreezeCash(number), number);
        log::bug(cond);
        return e2::Bool::B_FALSE;
    }

    // 信号多次触发，就会多次笔下单了，虽然可能是前端控制，或者这儿加一个 api
    // 来处不吧,多线程的话，也会同时下多笔订单，以后再优化
    if (e2q::FixPtr->_freeze_time.count(symbol) == 0) {
        e2q::FixPtr->_freeze_time.insert({symbol, 0});
    }
    // if (e2q::FixPtr->_freeze_time.at(symbol) == e2q::ticket_now) {
    //     log::bug("has order");
    //     return e2::Bool::B_FALSE;
    // }
    e2q::FixPtr->_freeze_time[symbol] = e2q::ticket_now;

    side = (e2::Side)NUMBERVAL(side);

    ordtype = (e2::OrdType)NUMBERVAL(ordtype);

    std::size_t quantid = 0;
    if (e2q::FixPtr->_quantId.count(_id) == 1) {
        quantid = e2q::FixPtr->_quantId[_id].first;
    }

    e2q::FixPtr->_cash.add_freeze(number, expenditure);

    e2q::fix_application.NewOrderSingle(symbol, side, qty, price, slippage,
                                        ordtype, quantid, e2q::ticket_now,
                                        number);

    return e2::Bool::B_TRUE;
} /* -----  end of function OrderSend  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderSelect
 *  ->  void *
 *  Parameters:
 *  - int_e index // index or order ticket
 *  - SelectFlag sel // SELECT_BY_POS - index in the order pool,
 *                   // SELECT_BY_TICKET - index is order ticket.
 *  - SelectFlag pool // MODE_TRADES
 *                   // MODE_HISTORY
 *  Description:
 *
 * ============================================
 */
e2::Bool OrderSelect(e2::Int_e index, e2::SelectFlag sel, e2::SelectFlag pool)
{
    index = NUMBERVAL(index);
    sel = (e2::SelectFlag)NUMBERVAL(sel);
    pool = (e2::SelectFlag)NUMBERVAL(pool);

    std::thread::id _id = std::this_thread::get_id();

    std::size_t quantid = 0;
    if (e2q::FixPtr->_quantId.count(_id) == 1) {
        quantid = e2q::FixPtr->_quantId[_id].first;
    }
    if (e2q::FixPtr->_OrderIds.count(quantid) == 0) {
        log::info("quantid == 0:", quantid);
        return e2::Bool::B_FALSE;
    }

    if (pool == e2::SelectFlag::P_Trade &&
        e2q::FixPtr->_OrderIds[quantid].size() == 0) {
        // no orders
        log::info("no orders:", quantid);
        return e2::Bool::B_FALSE;
    }

    // else {
    //    e2::SelectFlag::P_History
    // }
    e2q::e2_os.insert(_id, index, sel, pool);

    return e2::Bool::B_TRUE;
} /* -----  end of function OrderSelect  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderTicket
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e OrderTicket()
{
    e2q::SeqType tick = 0;
    std::thread::id _id = std::this_thread::get_id();

    if (e2q::e2_os.check(_id)) {
#ifdef DEBUG
        log::bug(" id empty:", _id);
#endif
        return tick;
    }

    e2q::OrderStruct os = e2q::e2_os.get(_id);
    if (os.id == -1) {
#ifdef DEBUG
        log::bug(" id empty:", _id);
#endif
        return tick;
    }
    if (os.select == e2::SelectFlag::F_ByTicket) {
        e2q::e2_os.release(_id);

#ifdef DEBUG
        log::bug("select error:", os.select);
#endif
        return VALNUMBER(os.id);
    }
    std::size_t m = 0;

    std::size_t quantId = 0;
    if (e2q::FixPtr->_quantId.count(_id) == 1) {
        quantId = e2q::FixPtr->_quantId[_id].first;
    }

    if (e2q::FixPtr->_OrderIds.count(quantId) == 0) {
        log::bug("quaniId:", quantId);
        return tick;
    }

    if (os.pool == e2::SelectFlag::P_Trade) {
        // trade
        if ((std::size_t)os.id >= e2q::FixPtr->_OrderIds[quantId].size()) {
            e2q::e2_os.release(_id);
#ifdef DEBUG
            log::bug(" id > size:", _id);
#endif
            return tick;
        }
        for (auto it = e2q::FixPtr->_OrderIds[quantId].begin();
             it != e2q::FixPtr->_OrderIds[quantId].end(); ++it) {
            // os.select == e2::SelectFlag::F_ByPos)

            //  it->second.trading == e2q::TradeStatus::CLOSEING
            //  不增加这个状态，因为有一个 平仓的订单会把 market 设置为 closeing
            //  的
            if (it->second.trading == e2q::TradeStatus::MARKET ||
                it->second.trading == e2q::TradeStatus::PARTIALLY_FILLED ||
                it->second.trading == e2q::TradeStatus::PENDING) {
                if (m == (std::size_t)os.id) {
                    tick = it->second.ticket;
                    // log::echo("trading:", it->second.trading,
                    //           " ticket:", it->second.ticket, " m:", m,
                    //           " os.id:", os.id, " quantid:", quantId);
                    break;
                }
                m++;
            }
        }
    }
    else {
        // history
    }
    e2q::e2_os.release(_id);

    return VALNUMBER(tick);
} /* -----  end of function OrderTicket  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderLots
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e OrderLots(e2::Int_e ticket)
{
    std::thread::id _id = std::this_thread::get_id();
    ticket = NUMBERVAL(ticket);

    e2::Int_e ret = 0;
    std::size_t quantId = 0;
    if (e2q::FixPtr->_quantId.count(_id) == 1) {
        quantId = e2q::FixPtr->_quantId[_id].first;
    }
    if (e2q::FixPtr->_OrderIds.count(quantId) == 0) {
        log::bug("quaniId:", quantId);
        return ret;
    }
    for (auto it = e2q::FixPtr->_OrderIds[quantId].begin();
         it != e2q::FixPtr->_OrderIds[quantId].end(); ++it) {
        if (it->second.ticket == ticket) {
            ret = it->second.openqty;

            break;
        }
    }
    return VALNUMBER(ret);
} /* -----  end of function OrderLots  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrdersHistoryTotal
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  Returns the number of closed orders in the account history loaded into the
 * terminal.
 * ============================================
 */
e2::Int_e OrdersHistoryTotal()
{
    std::size_t count = 0;
    std::thread::id _id = std::this_thread::get_id();
    std::size_t quantid = 0;
    if (e2q::FixPtr->_quantId.count(_id) == 1) {
        quantid = e2q::FixPtr->_quantId[_id].first;
    }
    if (e2q::FixPtr->_OrderIds.count(quantid) == 0) {
        return 0;
    }
    for (auto it : e2q::FixPtr->_OrderIds[quantid]) {
        if (it.second.trading == e2q::TradeStatus::CANCEL) {
            log::info("qid:", quantid);
        }
        if (it.second.trading == e2q::TradeStatus::FILLED ||
            it.second.trading == e2q::TradeStatus::CLOSED ||
            it.second.trading == e2q::TradeStatus::REJECT ||
            it.second.trading == e2q::TradeStatus::CANCEL) {
            count++;
        }
    }
    return VALNUMBER(count);
} /* -----  end of function OrdersHistoryTotal  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrdersTotal
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  market and pending ordres
 * ============================================
 */
e2::Int_e OrdersTotal()
{
    std::size_t count = 0;
    std::thread::id _id = std::this_thread::get_id();
    std::size_t quantid = 0;
    if (e2q::FixPtr->_quantId.count(_id) == 1) {
        quantid = e2q::FixPtr->_quantId[_id].first;
    }
    if (e2q::FixPtr->_OrderIds.count(quantid) == 0) {
        return 0;
    }
    for (auto it : e2q::FixPtr->_OrderIds[quantid]) {
        if (it.second.trading == e2q::TradeStatus::PENDING ||
            it.second.trading == e2q::TradeStatus::PARTIALLY_FILLED ||
            it.second.trading == e2q::TradeStatus::MARKET ||
            it.second.trading == e2q::TradeStatus::CLOSEING) {
            // log::info("trading:", it.second.trading);
            count++;
        }
    }
    return VALNUMBER(count);
} /* -----  end of function OrdersTotal  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderComment
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void OrderComment(e2::Int_e ticket, e2::Side side, e2::OrderEvent oe)
{
    std::thread::id _id = std::this_thread::get_id();
    std::size_t quantid = 0;
    ticket = NUMBERVAL(ticket);

    if (e2q::FixPtr->_quantId.count(_id) == 1) {
        quantid = e2q::FixPtr->_quantId[_id].first;
    }
    std::size_t _ticket = ticket;
    int _side = (int)NUMBERVAL(side);
    int _oe = (int)NUMBERVAL(oe);
    e2q::STLog slog;
    slog.TicketComment(quantid, _ticket, _side, _oe);

} /* -----  end of function OrderComment  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderOpenPrice
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  返回 ticket 的成交价
 * ============================================
 */
e2::Int_e OrderOpenPrice(e2::Int_e ticket, e2::Bool b)
{
    std::thread::id _id = std::this_thread::get_id();
    std::size_t quantid = 0;
    if (e2q::FixPtr->_quantId.count(_id) == 1) {
        quantid = e2q::FixPtr->_quantId.at(_id).first;
    }

    if (e2q::FixPtr->_OrderIds.count(quantid) == 0) {
        log::bug("qid: ", quantid, " error");

        return 0;
    }
    double px = 0.0;
    // e2::Int_e _ticket = ticket;
    //  log::echo("_tick:", _ticket);
    e2::Int_e _ticket = NUMBERVAL(ticket);
    for (auto it = e2q::FixPtr->_OrderIds[quantid].begin();
         it != e2q::FixPtr->_OrderIds[quantid].end(); ++it) {
        if (it->second.ticket == _ticket && it->second.adjpx > 0) {
            if (b == e2::Bool::B_FALSE) {
                px = it->second.avgpx;
            }
            else {
                px = it->second.adjpx;
            }
            // log::echo("px:", px);
            break;
        }
    }

    return px;
} /* -----  end of function OrderOpenPrice  ----- */

}  // namespace e2l
