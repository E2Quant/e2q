/*
 * =====================================================================================
 *
 *       Filename:  TraderAlgorithms.hpp
 *
 *    Description:  TraderAlgorithms
 *
 *        Version:  1.0
 *        Created:  2022年08月02日 17时49分12秒
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

#ifndef TRADERALGORITHMS_INC
#define TRADERALGORITHMS_INC

#include <vector>

#include "OMSPack/BrokerBase.hpp"
#include "OMSPack/Matcher/BaseMatcher.hpp"
#include "OMSPack/OrderBook/OrderMatcher.hpp"
#include "OMSPack/SessionGlobal.hpp"
#include "Toolkit/Norm.hpp"
#include "assembler/BaseType.hpp"
namespace e2q {

/*
 * =====================================================================================
 *        Class:  TraderAlgorithms
 *  Description:
 * =====================================================================================
 */
class TraderAlgorithms : public BaseMatcher {
public:
    /* ====================  LIFECYCLE =======================================
     */
    TraderAlgorithms() { _orderMatcher = std::make_shared<OrderMatcher>(); };
    ~TraderAlgorithms() { RELEASE(_sequence); }
    /* constructor */

    /* ====================  ACCESSORS =======================================
     */

    /* ====================  MUTATORS =======================================
     */
    void InitSequence();
    SeqType sequence(const FIX::SessionID &, e2::Side);

    std::vector<OrderLots> matcher(std::string symbol, e2::Int_e now,
                                   e2::Int_e price, e2::Int_e adjprice);
    bool insert(OrderItem *);
    void display();
    e2::Int_e CheckClose(SeqType ticket, const std::string &, e2::Int_e lots);
    int AddBotTicket(SeqType ticket, e2::OrdType ordType, e2::Side side,
                     double bot_qty, e2::Int_e symbol);
    bool OrdTypePending();
    void TopLevelPrice(const std::string &symbol, SeqType);

    double Equity(const FIX::SessionID &, std::size_t, const char status);
    //    void SettlInst(OrderLots &);

    void freeMargin(const FIX::SessionID &, std::size_t, double);
    bool Margin(const FIX::SessionID &, std::size_t, double, long qty);
    double CheckMargin(const FIX::SessionID &, double, long);
    double traders(const FIX::SessionID &, double);

    void ExdrChange(SeqType, SeqType ticket, double cash, double qty,
                    std::size_t);
    void exist()
    {
#ifdef DEBUG
        _broker.Debug();
#endif
    };
    /* ====================  OPERATORS =======================================
     */

protected:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS ============================ */

    /* ====================  DATA MEMBER  ============================= */
    std::shared_ptr<OrderMatcher> _orderMatcher = nullptr;

    /**
     * symbol,_PreTime, _PreNow, _PrePrice,_PreAdjPrice
     */

    MarketPrice _symbol_market_price;

    BrokerBase _broker;

    std::size_t offset = 718281828;
    AutoIncrement *_sequence{nullptr};
}; /* -----  end of class TraderAlgorithms  ----- */

}  // namespace e2q

#endif /* ----- #ifndef TRADERALGORITHMS_INC  ----- */

