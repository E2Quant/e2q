/*
 * =====================================================================================
 *
 *       Filename:  Trigger.cpp
 *
 *    Description:  Trigger
 *
 *        Version:  1.0
 *        Created:  2022年08月10日 10时08分42秒
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

#include "Coordinate/Trigger.hpp"

#include <atomic>
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Trigger::Trigger
 *  ->  void
 *  Parameters:
 *  - -
 *  Description:
 *
 * ============================================
 */
Trigger::Trigger()
{
    start();
} /* -----  end of function Trigger::Trigger  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Trigger::start
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Trigger::start()
{
    /**
     *  重新再来
     */
    _status.store(E2Q_STOP, std::memory_order_release);
} /* -----  end of function Trigger::start  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Trigger::emit
 *  ->  void
 *  Parameters:
 *  - -
 *  Description:
 *
 * ============================================
 */
void Trigger::emit()
{
    std::lock_guard<std::mutex> lk(cv_m);
    cv.notify_all();

} /* -----  end of function Trigger::emit  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Trigger::subscriber
 *  ->  void
 *  Parameters:
 *  - -
 *  Description:
 *
 * ============================================
 */
int Trigger::subscriber()
{
    SeqType st = _status.load(std::memory_order_acquire);
    if (st == E2Q_EXIST) {
        return -1;
    }
    else if (st == E2Q_RUNING) {
        /**
         * jump one emit
         * 跳过一个emit 好了
         * 不能在这儿 stop 啊，会死人的
         */
        return 0;
    }
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk, [this] {
        SeqType st = _status.load(std::memory_order_acquire);
        if (st == E2Q_EXIST || st == E2Q_RUNING) {
            return true;
        }
        else {
            return false;
        }
    });

    return 1;
} /* -----  end of function Trigger::subscriber  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Trigger::turn
 *  ->  void
 *  Parameters:
 *  - bool onoff
 *  Description:
 *
 * ============================================
 */

void Trigger::turn(SeqType t) { _status.store(t, std::memory_order_release); }
/* -----  end of function Trigger::turn  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Trigger::status
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
SeqType Trigger::status()
{
    return _status.load(std::memory_order_acquire);
} /* -----  end of function Trigger::status  ----- */
}  // namespace e2q
