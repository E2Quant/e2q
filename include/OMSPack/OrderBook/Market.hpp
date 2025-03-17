/*
 * =====================================================================================
 *
 *       Filename:  Market.hpp
 *
 *    Description:  Market
 *
 *        Version:  1.0
 *        Created:  2024年01月10日 14时19分48秒
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

#ifndef Market_INC
#define Market_INC
#include <cstddef>
#include <functional>
#include <map>
#include <queue>
#include <string>

#include "E2L/E2LType.hpp"
#include "MessagePack/Engine.hpp"
#include "OMSPack/OrderBook/Order.hpp"
#include "assembler/BaseType.hpp"
namespace e2q {
using BtreeLarge = Engine<OrderPending>;
using BtreeSmall = Engine<OrderPending, std::less<OrderPending>>;
/**
 *  pending orders
 *  FIX::OrdStatus_NEW
 *  long and short option
 */
typedef BtreeLarge BidOrders;
typedef BtreeSmall AskOrders;

/**
 *  market orders
 *  close order
 *  FIX::OrdStatus_REJECTED
 *  FIX::OrdStatus_FILLED
 *  FIX::OrdStatus_PARTIALLY_FILLED
 *  FIX::OrdStatus_CANCELED
 */

/*
 * ================================
 *        Class:  Market
 *  Description:
 * ================================
 */
class Market {
public:
    /* =============  LIFECYCLE     =================== */
    Market() {}; /* constructor */
    ~Market()
    {
        // log::echo("release Market!");
        for (auto it = MapOrder.begin(); it != MapOrder.end(); ++it) {
            RELEASE(it->second);
        }
    }
    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */
    bool insert(OrderItem* order);
    void erase(OrderItem* order);
    OrderItem* find(std::size_t ticket);
    bool match(std::queue<OrderLots>&, e2::Int_e, e2::Int_e);
    std::size_t length();
    void display();
    std::pair<e2::Int_e, e2::Int_e> top_bid_ask_price();
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    void MDebug(std::string& desz, const OrderPending*);
    OrderLots make(std::size_t);
    /* =============  DATA MEMBERS  =================== */

    BidOrders _bidOrders;
    AskOrders _askOrders;

    std::string _symbol = "";

    std::map<std::size_t, OrderItem*> MapOrder;
}; /* -----  end of class Market  ----- */

}  // namespace e2q
#endif /* ----- #ifndef Market_INC  ----- */
