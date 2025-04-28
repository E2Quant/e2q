/*
 * =====================================================================================
 *
 *       Filename:  UtilTime.hpp
 *
 *    Description:  UtilTime
 *
 *        Version:  1.0
 *        Created:  2022年08月17日 17时10分20秒
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

#ifndef UTILTIME_INC
#define UTILTIME_INC
#include <sys/time.h>
#include <time.h>

#include <cstddef>
#include <string>

#include "Norm.hpp"
namespace e2q {
#define DEF_MIN_SECOND 3745571651
/*
 * =====================================================================================
 *        Class:  UtilTime
 *  Description:
 * =====================================================================================
 */
class UtilTime {
public:
    /* ====================  LIFECYCLE =======================================
     */
    UtilTime(); /* constructor */

    /* ====================  ACCESSORS =======================================
     */

    /* ====================  MUTATORS =======================================
     */

    /* ====================  OPERATORS =======================================
     */
    _millisecond strtostamp(std::string timestr, const char *fmt);
    std::string stamptostr(size_t now, const char *fmt);
    std::string toDate(size_t now);
    std::string millitostr(_millisecond now, const char *fmt, bool ext);
    std::string millitostr(_millisecond now, const char *fmt);

    _millisecond milliseconds();
    size_t time();

    void usec();

    long offset_gmt();
    int time_offset();
    int deltam();

    /**
     * day = 2024-04-20
     * fmt = %Y-%m-%d
     */
    time_t daytm(std::string &day, std::string &fmt);

    time_t first_of_the_week(time_t, bool gmt = true);

    time_t first_of_the_month(time_t, bool gmt = true);

    /**
     *  一年中第几周
     */
    size_t yearweeks();

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

}; /* -----  end of class UtilTime  ----- */

}  // namespace e2q
#endif /* ----- #ifndef UTILTIME_INC  ----- */
