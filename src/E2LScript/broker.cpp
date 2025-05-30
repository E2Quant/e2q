/*
 * =====================================================================================
 *
 *       Filename:  broker.cpp
 *
 *    Description:  broker
 *
 *        Version:  1.0
 *        Created:  2024年09月02日 14时49分55秒
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
#include <cstddef>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "E2LScript/util_inline.hpp"
#include "assembler/BaseType.hpp"

namespace e2l {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  setCash
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  最小值为 1 万
 * ============================================
 */
void setCash(e2::Int_e cash)
{
    double _cash = (double)NUMBERVAL(cash);
    if (_cash <= 0) {
        _cash = 1;
    }
    double unit = 10000;
    e2q::FixPtr->_cash.total_cash = _cash * unit;
} /* -----  end of function setCash  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  getCash
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e getCash()
{
    std::size_t cash = e2q::FixPtr->_cash.total_cash;
    return VALNUMBER(cash);
} /* -----  end of function getCash  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  SettlInst
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void SettlInst(e2::SettleInstMode sim)
{
    e2q::FinFabr->_sim = (e2::SettleInstMode)NUMBERVAL(sim);
} /* -----  end of function SettlInst  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TIForce
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void TIForce(e2::TimeInForce tif)
{
    e2q::FinFabr->_tif = (e2::TimeInForce)NUMBERVAL(tif);

} /* -----  end of function TIForce  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  MarginRate
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  百分比 0.2 == 20%
 * ============================================
 */
void MarginRate(e2::Int_e mr)
{
    e2q::FinFabr->_margin_rate = (double)NUMBERVAL(mr);

} /* -----  end of function MarginRate  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  MatchEventInit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void MatchEventInit(e2::MatchEvent me)
{
    e2q::FinFabr->_ME = (e2::MatchEvent)NUMBERVAL(me);
} /* -----  end of function MatchEventInit  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  EnableExdividend
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void EnableExdividend(e2::Bool e)
{
    //log::info("e:", e);

    e2q::FinFabr->_enable_exrd = e;
} /* -----  end of function EnableExdividend  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ExDivPrice
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ExDivPrice(e2::Int_e cfi)
{
    return e2q::ExdiSymList.getPrice(cfi);
} /* -----  end of function ExDivPrice  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ExDividendSize
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ExDividendSize(e2::Int_e cfi)
{
    e2::Int_e ret = 0;

    if (e2q::FinFabr->_exrd.count(cfi) == 0) {
        return ret;
    }

    ret = e2q::FinFabr->_exrd.at(cfi).size();
    return VALNUMBER(ret);
}; /* -----  end of function ExDividendSize  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ExDividendDate
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ExDividendDate(e2::Int_e cfi, e2::Int_e idx)
{
    e2::Int_e ret = 0;
    std::size_t _idx = NUMBERVAL(idx);
    if (_idx > e2q::FinFabr->_exrd.at(cfi).size()) {
        return ret;
    }

    ret = e2q::FinFabr->_exrd.at(cfi).at(_idx)._ymd;

    return ret;
}; /* -----  end of function ExDividendDate  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ExDividendCash
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ExDividendCash(e2::Int_e cfi, e2::Int_e idx)
{
    e2::Int_e ret = 0;
    std::size_t _idx = NUMBERVAL(idx);
    if (_idx > e2q::FinFabr->_exrd.at(cfi).size()) {
        return ret;
    }

    ret = e2q::FinFabr->_exrd.at(cfi).at(_idx)._cash;

    return ret;
}; /* -----  end of function ExDividendCash  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ExDividendShare
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ExDividendShare(e2::Int_e cfi, e2::Int_e idx)
{
    e2::Int_e ret = 0;
    std::size_t _idx = NUMBERVAL(idx);
    if (_idx > e2q::FinFabr->_exrd.at(cfi).size()) {
        return ret;
    }

    ret = e2q::FinFabr->_exrd.at(cfi).at(_idx)._share;
    return ret;
}; /* -----  end of function ExDividendShare  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BrokerBook
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BrokerBook(e2::BookType bt)
{
    e2q::FinFabr->_BookType = (e2::BookType)NUMBERVAL(bt);
    log::echo("book Type:", e2q::FinFabr->_BookType);
} /* -----  end of function BrokerBook  ----- */

}  // namespace e2l
