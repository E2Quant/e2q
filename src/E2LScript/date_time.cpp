/*
 * =====================================================================================
 *
 *       Filename:  date_time.cpp
 *
 *    Description:  date_time
 *
 *        Version:  1.0
 *        Created:  2025/02/10 09时07分06秒
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
#include "E2L/date_time.hpp"

#include "E2LScript/ExternClazz.hpp"
#include "E2LScript/e2lLead.hpp"
#include "E2LScript/foreign.hpp"
#include "Toolkit/Norm.hpp"
#include "Toolkit/UtilTime.hpp"
#include "assembler/BaseType.hpp"
namespace e2l {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TimeCurrent
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e TimeCurrent()
{
    return e2q::ticket_now;
} /* -----  end of function TimeCurrent  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TimeLocal
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e TimeLocal()
{
    e2q::UtilTime ut;
    e2q::_millisecond now = ut.milliseconds();

    return now;
} /* -----  end of function TimeLocal  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Clock
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e Clock()
{
    e2::Int_e _time = 0;
    e2q::UtilTime ut;
    _time = ut.milliseconds();
    return _time;
} /* -----  end of function Clock  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  year
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e year(e2::Int_e now)
{
    e2::Int_e y = 0;
    const char *fmt = "%Y";
    e2q::UtilTime ut;
    std::string ys = ut.millitostr(now, fmt);
    y = atoi(ys.c_str());
    return VALNUMBER(y);
} /* -----  end of function year  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  month
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e month(e2::Int_e now)
{
    e2::Int_e v = 0;
    const char *fmt = "%m";
    e2q::UtilTime ut;
    std::string ys = ut.millitostr(now, fmt);
    v = atoi(ys.c_str());
    return VALNUMBER(v);
} /* -----  end of function month  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  day
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e day(e2::Int_e now)
{
    e2::Int_e v = 0;
    const char *fmt = "%d";
    e2q::UtilTime ut;
    std::string ys = ut.millitostr(now, fmt);
    v = atoi(ys.c_str());
    return VALNUMBER(v);
} /* -----  end of function day  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  week
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e week(e2::Int_e now)
{
    e2::Int_e v = 0;
    const char *fmt = "%w";
    e2q::UtilTime ut;
    std::string ys = ut.millitostr(now, fmt);
    v = atoi(ys.c_str());
    return VALNUMBER(v);
} /* -----  end of function week  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  year_week
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e year_week(e2::Int_e now)
{
    e2::Int_e v = 0;
    const char *fmt = "%U";
    e2q::UtilTime ut;
    std::string ys = ut.millitostr(now, fmt);
    v = atoi(ys.c_str());
    return VALNUMBER(v);
} /* -----  end of function year_week  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  hours
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e hours(e2::Int_e now)
{
    e2::Int_e v = 0;
    const char *fmt = "%H";
    e2q::UtilTime ut;
    std::string ys = ut.millitostr(now, fmt);
    v = atoi(ys.c_str());
    return VALNUMBER(v);
} /* -----  end of function hours  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  minutes
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e minutes(e2::Int_e now)
{
    e2::Int_e v = 0;
    const char *fmt = "%M";
    e2q::UtilTime ut;
    std::string ys = ut.millitostr(now, fmt);
    v = atoi(ys.c_str());
    return VALNUMBER(v);
} /* -----  end of function minutes  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  second
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e second(e2::Int_e now)
{
    e2::Int_e v = 0;
    const char *fmt = "%s";
    e2q::UtilTime ut;
    std::string ys = ut.millitostr(now, fmt);
    v = atoi(ys.c_str());
    return VALNUMBER(v);

} /* -----  end of function second  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  millisecond
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e millisecond(e2::Int_e t)
{
    return VALNUMBER((t % 1000));
} /* -----  end of function millisecond  ----- */
}  // namespace e2l
