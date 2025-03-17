/*
 * =====================================================================================
 *
 *       Filename:  OrderMatcher.cpp
 *
 *    Description:  OrderMatcher
 *
 *        Version:  1.0
 *        Created:  2024年03月07日 10时57分29秒
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
#include "OMSPack/OrderBook/OrderMatcher.hpp"

#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <utility>

#include "assembler/BaseType.hpp"

namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderMatcher::insert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool OrderMatcher::insert(OrderItem* order)
{
    BasicLock _lock(_OMMutex);
    std::string sym = order->getSymbol();
    MarketType::iterator i = _markets->find(sym);
    Market* _mark = nullptr;
    if (i == _markets->end()) {
        _mark = MALLOC(Market);
        if (_mark->insert(order)) {
            _markets->insert(std::make_pair(sym, _mark));
        }
    }
    else {
        _mark = i->second;
        if (_mark == nullptr) {
            log::bug("fetch bug");
            return false;
        }

        _mark->insert(order);
    }

    return true;
} /* -----  end of function OrderMatcher::insert  ----- */

void OrderMatcher::erase(OrderItem* order)
{
    /* MarketMap::iterator i = _markets->find(order.getSymbol()); */
    /* if (i == _markets->end()) return; */
    /* i->second.erase(order); */
}

OrderItem* OrderMatcher::find(std::string symbol, std::size_t ticket)
{
    MarketType::iterator i = _markets->find(symbol);
    if (i == _markets->end()) return nullptr;
    return i->second->find(ticket);
}

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderMatcher::mprice
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void OrderMatcher::mprice(e2::Int_e p)
{
    _market_price = p;
} /* -----  end of function OrderMatcher::mprice  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderMatcher::OrderSize
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::size_t OrderMatcher::OrderSize(std::string sym)
{
    std::size_t len = 0;
    if (_markets->count(sym) > 0) {
        len = _markets->at(sym)->length();
    }
    return len;
} /* -----  end of function OrderMatcher::OrderSize  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderMatcher::OrderSizes
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::size_t OrderMatcher::OrderSizes()
{
    std::size_t len = 0;
    for (auto it = _markets->begin(); it != _markets->end(); it++) {
        len += it->second->length();
    }
    return len;
} /* -----  end of function OrderMatcher::OrderSizes  ----- */

/**
 *
 * 目前还得需要增加对手盘,否则交易不成功的
 */
bool OrderMatcher::match(std::string symbol, std::queue<OrderLots>& lots,
                         e2::Int_e market_price, e2::Int_e adj_price)
{
    BasicLock _lock(_OMMutex);

    MarketType::iterator i = _markets->find(symbol);
    if (i == _markets->end()) {
        return false;
    }
    return i->second->match(lots, market_price, adj_price);
}

bool OrderMatcher::match(std::queue<OrderLots>& lots, e2::Int_e adj_price)
{
    BasicLock _lock(_OMMutex);

    MarketType::iterator i;
    for (i = _markets->begin(); i != _markets->end(); ++i)
        i->second->match(lots, _market_price, adj_price);
    return lots.size() != 0;
}

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderMatcher::AddBotTicket
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int OrderMatcher::AddBotTicket(SeqType ticket, e2::OrdType ordType,
                               e2::Side side, double bot_qty, e2::Int_e symbol)
{
    FIX::SessionID sid;
    e2::Side bside = e2::Side::os_Sell;

    auto it = FinFabr->_stock.at(symbol);
    SeqType ctime = it[Trading::t_time];
    double order_price = it[Trading::t_price];
    if (order_price <= 0) {
        UtilTime ut;
        const char fmt[] = "%Y-%m-%d %H:%M:%S";
        log::echo("sym:", symbol, " order_price:", order_price, " - ",
                  ut.stamptostr(ctime, fmt));
        return -1;
    }
    if (FinFabr->lob == BotLob::giveaway) {
        // long bot_qty = order_qty;
        if (side == e2::Side::os_Buy) {
            bside = e2::Side::os_Sell;
        }
        else {
            bside = e2::Side::os_Buy;
        }

        FIX::Symbol symfix;
        e2::Int_e qid_bot = 0;
        symfix.setValue(FinFabr->_fix_symbols[symbol]);
        OrderItem* bot =
            MALLOC(OrderItem, ticket, "", symfix, sid, bside, ordType,
                   order_price, bot_qty, qid_bot, ctime, ctime);

        bot->Bot();

        insert(bot);
    }
    return 0;
} /* -----  end of function OrderMatcher::AddBotTicket  ----- */

void OrderMatcher::display(std::string symbol) const
{
    MarketType::const_iterator i = _markets->find(symbol);
    if (i == _markets->end()) return;
    i->second->display();
}

void OrderMatcher::display() const
{
    std::cout << "SYMBOLS:" << std::endl;
    std::cout << "--------" << std::endl;

    MarketType::const_iterator i;
    for (i = _markets->begin(); i != _markets->end(); ++i) {
        std::cout << i->first << std::endl;
        i->second->display();
        std::cout << "--------" << std::endl;
    }
}

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderMatcher::OrdTypePending
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool OrderMatcher::OrdTypePending()
{
    bool ret = true;

    return ret;
} /* -----  end of function OrderMatcher::OrdTypePending  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OrderMatcher::TopLevelPrice
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void OrderMatcher::TopLevelPrice(const std::string& symbol, SeqType val)
{
    std::pair<e2::Int_e, e2::Int_e> tlp{0, 0};

    if (_markets == nullptr) {
        log::bug("_market is nullptr!");
        return;
    }

    if (_markets->empty()) {
        return;
    }
    if (_markets->count(symbol) == 0) {
        return;
    }
    MarketType::const_iterator i = _markets->find(symbol);
    if (i == _markets->end()) return;
    tlp = i->second->top_bid_ask_price();
    /* log::echo("sym:", symbol, " size:", _markets->size(), " bid:", tlp.first,
     */
    /*           " ask:", tlp.second, " now:", val); */
} /* -----  end of function OrderMatcher::TopLevelPrice  ----- */
}  // namespace e2q
