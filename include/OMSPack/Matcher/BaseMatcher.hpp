/*
 * =====================================================================================
 *
 *       Filename:  BaseMatcher.hpp
 *
 *    Description:  BaseMatcher
 *
 *        Version:  1.0
 *        Created:  2025/02/08 10时13分47秒
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

#ifndef BASEMATCHER_INC
#define BASEMATCHER_INC
#include <cstdint>
#include <string>
#include <vector>

#include "E2L/E2LType.hpp"
#include "OMSPack/OrderBook/Order.hpp"
#include "Toolkit/Norm.hpp"
#include "assembler/BaseType.hpp"
namespace e2q {

/*
 * ================================
 *        Class:  BaseMatcher
 *  Description:
 * ================================
 */
class BaseMatcher {
public:
    /* =============  LIFECYCLE     =================== */
    BaseMatcher() = default; /* constructor */
    virtual ~BaseMatcher() = default;

    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */
    virtual void InitSequence() = 0;
    virtual SeqType sequence(const FIX::SessionID &, e2::Side) = 0;

    // match
    virtual std::vector<OrderLots> matcher(std::string symbol, e2::Int_e now,
                                           e2::Int_e price,
                                           e2::Int_e adjprice) = 0;
    virtual bool insert(OrderItem *) = 0;
    virtual void display() = 0;
    virtual e2::Int_e CheckClose(SeqType ticket, const std::string &,
                                 e2::Int_e lots) = 0;
    virtual int AddBotTicket(SeqType ticket, e2::OrdType ordType, e2::Side side,
                             double bot_qty, e2::Int_e symbol) = 0;

    virtual bool OrdTypePending() = 0;
    virtual void TopLevelPrice(const std::string &symbol, SeqType) = 0;

    // broker
    virtual double Equity(const FIX::SessionID &, std::size_t,
                          const char status) = 0;
    // virtual void SettlInst(OrderLots &) = 0;
    virtual void freeMargin(const FIX::SessionID &, std::size_t, double) = 0;
    virtual bool Margin(const FIX::SessionID &, std::size_t, double, long) = 0;
    virtual double CheckMargin(const FIX::SessionID &, double, long) = 0;
    virtual double traders(const FIX::SessionID &, double) = 0;

    virtual void ExdrChange(SeqType, SeqType ticket, double cash, double qty,
                            std::size_t ctime) = 0;

    virtual void exist() = 0;
    void SessionLogout(const FIX::SessionID &) {};
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

}; /* -----  end of class BaseMatcher  ----- */

}  // namespace e2q
#endif /* ----- #ifndef BASEMATCHER_INC  ----- */
