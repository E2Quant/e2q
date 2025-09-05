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

#include <cstddef>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "E2LScript/e2lLead.hpp"
#include "E2LScript/foreign.hpp"
#include "Toolkit/Norm.hpp"
#include "assembler/BaseType.hpp"
#include "utility/Log.hpp"

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
    std::thread::id _id = std::this_thread::get_id();
    std::size_t number = e2q::e2l_thread_map.number(_id);

    e2::Int_e ret = e2q::FixPtr->_cash.TotalCash(number);

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
    std::thread::id _id = std::this_thread::get_id();
    std::size_t number = e2q::e2l_thread_map.number(_id);
    for (auto it : e2q::FixPtr->_cash.order_cash) {
        ret += it.second.margin;
        if (it.second.thread_number != number) {
            continue;
        }
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
    std::thread::id _id = std::this_thread::get_id();
    std::size_t number = e2q::e2l_thread_map.number(_id);

    e2::Int_e ret = e2q::FixPtr->_cash.TotalCash(number) -
                    e2q::FixPtr->_cash.FreezeCash(number);

    if (ret < 0) {
        ret = 0;
    }
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

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  AccountNumber
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e AccountNumber()
{
    e2::Int_e ret = 0;
    return ret;
} /* -----  end of function AccountNumber  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ThreadPosition
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  不同的线程仓位管理
 *  其实只需要从不同的 quant id 来区分就可以了
 *  输入的时候，是线程的 index
 *  是总的仓位分配，单笔交易，是当前策略的分配仓位
 * ============================================
 */
void ThreadPosition(e2::Int_e tid, e2::Int_e position)
{
    e2q::Postion thread_post;

    std::size_t num = NUMBERVAL(tid);
    thread_post._postion = (float)NUMBERVAL(position) / 100;
    if (e2q::FixPtr->_cash.all_postion < thread_post._postion) {
        thread_post._postion = e2q::FixPtr->_cash.all_postion;
    }
    double free_postion = e2q::FixPtr->_cash.all_postion - thread_post._postion;
    if (free_postion < 0) {
        std::string cond = log::format(
            "postion: %.03f all_postion:%.03f free:%.08f\n",
            thread_post._postion, e2q::FixPtr->_cash.all_postion, free_postion);
        log::bug(cond);
    }
    // log::info("number:", num, " post:", thread_post._postion);
    e2q::FixPtr->_cash.all_postion -= thread_post._postion;
    e2q::FixPtr->_cash._thread_pos.insert({num, thread_post});
    e2q::FixPtr->_cash._tsize++;
} /* -----  end of function ThreadPosition  ----- */

}  // namespace e2l

