/*
 * =====================================================================================
 *
 *       Filename:  system.cpp
 *
 *    Description:  system
 *
 *        Version:  1.0
 *        Created:  2024年07月08日 14时43分33秒
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
#include <cstddef>
#include <thread>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "E2LScript/e2lLead.hpp"
#include "assembler/BaseType.hpp"
namespace e2l {
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ethread_id
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void ethread_id()
{
    std::cout << std::this_thread::get_id() << std::endl;

} /* -----  end of function ethread_id  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  process_id
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e process_id()
{
    e2::Int_e number = VALNUMBER(e2q::GlobalProcessId);
    return number;
} /* -----  end of function process_id  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  process_runs
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e process_runs()
{
    e2::Int_e number = VALNUMBER(e2q::process_run_number);
    return number;
} /* -----  end of function process_runs  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ethread
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void ethread(e2::Int_e num)
{
    size_t _num = NUMBERVAL(num);
    if (_num <= 0) {
        _num = 1;
    }
    const auto processor_count = std::thread::hardware_concurrency();
    if (_num > processor_count) {
        _num = processor_count;
    }
    e2q::e2l_thread_num = _num;
} /* -----  end of function ethread  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TypeOf
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Bool TypeOf(const char *fun)
{
    if (!fun) {
        fprintf(stderr, "fun is null\n");
        return e2::Bool::B_FALSE;
    }
    e2::Bool r = e2::Bool::B_FALSE;
    std::string fun_c = std::string(fun);

    auto ret =
        std::find(std::begin(e2q::stFunList), std::end(e2q::stFunList), fun_c);
    if (ret != std::end(e2q::stFunList)) {
        r = e2::Bool::B_TRUE;
    }

    return r;
} /* -----  end of function TypeOf  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  SymbolUnion
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void SymbolUnion()
{
    std::cout << "union ISymbols { \n";

    for (auto syms : e2q::FixPtr->_fix_symbols) {
        std::cout << "  " << syms.second << "=" << syms.first << ";\n";
    }
    std::cout << "}" << std::endl;

} /* -----  end of function SymbolUnion  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ApiList
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void ApiList()
{
    std::string arg = "";
    for (e2q::E2lFun_t it : e2q::funList) {
        for (std::size_t m = 0; m < std::get<1>(it); m++) {
            if (arg.length() == 0) {
                arg = "a";
            }
            else {
                arg += ",a";
            }
        }

        std::cout << "\"" << std::get<2>(it) << "\": {\"prefix\":\""
                  << std::get<2>(it) << "\", \"body\": [\"" << std::get<2>(it)
                  << std::get<4>(it)
                  << "\"],\"description\": "
                     "\"e2l's "
                  << std::get<2>(it) << "\"}," << std::endl;

        arg = "";
    }

} /* -----  end of function ApiList  ----- */
}  // namespace e2l
