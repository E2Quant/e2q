/*
 * =====================================================================================
 *
 *       Filename:  FeedBalance.cpp
 *
 *    Description:  FeedBalance
 *
 *        Version:  1.0
 *        Created:  2022年11月19日 17时46分22秒
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

#include "FeedPack/FeedBalance.hpp"

#include <cstddef>
#include <memory>
#include <vector>

#include "E2L/E2LType.hpp"
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FeedBalance::CtrlPack
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FeedBalance::CtrlPack(std::shared_ptr<BeamData> data)
{
    _data_ptr = std::move(data);
    if (_data_ptr == nullptr || _shu_ptr == nullptr) {
        log::bug("data is nullptr");
        return;
    }

    obtain();

} /* -----  end of function FeedBalance::CtrlPack  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FeedBalance::obtain
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  beam connect
 * ============================================
 */
void FeedBalance::obtain()
{
    if (_resource == nullptr) {
        log::bug("_resourece is nullptr!");
        return;
    }

    std::vector<size_t> symId = FixPtr->_symbols;
    std::vector<e2::TimeFrames> tf = FixPtr->_tf;

    OHLCBeam _ohlc;
    _ohlc.shareptr(_resource);
    _ohlc.market(symId, tf, FixPtr->_offers, FixPtr->_tradetime);
    _data_ptr->assign<OHLCBeam, Func_beam>(_ohlc);

    AnalyBeam _analy;
    _analy.shareptr(_resource);
    _data_ptr->assign<AnalyBeam, Func_beam>(_analy);

} /* -----  end of function FeedBalance::obtain  ----- */

}  // namespace e2q
