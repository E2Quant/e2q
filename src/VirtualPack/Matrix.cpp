/*
 * =====================================================================================
 *
 *       Filename:  Matrix.cpp
 *
 *    Description:  Matrix
 *
 *        Version:  1.0
 *        Created:  2022年11月16日 16时00分54秒
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

#include "../../include/VirtualPack/Matrix.hpp"
namespace e2q {
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Matrix::Matrix
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
Matrix::Matrix() {} /* -----  end of function Matrix::Matrix  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Matrix::indexOf
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int Matrix::indexOf(const string name)
{
    int m = -1;

    auto fe = [&name, &m](string e) {
        m++;
        return name == e;
    };  // -----  end lambda  -----

    auto t = find_if(_column_names.cbegin(), _column_names.cend(), fe);
    if (t == _column_names.cend()) {
        m = -1;
    }
    return m;
} /* -----  end of function Matrix::indexOf  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Matrix::abs_n
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::size_t Matrix::abs_n(int n, size_t c)
{
    size_t asize = 0;
    size_t nsize = abs(n);
    if (nsize > c) {
        return 0;
    }
    if (n < 0) {
        asize = c + n;
    }
    else if (n >= 0) {
        asize = n;
    }
    else {
        return 0;
    }
    return asize;
} /* -----  end of function Matrix::abs_n  ----- */
}  // namespace e2q
