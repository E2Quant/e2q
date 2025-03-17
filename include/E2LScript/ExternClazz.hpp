/*
 * =====================================================================================
 *
 *       Filename:  ExternClazz.hpp
 *
 *    Description:  ExternClazz
 *
 *        Version:  1.0
 *        Created:  2024年01月29日 16时12分01秒
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

#ifndef EXTERNCLAZZ_INC
#define EXTERNCLAZZ_INC
#include <cstddef>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "E2L/E2LType.hpp"
#include "E2LScript/foreign.hpp"
#include "assembler/BaseType.hpp"
namespace e2q {
/**
 * 在 exchange 进程了
 */

extern std::shared_ptr<FinancialFabricate> FinFabr;

/**
 * ea node machine os
 */
extern std::shared_ptr<FinancialInstrument> FixPtr;

/**
 * e2l function
 */
extern std::vector<E2lFun_t> funList;

/**
 * script function
 */

extern std::vector<std::string> stFunList;

/**
 * strategy base program run size
 */

extern std::size_t ticket_now;
/**
 * e2l thread
 */
extern std::size_t e2l_thread_num;

#define E2L_RETURN false

#define E2L_NORETURN true

/**
 * check FixPtr is nullptr
 */
#define FIX_PTR_IS_NULL()                                              \
    ({                                                                 \
        do {                                                           \
            if (FixPtr == nullptr || FixPtr->_fix_cfg.length() == 0) { \
                log::bug("FixPtr is null!");                           \
                return;                                                \
            }                                                          \
        } while (0);                                                   \
    })

#define FIN_FABR_IS_NULL()                                               \
    ({                                                                   \
        do {                                                             \
            if (FinFabr == nullptr || FinFabr->_fix_cfg.length() == 0) { \
                log::bug("FinFabr is null!");                            \
                return;                                                  \
            }                                                            \
        } while (0);                                                     \
    })

}  // namespace e2q
#endif /* ----- #ifndef EXTERNCLAZZ_INC  ----- */
