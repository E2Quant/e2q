/*
 * =====================================================================================
 *
 *       Filename:  Market.cpp
 *
 *    Description:  Market
 *
 *        Version:  1.0
 *        Created:  2024年03月07日 09时39分10秒
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
#include "OMSPack/OrderBook/Market.hpp"

#include <algorithm>
#include <cstddef>
#include <utility>

#include "E2L/E2LType.hpp"
#include "assembler/BaseType.hpp"
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Market::insert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool Market::insert(OrderItem* order)
{
    if (order->Pending() == nullptr) {
        log::bug("order pending is nullptr!");
        return false;
    }
    if (order->getSide() == e2::Side::os_Buy) {
        _bidOrders.insert(order->Pending());
    }
    else {
        _askOrders.insert(order->Pending());
    }

    MapOrder.insert({order->Pending()->getTicket(), order});

    return true;
}
/* -----  end of function Market::insert  ----- */
void Market::erase(OrderItem* order)
{
    // size_t ticket = order.getTicket();
    if (order->getSide() == e2::Side::os_Buy) {
        /* BidOrders::iterator i; */
        /* for (i = _bidOrders.begin(); i != _bidOrders.end(); ++i) */
        /*     if (i->second.getClOrdID() == id) { */
        /*         _bidOrders.erase(i); */
        /*         return; */
        /*     } */
    }
    else if (order->getSide() == e2::Side::os_Sell) {
        /* AskOrders::iterator i; */
        /* for (i = _askOrders.begin(); i != _askOrders.end(); ++i) */
        /*     if (i->second.getClOrdID() == id) { */
        /*         _askOrders.erase(i); */
        /*         return; */
        /*     } */
    }
}

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Market::length
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::size_t Market::length()
{
    std::size_t len = _bidOrders.size();
    len = _askOrders.size();
    return len;
} /* -----  end of function Market::length  ----- */

OrderItem* Market::find(std::size_t ticket)
{
    auto item = MapOrder.find(ticket);
    if (item == MapOrder.end()) {
        log::bug("not found ticket:", ticket);
        return nullptr;
    }

    return item->second;
}

/**
 * price/time
 */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Market::match
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  目前先处理 Limit or market
 *  Limit 由于是多线程多进程，有可能会产生 不同的的 order 顺序
 *  比如： 前一次测试在 low 成交了，后一次因为时间慢了，可能在 close 也成交不了
 *  就会出现前后两次不一样的 交易表单了
 *  market 只要提交都能交易
 *
 * ============================================
 */
bool Market::match(std::queue<OrderLots>& Lots, e2::Int_e mprice,
                   e2::Int_e adj_price, std::size_t order_start_time)
{
    /*
    log::info("--- === ----");
    _bidOrders.dump();
    log::bug("---  ----");

    _askOrders.dump();
    log::info("-------");
*/
    e2q::OrderPending* spread_bid = nullptr;
    e2q::OrderPending* spread_ask = nullptr;

    e2::OrdType ordtype;
    bool bempty = false;
    bool aempty = false;
    long quantity = 0;  // 有效值
    long ask_qty = 0;
    long bid_qty = 0;
    double price = 0;
    // double adjprice = 0;
    // OrderLots lots;
    while (true) {
        bempty = _bidOrders.empty();
        aempty = _askOrders.empty();
        if (bempty && aempty) {
            // log::echo("bid ask is empty");
            break;
        }

        // 订单可以过夜不?
        // 先默认删除订单，否则订单会不小心
        // 在策略的后面几天才成交，这样就会出错了
        if (bempty && !aempty) {
            spread_ask = _askOrders.spread();
            if (spread_ask != nullptr) {
                // if (FinFabr->_settlement == 0 ||
                //     order_start_time > spread_ask->market_time()) {
                log::info(spread_ask->isBot(),
                          " ask opqty:", spread_ask->getOpenQuantity(),
                          " leaveqty:", spread_ask->getLeavesQty(),
                          " tick:", spread_ask->getTicket(),
                          " order_start_time:", order_start_time,
                          " spred_ask:", spread_ask->market_time());

                spread_ask->cancel();
                spread_ask->Closeed();
                if (!spread_ask->isBot()) {
                    Lots.push(make(spread_ask->getTicket()));
                }
                //   }
            }
            continue;
        }
        if (!bempty && aempty) {
            spread_bid = _bidOrders.spread();
            if (spread_bid != nullptr) {
                // if (FinFabr->_settlement == 0 ||
                //     order_start_time > spread_bid->market_time()) {
                log::info(spread_bid->isBot(),
                          " bid opqty:", spread_bid->getOpenQuantity(),
                          " leaveqty:", spread_bid->getLeavesQty(),
                          " tick:", spread_bid->getTicket(),
                          " order_start_time:", order_start_time,
                          " spred_ask:", spread_bid->market_time());
                spread_bid->cancel();
                spread_bid->Closeed();
                if (!spread_bid->isBot()) {
                    Lots.push(make(spread_bid->getTicket()));
                }
                // }
            }
            continue;
        }

        spread_bid = _bidOrders.spread();
        spread_ask = _askOrders.spread();

        if (spread_ask == nullptr || spread_bid == nullptr) {
            // log::bug("ask or bid level  empty ");
            continue;
        }
        /*
        if (spread_ask->isBot()) {
            log::info(" ask opqty:", spread_ask->getOpenQuantity(),
                      "  price:", spread_ask->getAvgExecutedPrice(),
                      " leaveqty:", spread_ask->getLeavesQty(),
                      " exeqty:", spread_ask->getExecutedQuantity(),
                      " tick:", spread_ask->getTicket(),
                      " bid tick:", spread_bid->getTicket());
        }
        if (spread_bid->isBot()) {
            log::info(" bid opqty:", spread_bid->getOpenQuantity(),
                      "  price:", spread_bid->getAvgExecutedPrice(),
                      " leaveqty:", spread_bid->getLeavesQty(),
                      " exeqty:", spread_bid->getExecutedQuantity(),
                      " tick:", spread_bid->getTicket(),
                      " ask tick:", spread_ask->getTicket(),
                      " ask leaveqty:", spread_ask->getLeavesQty(),
                      " ask openqty:", spread_ask->getOpenQuantity());
        }*/
        /**
         * market price
         */
        price = spread_ask->getPrice();
        /**
         *check user ordtype
         */
        ordtype = spread_bid->getType();
        if (spread_bid->isBot()) {
            ordtype = spread_ask->getType();
            price = spread_bid->getPrice();
        }

        /**
         * match event
         */
        if (mprice > 0) {
            price = mprice;
        }
        if (ordtype == e2::OrdType::ot_limit) {
            if (spread_bid->getPrice() < price) {
                log::bug("price null eq, tick:", spread_bid->getTicket(),
                         " , bid:", spread_bid->getPrice(),
                         ", tick:", spread_ask->getTicket(),
                         " ask:", spread_ask->getPrice());

                /**
                 * disable order;
                 */
                spread_ask->disable();
                spread_bid->disable();
                break;
            }
        }

        bid_qty = spread_bid->getLeavesQty();
        ask_qty = spread_ask->getLeavesQty();
        quantity = min(bid_qty, ask_qty);

        spread_ask->Adj(adj_price);
        spread_bid->Adj(adj_price);

        ask_qty = spread_ask->execute(price, quantity);
        bid_qty = spread_bid->execute(price, quantity);

        if (!spread_ask->isBot()) {
            Lots.push(make(spread_ask->getTicket()));
        }
        if (!spread_bid->isBot()) {
            Lots.push(make(spread_bid->getTicket()));
        }
        // log::echo("bid qty:", bid_qty, " ask_qty:", ask_qty);

        /**
         * 平仓的时候扣掉 qty
         */
        if (spread_ask->isCloseOrder()) {
            spread_ask->Lots(ask_qty);
        }

        if (spread_bid->isCloseOrder()) {
            spread_bid->Lots(bid_qty);
        }
        /*
        if (spread_ask->isBot()) {
            log::info("end ask opqty:", spread_ask->getOpenQuantity(),
                      "  price:", spread_ask->getAvgExecutedPrice(),
                      " leaveqty:", spread_ask->getLeavesQty(),
                      " exeqty:", spread_ask->getExecutedQuantity(),
                      " lots qty:", quantity, " tick:", spread_ask->getTicket(),
                      " bid tick:", spread_bid->getTicket(),
                      " bid leaveqty:", spread_bid->getLeavesQty(),
                      " bid openqty:", spread_bid->getOpenQuantity());
        }
        if (spread_bid->isBot()) {
            log::info("end bid opqty:", spread_bid->getOpenQuantity(),
                      "  price:", spread_bid->getAvgExecutedPrice(),
                      " leaveqty:", spread_bid->getLeavesQty(),
                      " exeqty:", spread_bid->getExecutedQuantity(),
                      " lots qty:", quantity, " tick:", spread_bid->getTicket(),
                      " ask tick:", spread_ask->getTicket(),
                      " ask leaveqty:", spread_ask->getLeavesQty(),
                      " ask openqty:", spread_ask->getOpenQuantity());
        }*/
    }

    return Lots.size() != 0;
} /* -----  end of function Market::match  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Market::make
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
OrderLots Market::make(std::size_t ticket)
{
    OrderItem* it = find(ticket);
    OrderLots lots;
    if (it == nullptr) {
        return lots;
    }
    lots = it->Lots();
    return lots;
} /* -----  end of function Market::make  ----- */

void Market::display()
{
    std::cout << "BIDS:" << _bidOrders.size() << std::endl;
    _bidOrders.dump();
    std::cout << "ASKS:" << _askOrders.size() << std::endl;
    _askOrders.dump();
}

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Market::top_bid_ask_price
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::pair<e2::Int_e, e2::Int_e> Market::top_bid_ask_price()
{
    std::pair<e2::Int_e, e2::Int_e> tbap;
    std::pair<e2::Int_e, long> bid = _bidOrders.topValue();
    std::pair<e2::Int_e, long> ask = _askOrders.topValue();

    /*
       std::cout << "BIDS:" << _bidOrders.size() << " depth:" <<
       _bidOrders.depth()
       << std::endl;
       std::cout << "ASKS:" << _askOrders.size() << " depth:" <<
       _askOrders.depth()
       << std::endl;
       */
    tbap = std::make_pair(bid.first, ask.first);
    return tbap;
} /* -----  end of function Market::top_bid_ask_price  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Market::MDebug
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Market::MDebug(std::string& desz, const OrderPending* op)
{
    log::info(desz, " open qty:", op->getOpenQuantity(),
              " avg price:", op->getAvgExecutedPrice(),
              " tick:", op->getTicket(), " price:", op->getPrice());

} /* -----  end of function Market::MDebug  ----- */
}  // namespace e2q
