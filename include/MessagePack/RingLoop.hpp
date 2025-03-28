/*
 * =====================================================================================
 *
 *       Filename:  RingLoop.hpp
 *
 *    Description:  RingLoop
 *
 *        Version:  1.0
 *        Created:  2024年07月11日 16时43分46秒
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

#ifndef RINGLOOP_INC
#define RINGLOOP_INC
#include <cmath>
#include <cstddef>
#include <numeric>
#include <thread>
#include <vector>

#include "E2L/E2LType.hpp"
#include "MessagePack/foreign.hpp"
#include "assembler/BaseType.hpp"
namespace e2q {
struct __RingLoop {
    std::vector<e2::Int_e> data;
    std::size_t length = 0;
    std::size_t pos = 0;
    std::size_t cur = 0;

}; /* ----------  end of struct __RingLoop  ---------- */
typedef __RingLoop RingLoopType;

#define CHECKARRAY(ret, idx, id)                                         \
    ({                                                                   \
        do {                                                             \
            if (_data.count(id) == 0 || _data.at(id).data.size() == 0 || \
                _data.at(id).data.size() < idx) {                        \
                ret = e2::Bool::B_FALSE;                                 \
            }                                                            \
            if (ret == e2::Bool::B_TRUE) {                               \
                std::size_t size = _data.at(id).data.size();             \
                idx = (_data.at(id).cur + idx) % size;                   \
            }                                                            \
        } while (0);                                                     \
    })

#define MAXMIN(id, ret, op)                                              \
    ({                                                                   \
        do {                                                             \
            if (_data.count(id) == 0) {                                  \
                return ret;                                              \
            }                                                            \
            for (std::size_t m = 0; m < _data.at(id).data.size(); m++) { \
                std::size_t idx =                                        \
                    (_data.at(id).pos + m) % _data.at(id).length;        \
                if (m == 0) {                                            \
                    ret = _data.at(id).data[idx];                        \
                }                                                        \
                else {                                                   \
                    ret = ret op _data.at(id).data[idx]                  \
                              ? ret                                      \
                              : _data.at(id).data[idx];                  \
                }                                                        \
            }                                                            \
        } while (0);                                                     \
    })

/*
 * ================================
 *        Class:  RingLoop
 *  Description:
 * ================================
 */
class RingLoop {
public:
    /* =============  LIFECYCLE     =================== */
    RingLoop(); /* constructor */

    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */
    bool init(e2::Int_e, std::size_t);
    e2::Bool add(e2::Int_e, e2::Int_e);
    std::size_t length(e2::Int_e);
    std::size_t size(e2::Int_e);

    std::size_t length(std::thread::id, e2::Int_e);
    std::size_t size(std::thread::id, e2::Int_e);

    void fixed(e2::Int_e);

    e2::Int_e get(std::thread::id, e2::Int_e, std::size_t);
    bool Ptr(e2::Int_e, double **ptr, std::size_t);
    e2::Bool update(e2::Int_e id, std::size_t, e2::Int_e x);
    e2::Bool update(std::thread::id, e2::Int_e id, std::size_t, e2::Int_e x);

    e2::Int_e Max(e2::Int_e);
    e2::Int_e Min(e2::Int_e);
    e2::Int_e Sum(e2::Int_e id, std::size_t p);
    double Stdev(e2::Int_e, func_type_ret<double, e2::Int_e> fun);
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    using Mutex = typename e2q::BasicLock::mutex_type;
    mutable Mutex _RLoopMutex;

private:
    /* =============  METHODS       =================== */
    void Debug(e2::Int_e);
    /* =============  DATA MEMBERS  =================== */
    // std::map<e2::Int_e, RingLoopType> _data;

    std::map<std::thread::id, std::map<e2::Int_e, RingLoopType>> _ring_data;

}; /* -----  end of class RingLoop  ----- */

}  // namespace e2q
#endif /* ----- #ifndef RINGLOOP_INC  ----- */
