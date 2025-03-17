/*
 * =====================================================================================
 *
 *       Filename:  account.cpp
 *
 *    Description:  account
 *
 *        Version:  1.0
 *        Created:  2024年09月04日 17时52分41秒
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

#include <string>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "E2LScript/e2lLead.hpp"
#include "E2LScript/foreign.hpp"
#include "assembler/BaseType.hpp"

namespace e2l {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  AccountBalance
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e AccountBalance()
{
    e2::Int_e ret = e2q::FixPtr->_cash.total_cash;

    return VALNUMBER(ret);
} /* -----  end of function AccountBalance  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  AccountMargin
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e AccountMargin()
{
    e2::Int_e ret = 0;
    for (auto it : e2q::FixPtr->_cash.order_cash) {
        ret += it.second.margin;
    }
    return VALNUMBER(ret);
} /* -----  end of function AccountMargin  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  AccountEquity
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e AccountEquity()
{
    e2::Int_e ret = e2q::FixPtr->_freeze_cash;

    return VALNUMBER(ret);
} /* -----  end of function AccountEquity  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  AccountProfit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e AccountProfit()
{
    e2::Int_e ret = 0;
    return ret;
} /* -----  end of function AccountProfit  ----- */
}  // namespace e2l

