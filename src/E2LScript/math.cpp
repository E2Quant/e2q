/*
 * =====================================================================================
 *
 *       Filename:  math.cpp
 *
 *    Description:  math
 *
 *        Version:  1.0
 *        Created:  2023年12月25日 11时34分51秒
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
#include <math.h>

#include <cmath>
#include <cstddef>
#include <numeric>
#include <thread>
#include <utility>
#include <vector>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "E2LScript/e2lLead.hpp"
#include "E2LScript/util_inline.hpp"
#include "assembler/BaseType.hpp"
namespace e2l {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Array
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Array(e2::Int_e id, e2::Int_e asize, const char *_vname, e2::Int_e loc,
           const char *_path)
{
    id = NUMBERVAL(id);

    std::size_t len = (std::size_t)NUMBERVAL(asize);

    bool ret = e2q::e2_share_array.init(id, len);
    if (ret == false) {
        log::bug("id:", std::string(_vname), " codeline:", loc,
                 " path:", std::string(_path));
    }
} /* -----  end of function Array  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ArrayFixed
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void ArrayFixed(e2::Int_e id, const char *_vname, e2::Int_e loc,
                const char *_path)
{
    id = NUMBERVAL(id);

    e2q::e2_share_array.fixed(id);

} /* -----  end of function ArrayFixed  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ArrayFill
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void ArrayFill(e2::Int_e id, e2::Int_e value, const char *_vname, e2::Int_e loc,
               const char *_path)
{
    id = NUMBERVAL(id);

    std::size_t max_size = e2q::e2_share_array.size(id);
    for (std::size_t m = 0; m < max_size; m++) {
        e2q::e2_share_array.update(id, m, value);
    }

} /* -----  end of function ArrayFill  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  AddArray
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Bool ArrayAdd(e2::Int_e id, e2::Int_e val, const char *_vname,
                  e2::Int_e loc, const char *_path)
{
    id = NUMBERVAL(id);

    e2::Bool ret = e2q::e2_share_array.add(id, val);
    if (ret == e2::Bool::B_FALSE) {
        log::bug("id:", id);
    }
    return ret;
} /* -----  end of function AddArray  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UpdateArray
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Bool ArrayUpdate(e2::Int_e id, e2::Int_e index, e2::Int_e x,
                     const char *_vname, e2::Int_e loc, const char *_path)
{
    id = NUMBERVAL(id);

    std::size_t idx = (std::size_t)NUMBERVAL(index);
    e2::Bool ret = e2::Bool::B_TRUE;

    ret = e2q::e2_share_array.update(id, idx, x);

    return ret;
} /* -----  end of function UpdateArray  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ArrayShare
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  跨进程的，只允许读，不能写
 * ============================================
 */
void ArrayShare(e2::Int_e id, const char *_vname, e2::Int_e loc,
                const char *_path)
{
    id = NUMBERVAL(id);
    e2q::e2_share_array.add_proce(id);

} /* -----  end of function ArrayShare  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ArrayLength
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ArrayLength(e2::Int_e id, const char *_vname, e2::Int_e loc,
                      const char *_path)
{
    id = NUMBERVAL(id);

    e2::Int_e len = e2q::e2_share_array.length(id);

    return VALNUMBER(len);
} /* -----  end of function ArrayLength  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ArraySize
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ArraySize(e2::Int_e id, const char *_vname, e2::Int_e loc,
                    const char *_path)
{
    id = NUMBERVAL(id);

    e2::Int_e len = e2q::e2_share_array.size(id);

    return VALNUMBER(len);
} /* -----  end of function ArraySize  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  GetArray
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ArrayGet(e2::Int_e id, e2::Int_e index, const char *_vname,
                   e2::Int_e loc, const char *_path)
{
    id = NUMBERVAL(id);

    std::size_t idx = (std::size_t)NUMBERVAL(index);
    e2::Int_e val = e2q::e2_share_array.get(id, idx);

    return val;
} /* -----  end of function GetArray  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  LastArray
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ArrayLast(e2::Int_e id, const char *_vname, e2::Int_e loc,
                    const char *_path)
{
    id = NUMBERVAL(id);

    std::size_t idx = e2q::e2_share_array.length(id) - 2;
    e2::Int_e val = e2q::e2_share_array.get(id, idx);

    return val;
} /* -----  end of function LastArray  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ArrayMax
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ArrayMax(e2::Int_e id, const char *_vname, e2::Int_e loc,
                   const char *_path)
{
    id = NUMBERVAL(id);

    e2::Int_e ret = e2q::e2_share_array.Max(id);
    return ret;
} /* -----  end of function ArrayMax  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ArrayMin
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e ArrayMin(e2::Int_e id, const char *_vname, e2::Int_e loc,
                   const char *_path)
{
    id = NUMBERVAL(id);

    e2::Int_e ret = e2q::e2_share_array.Min(id);
    return ret;

} /* -----  end of function ArrayMin  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Sum
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  id 值与前 P 个 X 值相加
 * ============================================
 */
e2::Int_e Sum(e2::Int_e id, e2::Int_e p)
{
    id = NUMBERVAL(id);

    e2::Int_e val = 0;
    if (p <= 0) {
        log::echo("p:", p);
        return 0;
    }
    std::size_t next = (std::size_t)NUMBERVAL(p);

    val = e2q::e2_share_array.Sum(id, next);

    return val;
} /* -----  end of function Sum  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Sqrt
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e Sqrt(e2::Int_e v)
{
    double n = NUMBERVAL(v);
    double s = std::sqrt(n);
    return VALNUMBER(s);
} /* -----  end of function Sqrt  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Stdev
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  ring loop id
 * ============================================
 */
e2::Int_e Stdev(e2::Int_e id)
{
    id = NUMBERVAL(id);

    double stdev = 0;

    auto fun = [](e2::Int_e val) {
        double ret = NUMBERVAL(val);
        return ret;
    };  // -----  end lambda  -----

    stdev = e2q::e2_share_array.Stdev(id, fun);
    return VALNUMBER(stdev);
} /* -----  end of function Stdev  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  math::Last
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e Last(e2::Int_e index)
{
    e2::Int_e b = index + 6;

    return b;
} /* -----  end of function math::Last  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Max
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e Max(e2::Int_e i1, e2::Int_e i2)
{
    return i1 > i2 ? i1 : i2;
} /* -----  end of function Max  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Maxs
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e Maxs(e2::Int_e i1, e2::Int_e i2, e2::Int_e i3)
{
    e2::Int_e o = i1 > i2 ? i1 : i2;
    return o > i3 ? o : i3;
} /* -----  end of function Maxs  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Min
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e Min(e2::Int_e i1, e2::Int_e i2)
{
    return i1 < i2 ? i1 : i2;
} /* -----  end of function Min  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Mins
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e Mins(e2::Int_e i1, e2::Int_e i2, e2::Int_e i3)
{
    e2::Int_e o = i1 < i2 ? i1 : i2;
    return o < i3 ? o : i3;
} /* -----  end of function Mins  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  between
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Bool between(e2::Int_e val, e2::Int_e start, e2::Int_e end)
{
    e2::Bool b = e2::Bool::B_FALSE;
    (start <= val && val <= end) ? b = e2::Bool::B_TRUE : b = e2::Bool::B_FALSE;
    return b;
} /* -----  end of function between  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Abs
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e Abs(e2::Int_e v)
{
    v = abs(v);
    return v;
} /* -----  end of function Abs  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Ceil
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e Ceil(e2::Int_e v)
{
    double vd = (double)NUMBERVAL(v);
    vd = ceil(vd);
    return VALNUMBER(vd);
} /* -----  end of function Ceil  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Floor
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e Floor(e2::Int_e v)
{
    double vd = (double)NUMBERVAL(v);
    vd = floor(vd);
    return VALNUMBER(vd);

} /* -----  end of function Floor  ----- */
}  // namespace e2l
