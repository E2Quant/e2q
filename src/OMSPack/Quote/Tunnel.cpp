/*
 * =====================================================================================
 *
 *       Filename:  Tunnel.cpp
 *
 *    Description:  Tunnel
 *
 *        Version:  1.0
 *        Created:  2022年08月15日 11时54分46秒
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

#include "OMSPack/Quote/Tunnel.hpp"

#include <bits/stdint-uintn.h>
#include <unistd.h>

#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Tunnel::handle
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Tunnel::handle(TradType tt)
{
    std::ifstream stream;

    std::string lineStream;
    size_t m = 0;
#ifdef DEBUG
    int test = 0;
#endif
    UtilTime ut;
    const char* fmt = "%Y-%m-%d";
    BigNumber bnum;
    int64_t _index;
    float _val;
    std::array<SeqType, trading_protocols> _tun_data;

    //    int ret = 0;
    std::ios_base::iostate exceptionMask =
        stream.exceptions() | std::ios::failbit;
    stream.exceptions(exceptionMask);

    std::size_t _symbol_id = 0;
    std::string sym_file = FinFabr->_fix_symbols.at(_symbol_id).symbol;

    std::string _path = _dir + "/" + sym_file + ".csv";

    try {
        stream.open(_path.c_str());
    }
    catch (std::ios_base::failure& e) {
        elog::bug(e.what());
    }

    if (!stream.is_open()) {
        elog::bug("open error:", _path);
        return;
    }
    // first line for title
    std::getline(stream, _line);

    while (getline(stream, _line)) {
        if (stream.peek() == EOF) {
            elog::echo("eof:");

            break;
        }

#ifdef DEBUG
        if (test > 20) {
            break;
        }
        test++;
#endif
        std::istringstream iss(_line);
        m = 0;

        while (getline(iss, lineStream, ',')) {
            if (m == 0) {
                _index = ut.strtostamp(lineStream, fmt);

                bnum.mergeLong(_index);
                _tun_data.at(m) = bnum.number() / bnum.deci();
            }
            else {
                _val = std::stof(lineStream);
                bnum.mergeFloat(_val);
                _tun_data.at(m) = bnum.number();
            }

            m++;
            if (m > trading_protocols) {
                m = 0;
            }
        }
        /**
         * bar to tick
         */
        _tun_data.at(trading_protocols - 1) = _symbol_id;
        /**
         * 转到  fix 上面去
         */
        tt(_tun_data);

        TSleep(0);
    }
    stream.close();
#ifdef DEBUG

    elog::info("tunnel is end..  test:", test);
#endif
} /* -----  end of function Tunnel::handle  ----- */

}  // namespace e2q
