/*
 * =====================================================================================
 *
 *       Filename:  BrokerBase.hpp
 *
 *    Description:  BrokerBase
 *
 *        Version:  1.0
 *        Created:  2022年08月02日 17时30分47秒
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

#ifndef BROKERBASE_INC
#define BROKERBASE_INC
#include <cstddef>
#include <map>

#include "OrderBook/Order.hpp"
#include "Toolkit/Norm.hpp"

#if __cplusplus >= 201703L

/* Now remove the trow */
#define throw(...)

#include <quickfix/FixValues.h>
#include <quickfix/SessionID.h>
#undef throw /* reset */
#endif
#include "OMSPack/foreign.hpp"
namespace e2q {

/*
 * =====================================================================================
 *        Class:  BrokerBase
 *  Description:
 * =====================================================================================
 */
class BrokerBase {
public:
    /* ====================  LIFECYCLE =======================================
     */
    BrokerBase() {}; /* constructor */

    /* ====================  ACCESSORS =======================================
     */
    void Debug();
    void setCommission(float cs);

    double Equity(const FIX::SessionID &, std::size_t, const char status);
    void SettlInst(OrderLots &);

    void freeMargin(const FIX::SessionID &, std::size_t, double);
    bool Margin(const FIX::SessionID &, std::size_t, double, long);
    double CheckMargin(const FIX::SessionID &, double, long);
    void traders(const FIX::SessionID &, double);

    void AddExdrCash(SeqType ticket, double cash, std::size_t);
    void AddExdrQty(SeqType, SeqType ticket, double qty, std::size_t);

    void trade_report(OrderLots &);
    /* ====================  MUTATORS =======================================
     */

    /* ====================  OPERATORS =======================================
     */

protected:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */
    std::map<FIX::SessionID, TraderInfo> _traders;
}; /* -----  end of class BrokerBase  ----- */

}  // namespace e2q
#endif /* ----- #ifndef BROKERBASE_INC  ----- */
