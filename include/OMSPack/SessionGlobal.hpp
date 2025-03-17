/*
 * =====================================================================================
 *
 *       Filename:  SessionGlobal.hpp
 *
 *    Description:  SessionGlobal
 *
 *        Version:  1.0
 *        Created:  2024年04月08日 17时53分04秒
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

#ifndef SESSIONGLOBAL_INC
#define SESSIONGLOBAL_INC

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "OMSPack/Matcher/BaseMatcher.hpp"
#include "Toolkit/Norm.hpp"

#if __cplusplus >= 201703L

/* Now remove the trow */
#define throw(...)

#include <quickfix/SessionID.h>

#undef throw /* reset */
#endif

namespace e2q {
// _fix_symbol id
inline std::map<FIX::SessionID, std::vector<std::size_t>> SessionSymList;

/**
 * symbol,_PreTime, _PreNow, _PrePrice,_PreAdjPrice
 */
struct __mpType {
    std::size_t PreTime = 0;
    std::size_t PreNow = 0;
    e2::Int_e PrePrice = 0;
    e2::Int_e PreAdjPrice;
}; /* ----------  end of struct __mpType  ---------- */
typedef struct __mpType mpType;

struct __MarketPrice {
    bool checkSymbol(std::string sym) { return _data.count(sym) == 1; }
    void init(std::string sym)
    {
        BasicLock _lock(_EMute);
        mpType data;
        if (_data.count(sym) == 0) {
            _data.insert({sym, data});
        }
    }
    mpType get(std::string sym) { return _data.at(sym); }
    void set(std::string sym, mpType data)
    {
        BasicLock _lock(_EMute);

        _data[sym] = data;
    }

private:
    std::map<std::string, mpType> _data;
    using EMute = BasicLock::mutex_type;
    mutable EMute _EMute;

}; /* ----------  end of struct __MarketPrice  ---------- */

typedef struct __MarketPrice MarketPrice;

inline std::shared_ptr<BaseMatcher> GlobalMatcher{nullptr};

}  // namespace e2q
#endif /* ----- #ifndef SESSIONGLOBAL_INC  ----- */
