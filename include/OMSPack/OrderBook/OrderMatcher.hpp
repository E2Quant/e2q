/*
 * =====================================================================================
 *
 *       Filename:  OrderMatcher.hpp
 *
 *    Description:  OrderMatcher
 *
 *        Version:  1.0
 *        Created:  2024年01月10日 16时22分22秒
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
 *
 * =====================================================================================
 */

#ifndef ORDERMATCHER_INC
#define ORDERMATCHER_INC
#include <cstddef>
#include <map>
#include <queue>
#include <string>

#include "E2L/E2LType.hpp"
#include "OMSPack/OrderBook/Market.hpp"
#include "OMSPack/OrderBook/Order.hpp"
#include "assembler/BaseType.hpp"
namespace e2q {

/**
 * OrderMatch(symbol) -> Market->Order({ask,bid})
 */
typedef std::map<std::string, Market*> MarketType;
/*
 * ================================
 *        Class:  OrderMatcher
 *  Description:
 * ================================
 */
class OrderMatcher {
public:
    /* =============  LIFECYCLE     =================== */
    OrderMatcher() { _markets = MALLOC(MarketType); }; /* constructor */

    ~OrderMatcher()
    {
        // log::echo("release order matcher ...");
        for (auto it = _markets->begin(); it != _markets->end(); ++it) {
            RELEASE(it->second);
        }

        RELEASE(_markets);
    }
    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */
    void mprice(e2::Int_e);
    bool insert(OrderItem*);
    void erase(OrderItem*);
    OrderItem* find(std::string symbol, std::size_t ticket);
    bool match(std::string symbol, std::queue<OrderLots>& lots,
               e2::Int_e market_price, e2::Int_e, std::size_t);
    bool match(std::queue<OrderLots>& orders, e2::Int_e, std::size_t);

    std::size_t OrderSize(std::string);
    std::size_t OrderSizes();

    int AddBotTicket(SeqType ticket, e2::OrdType ordType, e2::Side side,
                     double bot_qty, e2::Int_e symbol);

    /**
     * ord type
     * stop or stop_limit
     *
     * 2. pending loop 的时候才使用
     * 以后再优化
     */
    bool OrdTypePending();
    void TopLevelPrice(const std::string& symbol, SeqType);

    void display(std::string symbol) const;
    void display() const;
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    MarketType* _markets = nullptr;

    /**
     * 撮合的价格
     */
    e2::Int_e _market_price = 0;

    using Mute = BasicLock::mutex_type;
    mutable Mute _OMMutex;

}; /* -----  end of class OrderMatcher  ----- */

}  // namespace e2q
#endif /* ----- #ifndef ORDERMATCHER_INC  ----- */
