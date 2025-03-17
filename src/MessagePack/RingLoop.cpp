/*
 * =====================================================================================
 *
 *       Filename:  RingLoop.cpp
 *
 *    Description:  RingLoop
 *
 *        Version:  1.0
 *        Created:  2024年07月12日 09时11分36秒
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
#include "MessagePack/RingLoop.hpp"

#include <cstddef>
#include <exception>
#include <vector>

#include "E2L/E2LType.hpp"
#include "assembler/BaseType.hpp"
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::RingLoop
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
RingLoop::RingLoop() {} /* -----  end of function RingLoop::RingLoop  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::init
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool RingLoop::init(e2::Int_e id, std::size_t length)
{
    BasicLock _lock(_RLoopMutex);
    std::thread::id _tid = std::this_thread::get_id();
    std::map<e2::Int_e, RingLoopType> _data;
    if (_ring_data.count(_tid) == 0) {
        RingLoopType rn;
        _data.insert({id, rn});
        _data.at(id).length = length;

        _ring_data.insert({_tid, _data});
        return true;
    }

    _data = _ring_data.at(_tid);
    if (_data.count(id) == 0) {
        RingLoopType rn;
        rn.length = length;

        _ring_data.at(_tid).insert({id, rn});

        return true;
    }
    else {
        if (_ring_data.at(_tid).at(id).length != length) {
            log::bug("id:", id, " len:", _ring_data.at(_tid).at(id).length,
                     " now length:", length);
            _ring_data.at(_tid).at(id).length = length;
            return false;
        }
    }

    if (_ring_data.at(_tid).at(id).length == 0) {
        _ring_data.at(_tid).at(id).length = length;
    }

    return true;

} /* -----  end of function RingLoop::init  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::length
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::size_t RingLoop::length(e2::Int_e id)
{
    std::thread::id _tid = std::this_thread::get_id();

    return length(_tid, id);
}
std::size_t RingLoop::length(std::thread::id _tid, e2::Int_e id)
{
    std::map<e2::Int_e, RingLoopType> _data;
    if (_ring_data.count(_tid) == 0) {
        return 0;
    }
    _data = _ring_data.at(_tid);
    if (_data.count(id) == 0) {
        return 0;
    }
    return _data.at(id).length;
} /* -----  end of function RingLoop::length  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::size
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::size_t RingLoop::size(e2::Int_e id)
{
    std::thread::id _tid = std::this_thread::get_id();

    return size(_tid, id);
}

std::size_t RingLoop::size(std::thread::id _tid, e2::Int_e id)
{
    std::map<e2::Int_e, RingLoopType> _data;
    if (_ring_data.count(_tid) == 0) {
        return 0;
    }
    _data = _ring_data.at(_tid);
    if (_data.count(id) == 0) {
        log::bug("id not exit id:", id, " data size:", _data.size());
        return 0;
    }
    return _data.at(id).data.size();
} /* -----  end of function RingLoop::size  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::add
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Bool RingLoop::add(e2::Int_e id, e2::Int_e val)
{
    size_t pos = 0;
    std::map<e2::Int_e, RingLoopType> _data;

    BasicLock _lock(_RLoopMutex);

    std::thread::id _tid = std::this_thread::get_id();
    if (_ring_data.count(_tid) == 0) {
        return e2::Bool::B_FALSE;
    }
    _data = _ring_data.at(_tid);
    if (_data.count(id) == 0 || _data.at(id).length == 0) {
        return e2::Bool::B_FALSE;
    }

    if (_data.at(id).data.size() < _data.at(id).length) {
        _ring_data.at(_tid).at(id).data.push_back(val);
        _ring_data.at(_tid).at(id).pos++;
    }
    else {
        pos = _data.at(id).pos % _data.at(id).length;
        _ring_data.at(_tid).at(id).data[_data.at(id).cur] = val;
        _ring_data.at(_tid).at(id).cur =
            (_data.at(id).pos + 1) % _data.at(id).length;

        _ring_data.at(_tid).at(id).pos = pos + 1;
    }

    return e2::Bool::B_TRUE;
} /* -----  end of function RingLoop::add  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::Debug
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void RingLoop::Debug(e2::Int_e id)
{
    std::thread::id _tid = std::this_thread::get_id();
    std::map<e2::Int_e, RingLoopType> _data;
    if (_ring_data.count(_tid) == 0) {
        return;
    }
    _data = _ring_data.at(_tid);

    std::size_t m = _data.at(id).cur;
    std::cout << "[ ";
    for (std::size_t l = 0; l < _data.at(id).data.size(); l++) {
        std::size_t idx = (m) % _data.at(id).data.size();

        std::cout << _data.at(id).data[idx] << ", ";
        m++;
    }
    std::cout << " ]" << std::endl;
} /* -----  end of function RingLoop::Debug  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::fixed
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void RingLoop::fixed(e2::Int_e id)
{
    std::thread::id _tid = std::this_thread::get_id();
    std::map<e2::Int_e, RingLoopType> _data;
    if (_ring_data.count(_tid) == 0) {
        return;
    }
    BasicLock _lock(_RLoopMutex);
    _data = _ring_data.at(_tid);

    if (_data.count(id) == 0) {
        log::bug("fixed not found id:", id);

        return;
    }

    if (_data.at(id).data.size() != _data.at(id).length) {
        _ring_data.at(_tid).at(id).data.resize(_data.at(id).length);
    }

} /* -----  end of function RingLoop::fixed  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::get
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e RingLoop::get(std::thread::id _tid, e2::Int_e id, std::size_t idx)
{
    e2::Int_e val = 0;

    e2::Bool ret = e2::Bool::B_TRUE;

    std::map<e2::Int_e, RingLoopType> _data;
    if (_ring_data.count(_tid) == 0) {
        return val;
    }

    _data = _ring_data.at(_tid);

    CHECKARRAY(ret, idx, id);

    if (ret == e2::Bool::B_TRUE) {
        val = _data.at(id).data.at(idx);
    }

    return val;

} /* -----  end of function RingLoop::get  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::Ptr
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool RingLoop::Ptr(e2::Int_e id, double **ptr, std::size_t len)
{
    std::thread::id _tid = std::this_thread::get_id();

    std::map<e2::Int_e, RingLoopType> _data;
    if (_ring_data.count(_tid) == 0) {
        return false;
    }
    _data = _ring_data.at(_tid);

    if (_data.count(id) == 0 || ptr == nullptr) {
        return false;
    }
    std::size_t m = _data.at(id).cur;

    for (std::size_t l = 0; l < _data.at(id).data.size(); l++) {
        std::size_t idx = (m) % _data.at(id).data.size();
        if (l == len) {
            break;
        }
        e2::Int_e v = _data.at(id).data[idx];

        (*ptr)[l] = NUMBERVAL(v);
        m++;
    }

    return true;
} /* -----  end of function RingLoop::Ptr  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::update
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Bool RingLoop::update(e2::Int_e id, std::size_t idx, e2::Int_e x)
{
    std::thread::id _tid = std::this_thread::get_id();
    return update(_tid, id, idx, x);
}
e2::Bool RingLoop::update(std::thread::id _tid, e2::Int_e id, std::size_t idx,
                          e2::Int_e x)
{
    e2::Bool ret = e2::Bool::B_TRUE;

    std::map<e2::Int_e, RingLoopType> _data;
    if (_ring_data.count(_tid) == 0) {
        log::bug("thread is empty ring data!");
        return e2::Bool::B_FALSE;
    }
    _data = _ring_data.at(_tid);

    if (_data.count(id) == 0) {
        log::bug("not found id:", id);
        return e2::Bool::B_FALSE;
    }

    if (_data.at(id).data.size() > idx) {
        BasicLock _lock(_RLoopMutex);

        CHECKARRAY(ret, idx, id);
        _ring_data.at(_tid).at(id).data[idx] = x;
    }
    else {
        add(id, x);
    }
    return ret;
} /* -----  end of function RingLoop::update  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::Max
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e RingLoop::Max(e2::Int_e id)
{
    e2::Int_e ret = 0;
    std::thread::id _tid = std::this_thread::get_id();

    std::map<e2::Int_e, RingLoopType> _data;
    if (_ring_data.count(_tid) == 0) {
        return e2::Bool::B_FALSE;
    }
    _data = _ring_data.at(_tid);

    MAXMIN(id, ret, >);

    return ret;

} /* -----  end of function RingLoop::Max  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::Min
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e RingLoop::Min(e2::Int_e id)
{
    e2::Int_e ret = 0;
    std::thread::id _tid = std::this_thread::get_id();

    std::map<e2::Int_e, RingLoopType> _data;
    if (_ring_data.count(_tid) == 0) {
        return e2::Bool::B_FALSE;
    }
    _data = _ring_data.at(_tid);

    MAXMIN(id, ret, <);

    return ret;
} /* -----  end of function RingLoop::Min  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::Sum
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e RingLoop::Sum(e2::Int_e id, std::size_t next)
{
    e2::Int_e val = 0;
    if (next <= 0) {
        return 0;
    }
    std::size_t idx = 0;
    std::thread::id _tid = std::this_thread::get_id();

    std::map<e2::Int_e, RingLoopType> _data;
    if (_ring_data.count(_tid) == 0) {
        return val;
    }
    _data = _ring_data.at(_tid);

    if (_data.count(id) == 0) {
        return val;
    }
    if (next > _data.at(id).data.size()) {
        return val;
    }

    for (std::size_t m = 0; m < next; m++) {
        idx = (_data.at(id).pos + m) % _data.at(id).length;

        val += _data.at(id).data[idx];
    }

    return val;
} /* -----  end of function RingLoop::Sum  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  RingLoop::Stdev
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
double RingLoop::Stdev(e2::Int_e id, func_type_ret<double, e2::Int_e> fun)
{
    std::thread::id _tid = std::this_thread::get_id();

    std::map<e2::Int_e, RingLoopType> _data;
    if (_ring_data.count(_tid) == 0) {
        return 0;
    }
    _data = _ring_data.at(_tid);

    if (_data.count(id) == 0) {
        return 0;
    }
    std::size_t size = _data.at(id).data.size();
    e2::Int_e old_val = 0;
    std::size_t idx = 0;
    double val = 0.0;
    std::vector<double> vals;

    double sum = 0;
    if (fun != nullptr) {
        for (std::size_t m = 0; m < size; m++) {
            idx = (_data.at(id).cur + m) % size;
            old_val = _data.at(id).data[idx];

            val = fun(old_val);
            vals.push_back(val);
        }
        sum = std::accumulate(vals.begin(), vals.end(), 0.0);
    }
    else {
        sum = std::accumulate(_data.at(id).data.begin(),
                              _data.at(id).data.end(), 0.0);
    }

    double mean = sum / (double)size;

    double sq_sum =
        std::inner_product(vals.begin(), vals.end(), vals.begin(), 0.0);
    double sq_mean = sq_sum / (double)size - mean * mean;

    double stdev = std::sqrt(sq_mean);
    return stdev;

} /* -----  end of function RingLoop::Stdev  ----- */
}  // namespace e2q
