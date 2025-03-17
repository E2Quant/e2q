/*
 * =====================================================================================
 *
 *       Filename:  Analyzers.cpp
 *
 *    Description:  Analyzers
 *
 *        Version:  1.0
 *        Created:  2024年05月29日 17时11分03秒
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
#include "PlunkPack/Analyzers.hpp"

#include <cstdint>
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Analyzers::run
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Analyzers::run(OrderInfo info, SeqType atime)
{
    FIX_PTR_IS_NULL();

    _cumqty = info.cumqty;
    _avgpx = info.avgpx;
    _leavesqty = info.leavesqty;
    _qty = info.qty;
    _price = info.price;

    /* bprinter::TablePrinter tp(&std::cout); */
    /* tp.AddColumn("Name", 25); */
    /* tp.AddColumn("Val", 5); */

    /* tp.PrintHeader(); */
    /* tp << "Equity Final[$]" << FixPtr->_analy.EquityFinal; */
    /* tp << "Equity Peak[$]" << FixPtr->_analy.EquityPeak; */
    /* tp << "Return[%]" << FixPtr->_analy.Return; */
    /* tp << "Buy & Hold Return [%]" << FixPtr->_analy.BuyAndHold; */
    /* tp.PrintFooter(); */

    FixPtr->_analy.start = atime;

    equity();

    // pgsql.analytics(FixPtr->_analy);
} /* -----  end of function Analyzers::run  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Analyzers::equity
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Analyzers::equity()
{
    SeqType equity = Mul(_cumqty, _current_price) - Mul(_qty, _price);

    if (equity > FixPtr->_analy.EquityPeak) {
        FixPtr->_analy.EquityPeak = equity;
    }

    int64_t ee = FixPtr->_analy.EquityFinal - equity;
    FixPtr->_analy.Return = SDiv(ee, equity);
    FixPtr->_analy.EquityFinal = equity;

    if (_pre_price == 0) {
        FixPtr->_analy.BuyAndHold = 0;
    }
    else {
        FixPtr->_analy.BuyAndHold =
            SDiv((_pre_price - _current_price), _current_price);
    }

} /* -----  end of function Analyzers::equity  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ReturnAnn
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  https://www.investopedia.com/terms/a/annualized-total-return.asp
 * ============================================
 */
void ReturnAnn() {} /* -----  end of function ReturnAnn  ----- */
}  // namespace e2q
