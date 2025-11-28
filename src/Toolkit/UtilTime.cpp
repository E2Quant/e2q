/*
 * =====================================================================================
 *
 *       Filename:  UtilTime.cpp
 *
 *    Description:  UtilTime
 *
 *        Version:  1.0
 *        Created:  2022年08月17日 17时13分29秒
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
#include "Toolkit/UtilTime.hpp"

#include <chrono>
#include <cstddef>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

namespace e2q {

using namespace std::chrono;
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::UtilTime
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
UtilTime::UtilTime() {} /* -----  end of function UtilTime::UtilTime  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::strtostamp
 *  ->  void *
 *  Parameters:
 *  - string  timestr : 2021-07-12 15:14:23
 *  - const char* fmt : "%Y-%m-%d %H:%M:%S"
 *  Description:
 *
 * ============================================
 */
_millisecond UtilTime::strtostamp(std::string timestr, const char* fmt)
{
    std::tm tm = {};
    const char* snext = strptime(timestr.c_str(), fmt, &tm);
    if (snext == NULL) {
        //       elog::echo("snext is null");
        return 0;
    }

    auto time_point = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    _millisecond time_stamp =
        time_point.time_since_epoch() / std::chrono::milliseconds(1) +
        std::atof(snext) * 1000.0f;

    return time_stamp;

} /* -----  end of function UtilTime::strtostamp  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::stamptostr
 *  ->  void *
 *  Parameters:
 *  - size_t now : unix timestamp
 *  - const char *fmt : "%Y-%m-%d %H:%M:%S"
 *  Description:
 *
 * ============================================
 */
std::string UtilTime::stamptostr(size_t now, const char* fmt)
{
    if (now == 0) {
        return "";
    }
    if (now > DEF_MIN_SECOND) {
        // std::cerr << "now is  millisecond:" << now << std::endl;
        now /= 1000;
    }
    std::time_t tmp = now;
    std::tm* t = std::localtime(&tmp);

    std::stringstream day;
    day << std::put_time(t, fmt);
    return day.str();

} /* -----  end of function UtilTime::stamptostr  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::toDate
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::string UtilTime::toDate(size_t now)
{
    const char fmt[] = "%Y-%m-%d %H:%M:%S";
    return stamptostr(now, fmt);
} /* -----  end of function UtilTime::toDate  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Utiltime::millitostr
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::string UtilTime::millitostr(_millisecond now, const char* fmt)
{
    return millitostr(now, fmt, false);
} /* -----  end of function Utiltime::millitostr  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::millitostr
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   millisecond to string
 * ============================================
 */
std::string UtilTime::millitostr(_millisecond now, const char* fmt, bool ext)
{
    if (now == 0) {
        return "";
    }
    if (now < DEF_MIN_SECOND) {
        std::cerr << "now is not millisecond:" << now << std::endl;
    }
    std::time_t tmp = now / 1000;
    int mill = now % 1000;

    std::tm* t = std::localtime(&tmp);

    std::stringstream day;
    if (ext) {
        day << std::put_time(t, fmt) << "." << mill;
    }
    else {
        day << std::put_time(t, fmt);
    }

    return day.str();
} /* -----  end of function UtilTime::millitostr  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::miscoseconds
 *  ->  _millisecond
 *  Parameters:
 *  Description:
 *
 * ============================================
 */
_millisecond UtilTime::milliseconds()
{
    _millisecond millisecond_since_epoch =
        std::chrono::milliseconds(
            std::chrono::system_clock::now().time_since_epoch().count())
            .count() /
        UNIT;

    return millisecond_since_epoch / UNIT;

} /* -----  end of function UtilTime::miscoseconds  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::time
 *  ->  size_t
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
size_t UtilTime::time()
{
    std::chrono::duration<size_t> unix_timestamp =
        std::chrono::seconds(std::time(NULL));
    size_t timestamp = unix_timestamp.count();

    return timestamp;

} /* -----  end of function UtilTime::time  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::usec
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void UtilTime::usec()
{
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);
    printf("tv_sec: %ld\n", tv.tv_sec);
    printf("tv_usec: %ld\n", tv.tv_usec);

    //    std::chrono::time_zone *current = std::chrono::currnet_zone();
} /* -----  end of function UtilTime::usec  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::offset_gmt
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
long UtilTime::offset_gmt()
{
    time_t t = std::time(NULL);  // get the system time
    struct tm lt = {0};

    localtime_r(&t, &lt);

    /* printf("Offset to GMT is %lds.\n", lt.tm_gmtoff); */
    /* printf("The time zone is '%s'.\n", lt.tm_zone); */
    return lt.tm_gmtoff;
} /* -----  end of function UtilTime::offset_gmt  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::time_offset
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int UtilTime::time_offset()
{
    time_t gmt, rawtime = std::time(NULL);
    struct tm* ptm;

#if !defined(WIN32)
    struct tm gbuf;
    ptm = gmtime_r(&rawtime, &gbuf);
#else
    ptm = gmtime(&rawtime);
#endif
    // Request that mktime() looksup dst in timezone database
    ptm->tm_isdst = -1;
    gmt = mktime(ptm);

    return (int)difftime(rawtime, gmt);

} /* -----  end of function UtilTime::time_offset  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::deltam
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int UtilTime::deltam()
{
    time_t t = std::time(NULL);
    struct tm* loc = std::localtime(&t);
    /* save values because they could be erased by the call to gmtime */
    int loc_min = loc->tm_min;
    int loc_hour = loc->tm_hour;
    int loc_day = loc->tm_mday;
    struct tm* utc = gmtime(&t);
    int delta = loc_min - utc->tm_min;
    int deltaj = loc_day - utc->tm_mday;
    delta += (loc_hour - utc->tm_hour) * 60;
    /* hack for the day because the difference actually is only 0, 1 or -1 */
    if ((deltaj == 1) || (deltaj < -1)) {
        delta += 1440;
    }
    else if ((deltaj == -1) || (deltaj > 1)) {
        delta -= 1440;
    }
    return delta;
} /* -----  end of function UtilTime::deltam  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::daytm
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
time_t UtilTime::daytm(std::string& day, std::string& fmt)
{
    std::tm then{};

    std::istringstream in(day);

    if (!(in >> std::get_time(&then, fmt.c_str())))
        std::cerr << "Conversion failed\n";

    time_t t = mktime(&then);

    /*  %W for 0-based, %V for 1-based (ISO) week number: */
    /* std::cout << std::put_time(&then, "%V\n"); */

    return t;
} /* -----  end of function UtilTime::daytm  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::first_of_the_week
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   计算 time_t 的当前周的第一天
 * ============================================
 */
time_t UtilTime::first_of_the_week(time_t t, bool gmt)
{
    tm* timeinfo;

    if (gmt) {
        timeinfo = gmtime(&t);
    }
    else {
        timeinfo = std::localtime(&t);
    }
    int wday = timeinfo->tm_wday;
    int hour = timeinfo->tm_hour;
    int min = timeinfo->tm_min;
    int sec = timeinfo->tm_sec;

    time_t SECONDS_IN_DAY = 60 * 60 * 24;

    // monday should be 0 and sunday 6
    wday--;
    if (wday == -1) {
        wday = 6;
    }

    time_t ret = t - wday * SECONDS_IN_DAY;
    ret -= (sec + 60 * min + 60 * 60 * hour);
    return ret;
} /* -----  end of function UtilTime::first_of_the_week  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UtilTime::first_of_the_month
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
time_t UtilTime::first_of_the_month(time_t t, bool gmt)
{
    struct tm tmt;

    if (gmt) {
        if (!gmtime_r(&t, &tmt)) return -1;
    }
    else {
        localtime_r(&t, &tmt);
    }

    tmt.tm_sec = 0;
    tmt.tm_min = 0;
    tmt.tm_hour = 0;
    tmt.tm_mday = 1;
    tmt.tm_isdst = -1;
    if (gmt) {
        return timegm(&tmt);
    }
    else {
        time_t ret = mktime(&tmt);
        return ret;
    }

} /* -----  end of function UtilTime::first_of_the_month  ----- */
}  // namespace e2q
