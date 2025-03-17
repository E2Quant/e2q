/*
 * =====================================================================================
 *
 *       Filename:  GlobalConfig.hpp
 *
 *    Description:  GlobalConfig
 *
 *        Version:  1.0
 *        Created:  2023年07月31日 15时17分35秒
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

#ifndef GLOBALCONFIG_INC
#define GLOBALCONFIG_INC
#include <cstddef>
#include <e2l.hpp>
#include <memory>
#include <string>

#include "libs/DB/PGConnectPool.hpp"
namespace e2q {

enum FeedType {
    csv = 0,
    io
}; /* ----------  end of enum FeedType  ---------- */

typedef enum FeedType FeedType;

struct __Feed {
    std::string path;
    FeedType ft;
}; /* ----------  end of struct __path  ---------- */

typedef struct __Feed FEED;

/**
 * to llvm function point
 */
// typedef std::tuple<void *, std::string, retType *, ArgType, desc> E2lFun_t;

typedef std::tuple<void *, size_t, std::string, bool, std::string> E2lFun_t;

#define AddFun(fun, arg, name, act, desz)                                \
    ({                                                                   \
        do {                                                             \
            E2lFun_t _fun##_f =                                          \
                std::make_tuple((void *)e2l::fun, arg, name, act, desz); \
            funList.push_back(_fun##_f);                                 \
        } while (0);                                                     \
    })

#define AddFunExt(vfuns, ns, fun, arg, name, act, desz)                 \
    ({                                                                  \
        do {                                                            \
            E2lFun_t _fun##_f =                                         \
                std::make_tuple((void *)ns::fun, arg, name, act, desz); \
            vfuns.push_back(_fun##_f);                                  \
        } while (0);                                                    \
    })

/**
 * store type
 */
typedef std::array<e2::Int_e, 2> Store_t;

enum __OrderProperties {
    OP_BUY = 0,    // Buy operation
    OP_SELL,       // Sell operation
    OP_BUYLIMIT,   // Buy limit pending order
    OP_SELLLIMIT,  // Sell limit pending order
    OP_BUYSTOP,    // Buy stop pending order
    OP_SELLSTOP    // Sell stop pending order
}; /* ----------  end of enum OrderProperties  ---------- */

typedef enum __OrderProperties OrderPro_t;

/**
 * global properties
 */
// inline std::string GlobalProperties;

inline std::size_t GlobalProcessId;

/**
 * 第几次运行进程了
 */
inline int process_run_number;

inline bool process_debug;

#define pg_max_connect 3
inline std::shared_ptr<PGConnectPool> GlobalDBPtr{nullptr};

}  // namespace e2q
#endif /* ----- #ifndef GLOBALCONFIG_INC  ----- */
