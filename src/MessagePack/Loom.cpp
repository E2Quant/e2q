/*
 * =====================================================================================
 *
 *       Filename:  Loom.cpp
 *
 *    Description:  Loom
 *
 *        Version:  1.0
 *        Created:  2023年08月22日 09时59分59秒
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

#include "MessagePack/Loom.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <list>
#include <string>
#include <utility>
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Loom::names
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Loom::names(const std::list<std::string>& names)
{
    _columns_name = std::move(names);
} /* -----  end of function Loom::names  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Loom::names
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
const std::list<std::string>& Loom::names()
{
    return _columns_name;
} /* -----  end of function Loom::names  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Loom::size
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
size_t Loom::size() const
{
    return _mulberry.row * _mulberry.column;
} /* -----  end of function Loom::size  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Loom::shape
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
const std::pair<size_t, size_t> Loom::shape() const
{
    return std::make_pair(_mulberry.row, _mulberry.column);
} /* -----  end of function Loom::shape  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Loom::empty
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool Loom::empty()
{
    return hash_row() == 0;
} /* -----  end of function Loom::empty  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Loom::operator=
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
/* Loom& Loom::operator=(const Loom& _other) */
/* { */
/*     if (this != &_other) { */
/*         _columns_name = std::move(_other._columns_name); */
/*         SilkPermit<NumberType>::operator=(_other); */
/*     } */

/*     return *this; */
/* } /1* -----  end of f:qunction Loom::operator=  ----- *1/ */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Loom::operator+
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
NumberTypes Loom::operator+(const Loom& other) const
{
    size_t size_c = 0;

    NumberTypes lt;
    if (_scope.size_c == other._scope.size_c) {
        size_c = _scope.size_c;
    }
    else {
        size_c = (_scope.size_c < other._scope.size_c ? _scope.size_c
                                                      : other._scope.size_c);
    }
    size_t me = _scope.column;
    size_t ot = other._scope.column;

    for (size_t n = 0; n < size_c; n++) {
        lt.push_back(at(0, n + me) + other.at(0, n + ot));
    }

    return lt;

} /* -----  end of function Loom::operator+  ----- */
}  // namespace e2q
