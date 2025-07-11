/*
 * =====================================================================================
 *
 *       Filename:  E2Q.cpp
 *
 *    Description:  E2Q
 *
 *        Version:  1.0
 *        Created:  2022年08月15日 15时49分24秒
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

#include "../include/E2Q.hpp"

#include <string>

#include "E2LScript/util_inline.hpp"

namespace e2q {
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  E2Q:E2Q
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
E2Q::E2Q() {} /* -----  end of function E2Q:E2Q  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  E2Q::exchange
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void E2Q::exchange(int pros)
{
    log::echo("server is run!");
    if (db_path.length() == 0) {
        log::echo("properties is empty");
    }
    e2q::Fusion fusion;
    fusion.properties(db_path);

    fusion.oms(e2l_path, esearch_dir, pros);
} /* -----  end of function E2Q::exchange  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  E2Q::trader
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void E2Q::trader(size_t n, std::size_t quant_start, int run,
                 std::size_t total_pro)
{
    log::echo("node is run");
    if (db_path.length() == 0) {
        log::echo("properties is empty");
    }

    e2q::Fusion fusion;
    fusion.properties(db_path);
    fusion.ea(e2l_path, esearch_dir, n, quant_start, total_pro);

} /* -----  end of function E2Q::trader  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  E2Q:setCfg
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void E2Q::setCfg(std::string &e2l, std::string &db)
{
    e2l_path = e2l;
    db_path = db;
} /* -----  end of function E2Q:setCfg  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  E2Q::log_dir
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void E2Q::log_dir(std::string &dir)
{
#ifndef KAFKALOG
    elog.dir(dir);
#endif

} /* -----  end of function E2Q::log_dir  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  E2Q::search_dir
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void E2Q::search_dir(std::string &dir)
{
    esearch_dir = dir;
} /* -----  end of function E2Q::search_dir  ----- */
}  // namespace e2q

