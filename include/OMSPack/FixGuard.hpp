/*
 * =====================================================================================
 *
 *       Filename:  FixGuard.hpp
 *
 *    Description:  FixGuard
 *
 *        Version:  1.0
 *        Created:  2024年05月29日 14时06分25秒
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

#ifndef FIXGUARD_INC
#define FIXGUARD_INC
#include <cstddef>
#include <string>

#include "E2L/E2LType.hpp"
#include "OMSPack/IDGenerator.hpp"
#include "OMSPack/OrderBook/Order.hpp"
#include "OMSPack/foreign.hpp"
#include "Toolkit/Norm.hpp"

#if __cplusplus >= 201703L

/* Now remove the trow */
#define throw(...)
#include <quickfix/Application.h>
#include <quickfix/FixFields.h>
#include <quickfix/FixValues.h>
#include <quickfix/Group.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/SessionID.h>
#include <quickfix/SocketAcceptor.h>
#include <quickfix/fix44/BidResponse.h>
#include <quickfix/fix44/ExecutionReport.h>
#include <quickfix/fix44/MarketDataRequest.h>
#include <quickfix/fix44/MarketDataSnapshotFullRefresh.h>
#include <quickfix/fix44/MassQuote.h>
#include <quickfix/fix44/MessageCracker.h>
#undef throw /* reset */
#endif

namespace e2q {
/*
 * ================================
 *        Class:  FixGuard
 *  Description:
 * ================================
 */
class FixGuard {
public:
    /* =============  LIFECYCLE     =================== */
    FixGuard() {

    }; /* constructor */

    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */

    void MarketMessage(const FIX::SessionID &,
                       std::array<SeqType, trading_protocols> &);

    void CustemRequest(const FIX::SessionID &, std::string &, int);
    void updateOrder(const OrderLots &, char status, double);
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    // std::string genOrderID() { return std::to_string(sequence()); };
    std::string genExecID() { return UUidGen(); };

    e2::Side convert(const FIX::Side &);
    FIX::Side convert(e2::Side);

    e2::OrdType convert(const FIX::OrdType &);
    FIX::OrdType convert(e2::OrdType);

    e2::OrdStatus convert(const FIX::OrdStatus &);
    FIX::OrdStatus convert(e2::OrdStatus);

    e2::TimeInForce convert(const FIX::TimeInForce &);
    FIX::TimeInForce convert(e2::TimeInForce);
    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

}; /* -----  end of class FixGuard  ----- */
}  // namespace e2q
#endif /* ----- #ifndef FIXGUARD_INC  ----- */
