/*
 * =====================================================================================
 *
 *       Filename:  TraderAlgorithms.cpp
 *
 *    Description:  TraderAlgorithms
 *
 *        Version:  1.0
 *        Created:  2025/02/08 10时27分19秒
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
#include "OMSPack/Matcher/TraderAlgorithms.hpp"

#include "E2L/E2LType.hpp"
#include "Toolkit/Norm.hpp"
#include "assembler/BaseType.hpp"
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::InitSequence
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void TraderAlgorithms::InitSequence()
{
    SeqType inc = 0;
    std::size_t idx = GlobalDBPtr->getId();
    char* field = nullptr;
    char* val = nullptr;
    std::size_t last_ticket = 0;
    std::string sql = "SELECT ticket from trades ORDER BY id DESC LIMIT 1;";
    Pgsql* gsql = GlobalDBPtr->ptr(idx);
    if (gsql == nullptr) {
        GlobalDBPtr->release(idx);
        return;
    }
    bool r = SelectSQL(gsql, sql);
    if (r && gsql->tuple_size() > 0) {
        gsql->OneHead(&field, &val);
        if (val != nullptr) {
            last_ticket = atol(val);
        }
    }
    GlobalDBPtr->release(idx);

    e2q::UtilTime ut;

    if (last_ticket > 0) {
        inc = last_ticket + 1;
    }
    else {
        inc = ut.time() - offset;
    }
    _sequence = MALLOC(AutoIncrement, inc);
} /* -----  end of function TraderAlgorithms::InitSequence  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::sequence
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
SeqType TraderAlgorithms::sequence(const FIX::SessionID& sid, e2::Side)
{
    SeqType seq = 0;
    if (_sequence) {
        seq = _sequence->StoreId();
    }

    return seq;
} /* -----  end of function TraderAlgorithms::sequence  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::matcher
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::vector<OrderLots> TraderAlgorithms::matcher(std::string symbol,
                                                 e2::Int_e now, e2::Int_e price,
                                                 e2::Int_e adjprice)
{
    // T+1 ...
    std::size_t mod_time = 86400;
    std::size_t startTime = now - (now % mod_time);
    e2::Int_e match_now = now;
    e2::Int_e adj_now = adjprice;
    e2::Int_e market_price = price;
    mpType PreData;
    std::vector<OrderLots> retLots;

    std::size_t len = _orderMatcher->OrderSizes();
    if (len == 0) {
        elog::bug("symbole order matcher is empty:", symbol);
        return retLots;
    }
    if (FinFabr->_match_trigger == e2::Bool::B_FALSE) {
        _symbol_market_price.init(symbol);
        PreData = _symbol_market_price.get(symbol);

        if (FinFabr->_settlement == 0 ||
            FinFabr->_ME == e2::MatchEvent::ME_OrderIn) {
            market_price = 0;
        }
        else {
            if (PreData.PreTime == 0) {
                PreData.PreTime = startTime;
                PreData.PreNow = now;
                PreData.PrePrice = price;
                PreData.PreAdjPrice = adjprice;

                _symbol_market_price.set(symbol, PreData);

                return retLots;
            }

            if (startTime == PreData.PreTime) {
                PreData.PrePrice = price;
                PreData.PreAdjPrice = adjprice;

                _symbol_market_price.set(symbol, PreData);

                return retLots;
            }

            if (FinFabr->_ME == e2::MatchEvent::ME_Close) {
                market_price = PreData.PrePrice;
                match_now = PreData.PreNow;
                adj_now = PreData.PreAdjPrice;
            }
            else {
                market_price = price;
            }
        }
    }
    else {
        market_price = price;
    }

    std::queue<OrderLots> orders;
    elog::echo("symbol:", symbol);
    _orderMatcher->match(symbol, orders, market_price, adj_now, now);

    while (orders.size() > 0) {
        OrderLots ol = orders.front();
        ol.ctime = match_now;
        if (ol.executedQuantity == 0 && ol.ticket > 0) {
            std::size_t idx = GlobalDBPtr->getId();
            Pgsql* gsql = GlobalDBPtr->ptr(idx);
            if (gsql != nullptr) {
                gsql->update_table("trades");
                gsql->update_field(
                    "stat",
                    (int)e2::OrdStatus::ost_Canceled);  // OrdStatus: Canceled
                gsql->update_condition("ticket", ol.ticket);
                UpdateCommit(gsql);
            }
            GlobalDBPtr->release(idx);
            _broker.freeMargin(ol.owner, ol.ticket, 0);
            ol.isCancel = true;
        }
        else if (ol.quantId > 0) {
            /**
             * 结算, 只交易一部分的
             */
            if (adj_now > 0) {
                ol.adjprice = (double)NUMBERVAL(adj_now);
            }
            _broker.SettlInst(ol);

            ol.isCancel = false;
        }
        else {
            elog::bug("not match order ticket: ", ol.ticket);
        }

        retLots.push_back(ol);
        orders.pop();
    }
    if (FinFabr->_match_trigger == e2::Bool::B_FALSE) {
        PreData.PreTime = startTime;
        PreData.PreNow = now;

        PreData.PrePrice = price;
        PreData.PreAdjPrice = adjprice;
        _symbol_market_price.set(symbol, PreData);
    }
    return retLots;
} /* -----  end of function TraderAlgorithms::matcher  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::insert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool TraderAlgorithms::insert(OrderItem* order)
{
    return _orderMatcher->insert(order);
} /* -----  end of function TraderAlgorithms::insert  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::display
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void TraderAlgorithms::display()
{
    _orderMatcher->display();
} /* -----  end of function TraderAlgorithms::display  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::CheckClose
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e TraderAlgorithms::CheckClose(SeqType ticket,
                                       const std::string& symbol,
                                       e2::Int_e lots)
{
    OrderItem* oi = _orderMatcher->find(symbol, ticket);
    if (oi == nullptr) {
        elog::bug(" null found, tick:", ticket, " sym:", symbol);
        return 0;
    }
    if (oi->Pending()->isClosed()) {
        elog::bug("ticket:", ticket, " symbol:", symbol,
                  " oi ticket:", oi->Pending()->getTicket(),
                  " oi sym:", oi->getSymbol(),
                  " open:", oi->Pending()->getOpenQuantity(),
                  "leaveqty:", oi->Pending()->getLeavesQty());
        return 0;
    }

    std::size_t ctime = oi->getTime();
    std::size_t now = FinFabr->_stock.at(0)[Trading::t_time];
    std::size_t startTime = now - (now % 86400);

    switch (FinFabr->_settlement) {
        case 0:
            // T+0
            break;
        case 1: {
            // T+1
            // 当前买入的订单不能当天卖出
            if (ctime > startTime) {
                return 0;
            }

            break;
        }
        default:
            break;
    }

    long _lots = oi->Pending()->CloseOrder(lots);

    return _lots;
} /* -----  end of function TraderAlgorithms::CheckClose  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::AddBotTicket
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int TraderAlgorithms::AddBotTicket(SeqType ticket, e2::OrdType ordType,
                                   e2::Side side, double bot_qty,
                                   e2::Int_e symbol)
{
    return _orderMatcher->AddBotTicket(ticket, ordType, side, bot_qty, symbol);
} /* -----  end of function TraderAlgorithms::AddBotTicket  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::OrdTypePending
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool TraderAlgorithms::OrdTypePending()
{
    return _orderMatcher->OrdTypePending();
} /* -----  end of function TraderAlgorithms::OrdTypePending  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::TopLevelPrice
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void TraderAlgorithms::TopLevelPrice(const std::string& symbol, SeqType price)
{
    _orderMatcher->TopLevelPrice(symbol, price);
} /* -----  end of function TraderAlgorithms::TopLevelPrice  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::Equity
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
double TraderAlgorithms::Equity(const FIX::SessionID& sid, std::size_t ticket,
                                const char status)
{
    return _broker.Equity(sid, ticket, status);
} /* -----  end of function TraderAlgorithms::Equity  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::freeMargin
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void TraderAlgorithms::freeMargin(const FIX::SessionID& sid, std::size_t ticket,
                                  double margin)
{
    _broker.freeMargin(sid, ticket, margin);
} /* -----  end of function TraderAlgorithms::freeMargin  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::Margin
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool TraderAlgorithms::Margin(const FIX::SessionID& sid, std::size_t ticket,
                              double margin, long qty)
{
    return _broker.Margin(sid, ticket, margin, qty);
} /* -----  end of function TraderAlgorithms::Margin  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::CheckMargin
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
double TraderAlgorithms::CheckMargin(const FIX::SessionID& sid, double price,
                                     long qty)
{
    return _broker.CheckMargin(sid, price, qty);
} /* -----  end of function TraderAlgorithms::CheckMargin  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::traders
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
double TraderAlgorithms::traders(const FIX::SessionID& sid, double cash)
{
    // 返回总的实际资金
    double rcash = _broker.traders(sid, cash);
    return rcash;
} /* -----  end of function TraderAlgorithms::traders  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::ExdrChange
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   不知道是先加了 QTY 再计算分红，还是先分红再加QTY ，先放这儿吧
 * ============================================
 */
void TraderAlgorithms::ExdrChange(SeqType cfi, SeqType ticket, double cash,
                                  double qty, std::size_t ctime)
{
    _broker.AddExdrCash(ticket, cash, ctime);
    _broker.AddExdrQty(cfi, ticket, qty, ctime);
} /* -----  end of function TraderAlgorithms::ExdrChange  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TraderAlgorithms::SessionLogout
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void TraderAlgorithms::SessionLogout(const FIX::SessionID& sid)
{
    _broker.CloseSession(sid);
} /* -----  end of function TraderAlgorithms::SessionLogout  ----- */
}  // namespace e2q
