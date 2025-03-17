/*
 * =====================================================================================
 *
 *       Filename:  DataTable.cpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2022年09月20日 17时54分36秒
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

#include "VirtualPack/DataTable.hpp"

#include <iostream>

namespace e2q {
/* Right-aligns string within a field of width w. Pads with blank spaces
   to enforce alignment. */
std::string DataTable::right(const std::string s, const int w)
{
    std::stringstream ss, spaces;
    int padding = w - s.size();  // count excess room to pad
    for (int i = 0; i < padding; ++i) spaces << " ";
    ss << spaces.str() << s;  // format with padding
    return ss.str();
}

/*! Left-aligns string within a field of width w. Pads with blank spaces
    to enforce alignment. */
std::string left(const std::string s, const int w)
{
    std::stringstream ss, spaces;
    int padding = w - s.size();  // count excess room to pad
    for (int i = 0; i < padding; ++i) spaces << " ";
    ss << s << spaces.str();  // format with padding
    return ss.str();
}
std::string DataTable::center(const std::string s, const int w)
{
    std::stringstream ss, spaces;
    int padding = w - s.size();  // count excess room to pad
    for (int i = 0; i < padding / 2; ++i) spaces << " ";
    ss << spaces.str() << s << spaces.str();  // format with padding
    if (padding > 0 && padding % 2 != 0)      // if odd #, add 1 space
        ss << " ";
    return ss.str();
}
std::string DataTable::prd(const double x, const int decDigits, const int width)
{
    std::stringstream ss;
    //    ss << fixed << right;
    //    ss << right;

    ss.fill(' ');             // fill space around displayed #
    ss.width(width);          // set  width around displayed #
    ss.precision(decDigits);  // set # places after decimal
    ss << x;
    return ss.str();
}
std::string DataTable::prd(const double x, const int decDigits)
{
    std::stringstream ss;
    //    ss << fixed;
    // ss << fixed;

    ss.precision(decDigits);  // set # places after decimal
    ss << x;
    return ss.str();
}
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  DataTable::print
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void DataTable::print()
{
    std::cout << center("x", 10) << " | " << center("x^2", 10) << " | "
              << center("(x^2)/8", 10) << "\n";

    std::cout << std::string(10 * 3 + 2 * 3, '-') << "\n";

    for (double x = 1.5; x < 200; x += x * 2) {
        std::cout << prd(x, 1, 10) << " | " << prd(x * x, 2, 10) << " | "
                  << prd(x * x / 8.0, 4, 10) << "\n";
    }
} /* -----  end of function DataTable::print  ----- */
}  // namespace e2q
