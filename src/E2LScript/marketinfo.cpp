/*
 * =====================================================================================
 *
 *       Filename:  marketinfo.cpp
 *
 *    Description:  marketinfo
 *
 *        Version:  1.0
 *        Created:  2024年01月31日 14时50分15秒
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
#include <algorithm>
#include <cstddef>
#include <thread>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "E2LScript/e2lLead.hpp"
#include "E2LScript/foreign.hpp"
#include "assembler/BaseType.hpp"
namespace e2l {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Settlement
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  t=0 T+0  t=1 T+1 ..
 * ============================================
 */
void Settlement(e2::Int_e t)
{
    if (t >= 0) {
        std::size_t _t = NUMBERVAL(t);

        e2q::FinFabr->_settlement = _t;
    }
} /* -----  end of function Settlement  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  SymbolsTotal
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e SymbolsTotal()
{
    e2::Int_e num = e2q::FixPtr->_fix_symbols.size();

    return VALNUMBER(num);
} /* -----  end of function SymbolsTotal  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  SymbolName
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void SymbolName(e2::Int_e cfi) {} /* -----  end of function SymbolName  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  SymbolCFICode
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  从 e2l cfi code to data stock code
 * ============================================
 */
void SymbolCFICode(e2::Int_e cfi)
{
    int _id = (int)cfi;
    if (e2q::FixPtr->_fix_symbols.count(_id) == 1) {
        log::bug(e2q::FixPtr->_fix_symbols.at(_id));
    }
    else {
        log::info("error cfi id:", _id);
    }

} /* -----  end of function SymbolCFICode  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  SymbolSelect
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  保存好，等 fix 连接上去之后，就可以初始化了
 *  不需要在这儿分析是不是在 fix 中
 * ===========================================
 */
void SymbolSelect(e2::Int_e id)
{
    std::size_t _id = NUMBERVAL(id);
    e2q::FixPtr->_symbols.push_back(_id);

} /* -----  end of function SymbolSelect  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  SymbolCurrent
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e SymbolCurrent()
{
    std::size_t m = 0;
    int cficode = 0;
    for (auto it : e2q::FixPtr->_fix_symbols) {
        if (m == 1) {
            cficode = it.first;
        }
        m++;
    }
    return cficode;
} /* -----  end of function SymbolCurrent  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BarOnOpen
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  如果源数据是 on close 就得开启，否则30 分钟之类的周期会出错
 * ============================================
 */
void BarOnOpen()
{
    e2q::FixPtr->_onOpen = true;

} /* -----  end of function BarOnOpen  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BarVolumeAppend
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BarVolumeAppend()
{
    e2q::FixPtr->_volume_append = true;
} /* -----  end of function BarVolumeAppend  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BarSize
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e BarSize(e2::Int_e id, e2::Int_e timeframe)
{
    size_t _id = NUMBERVAL(id);
    timeframe = NUMBERVAL(timeframe);
    e2::Int_e ret = 0;
    if (e2q::e2l_cnt != nullptr) {
        ret = e2q::e2l_cnt->data_ptr->writed(_id, timeframe);
    }

    return VALNUMBER(ret);
} /* -----  end of function BarSize  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Bar
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Bool Bar(e2::Int_e id, e2::TimeFrames timeframe, e2::Int_e shift)
{
    size_t stock = NUMBERVAL(id);
    shift = NUMBERVAL(shift);
    timeframe = (e2::TimeFrames)NUMBERVAL(timeframe);
    e2::Int_e ret = 0;

    e2q::BasicLock lock(e2q::e2Mutex);

    std::thread::id pid;
    E2LBAR(pid);

    std::fill(e2q::e2l_bar_ohlc.at(pid).second.begin(),
              e2q::e2l_bar_ohlc.at(pid).second.end(), 0);

    e2::Bool _bool = e2::Bool::B_FALSE;
    std::array<e2q::SeqType, ohlc_column> bar;
    std::size_t idx = 0;

    if (e2q::e2l_cnt != nullptr) {
        idx = e2q::e2l_cnt->data_ptr->idx(stock, timeframe);
        ret = e2q::e2l_cnt->data_ptr->read(bar, stock, timeframe, shift);

        if (ret != -1) {
            e2q::e2l_bar_ohlc.at(pid).second = bar;
            e2q::e2l_bar_ohlc.at(pid).first = idx;
            _bool = e2::Bool::B_TRUE;
        }
    }
    return _bool;
} /* -----  end of function Bar  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BarSeries
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e BarSeries(e2::BarType bt)
{
    e2::Int_e ret = 0;
    if ((int)bt > 10) {
        bt = (e2::BarType)NUMBERVAL(bt);
    }
    std::thread::id pid;
    E2LBAR(pid);
    std::size_t len = e2q::e2l_bar_ohlc.at(pid).second.size();

    if (bt >= len) {
        log::bug("error len:", len, " bt:", bt);
        return 0;
    }
    ret = e2q::e2l_bar_ohlc.at(pid).second[bt];

    return ret;
} /* -----  end of function BarSeries  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  iOpen
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e iOpen(e2::Int_e id, e2::TimeFrames timeframe, e2::Int_e shift)
{
    e2::Int_e ret = 0;

    e2::Bool b = Bar(id, timeframe, shift);
    if (b == e2::Bool::B_FALSE) {
        return ret;
    }

    ret = BarSeries(e2::BarType::MODE_OPEN);

    return ret;
} /* -----  end of function iOpen  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  iHigh
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e iHigh(e2::Int_e id, e2::TimeFrames timeframe, e2::Int_e shift)
{
    e2::Int_e ret = 0;
    e2::Bool b = Bar(id, timeframe, shift);
    if (b == e2::Bool::B_FALSE) {
        return ret;
    }

    ret = BarSeries(e2::BarType::MODE_HIGH);
    return ret;
} /* -----  end of function iHigh  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  iHighest
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e iHighest(e2::Int_e id, e2::TimeFrames timeframe, e2::BarType bt,
                   e2::Int_e count, e2::Int_e start)
{
    id = NUMBERVAL(id);
    timeframe = (e2::TimeFrames)NUMBERVAL(timeframe);
    bt = (e2::BarType)NUMBERVAL(bt);

    count = NUMBERVAL(count);
    start = NUMBERVAL(start);

    int isread = 0;
    e2::Int_e ret = 0;
    std::array<e2q::SeqType, ohlc_column> ohlc;
    if (e2q::e2l_cnt != nullptr) {
        e2::Int_e next = count + start;
        for (e2::Int_e shift = start; shift < next; shift++) {
            isread = e2q::e2l_cnt->data_ptr->read(ohlc, id, timeframe, shift);

            if (isread != -1) {
                e2::Int_e high = ret > ohlc[bt] ? ret : ohlc[bt];
                ret = high;
            }
        }
    }

    return ret;
} /* -----  end of function iHighest  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  iLow
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e iLow(e2::Int_e id, e2::TimeFrames timeframe, e2::Int_e shift)
{
    e2::Int_e ret = 0;
    e2::Bool b = Bar(id, timeframe, shift);
    if (b == e2::Bool::B_FALSE) {
        return ret;
    }

    ret = BarSeries(e2::BarType::MODE_LOW);
    return ret;
} /* -----  end of function iLow  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  iLowest
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e iLowest(e2::Int_e id, e2::TimeFrames timeframe, e2::BarType bt,
                  e2::Int_e count, e2::Int_e start)
{
    e2::Int_e ret = 0;
    return ret;
} /* -----  end of function iLowest  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  iClose
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e iClose(e2::Int_e id, e2::TimeFrames timeframe, e2::Int_e shift)
{
    e2::Int_e ret = 0;

    e2::Bool b = Bar(id, timeframe, shift);
    if (b == e2::Bool::B_FALSE) {
        return ret;
    }

    ret = BarSeries(e2::BarType::MODE_CLOSE);

    return ret;
} /* -----  end of function iClose  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  iAdjClose
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e iAdjClose(e2::Int_e id, e2::TimeFrames timeframe, e2::Int_e shift)
{
    e2::Int_e ret = 0;

    e2::Bool b = Bar(id, timeframe, shift);
    if (b == e2::Bool::B_FALSE) {
        return ret;
    }

    ret = BarSeries(e2::BarType::MODE_ADJCLOSE);

    return ret;

} /* -----  end of function iAdjClose  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  iVolume
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e iVolume(e2::Int_e id, e2::TimeFrames timeframe, e2::Int_e shift)
{
    e2::Int_e ret = 0;

    e2::Bool b = Bar(id, timeframe, shift);
    if (b == e2::Bool::B_FALSE) {
        return ret;
    }

    ret = BarSeries(e2::BarType::MODE_VOL);
    return ret;
} /* -----  end of function iVolume  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  iTime
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e iTime(e2::Int_e id, e2::TimeFrames timeframe, e2::Int_e shift)
{
    e2::Int_e ret = 0;

    e2::Bool b = Bar(id, timeframe, shift);
    if (b == e2::Bool::B_FALSE) {
        return ret;
    }

    ret = BarSeries(e2::BarType::MODE_TIME);

    return ret;
} /* -----  end of function iTime  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  isExdiDate
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Bool isExdiDate(e2::Int_e id)
{
    e2::Bool b = e2q::ExdiSymList.date(id);

    return b;
} /* -----  end of function isExdiDate  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ExdiLoop
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Bool ExdiLoop(e2::Int_e id)
{
    e2::Bool b = e2::Bool::B_FALSE;
    return b;
} /* -----  end of function ExdiLoop  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ExdiCash
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ExdiCash(e2 ::Int_e id)
{
    e2::Int_e ret = e2q::ExdiSymList.cash(id);
    return ret;
} /* -----  end of function ExdiCash  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ExdiShare
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ExdiShare(e2::Int_e id)
{
    e2::Int_e ret = e2q::ExdiSymList.share(id);
    return ret;
} /* -----  end of function ExdiShare  ----- */
}  // namespace e2l
