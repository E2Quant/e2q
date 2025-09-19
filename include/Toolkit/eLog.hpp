/*
 * =====================================================================================
 *
 *       Filename:  Log.hpp
 *
 *    Description:  Log
 *
 *        Version:  1.0
 *        Created:  2022年08月16日 14时38分20秒
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

#ifndef ELOG_INC
#define ELOG_INC
#include <sys/stat.h>
#include <unistd.h>

#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "Colors.hpp"
#include "Util.hpp"
namespace e2q {

namespace elog {

template <class T>
T base_name(T const &path, T const &delims = "/\\")
{
    return path.substr(path.find_last_of(delims) + 1);
}
template <class T>
T remove_extension(T const &filename)
{
    typename T::size_type const p(filename.find_last_of('.'));
    return p > 0 && p != T::npos ? filename.substr(0, p) : filename;
}
/* template <typename T> */
/* void logError(const T &message, const char *file, const int line); */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  logError
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */

template <typename... Args>
void log_cout(const char *file, const char *functionName, long lineNumber,
              std::string color, Args &&...args)
{
    std::time_t t = std::time(nullptr);

#ifdef DEBUG
    std::cout << KCYN
              << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S %Z")
              << ":" << getpid() << "=>[" << base_name(std::string(file))
              << "::" << functionName << " line." << lineNumber << "] " << RST
              << color;
    (std::cout << ... << args) << RST << std::endl;
#else
    //  const char *log_file = GlobalMainArguments.log_dir.c_str();
    // 到时候再优化吧
    // const char fmt[] = "%s/%s";
    // std::size_t len =
    //     snprintf(NULL, 0, fmt, GlobalMainArguments.log_dir.c_str(),
    //              std::put_time(std::localtime(&t), "%Y-%m-%d_%H_%M")) +
    //     1;

    // snprintf(_dir, len, fmt, GlobalMainArguments.log_dir.c_str(),
    //          std::put_time(std::localtime(&t), "%Y-%m-%d_%H_%M"));

    //   std::ofstream cout(log_file, std::ios::app);
    if (GlobalMainArguments.log_io.is_open()) {
        GlobalMainArguments.log_io
            << KCYN << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S %Z")
            << ":" << getpid() << "=>[" << base_name(std::string(file))
            << "::" << functionName << " line." << lineNumber << "] " << RST
            << color;
        (GlobalMainArguments.log_io << ... << args) << RST << std::endl;
    }
#endif
}

#define echo(...) log_cout(__FILE__, __FUNCTION__, __LINE__, KBLU, __VA_ARGS__);
#define bug(...) log_cout(__FILE__, __FUNCTION__, __LINE__, KRED, __VA_ARGS__);
#define info(...) log_cout(__FILE__, __FUNCTION__, __LINE__, KGRN, __VA_ARGS__);
#define debug(...) \
    log_cout(__FILE__, __PRETTY_FUNCTION__, __LINE__, KORG, __VA_ARGS__);

std::string format(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

}  // namespace elog
}  // namespace e2q
#endif /* ----- #ifndef ELOG_INC  ----- */
