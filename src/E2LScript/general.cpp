/*
 * =====================================================================================
 *
 *       Filename:  general.cpp
 *
 *    Description:  general
 *
 *        Version:  1.0
 *        Created:  2023年12月22日 10时46分03秒
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
#include "E2L/general.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <thread>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "E2LScript/e2lLead.hpp"
#include "E2LScript/util_inline.hpp"
#include "assembler/BaseType.hpp"
#include "libs/kafka/protocol/proto.hpp"

namespace e2l {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  isDebug
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void isDebug(e2::Bool b)
{
    std::thread::id tid = std::this_thread::get_id();
    e2q::log.Debug(tid, b);
} /* -----  end of function isDebug  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  log
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void log(e2::Int_e s, const char *_vname, e2::Int_e loc, const char *_path)
{
    std::thread::id tid = std::this_thread::get_id();

    char *p = nullptr;
    e2::Bool ret = e2q::log.isDebug(tid);
    if (ret == e2::Bool::B_FALSE) {
        return;
    }
    e2q::log.log(tid, &p);

    e2q::LogProto_t lp;
    lp.data(p);
    lp.logtype(e2q::LogType_t::BASE);
    if (s < 0) {
        lp.numtype(e2q::NumberType_t::NEGATIVE);
    }
    else {
        lp.numtype(e2q::NumberType_t::POSITIVE);
    }
    lp.value((std::uint64_t)abs(s));
    std::uint16_t decis = 0;
    lp.deci(decis);
    lp.loc((std::uint32_t)loc);
    lp.vname(_vname);
    lp.path(_path);

    E2LOG(p, lp.size(), tid);

} /* -----  end of function log  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  PrintLine
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void PrintLine(e2::Int_e s, const char *_vname, e2::Int_e loc,
               const char *_path)
{
    s = NUMBERVAL(s);
    // std::string msg = "";
    // switch (s) {
    //     case e2::LineState::L_Dotted:
    //         msg = "...............";
    //         break;
    //     case e2::LineState::L_Solid:
    //         msg = "---------------";
    //         break;
    //     default:
    //         msg = "===============";
    //         break;
    // }
    std::thread::id tid = std::this_thread::get_id();
    e2::Bool ret = e2q::log.isDebug(tid);

    if (ret == e2::Bool::B_FALSE) {
        return;
    }
    char *p = nullptr;
    e2q::log.log(tid, &p);

    e2q::LogProto_t lp;
    lp.data(p);
    lp.logtype(e2q::LogType_t::LINE);
    lp.numtype(e2q::NumberType_t::POSITIVE);

    lp.value((std::uint64_t)abs(s));
    std::uint16_t decis = 0;
    lp.deci(decis);
    lp.loc((std::uint32_t)loc);
    lp.vname(_vname);
    lp.path(_path);

    E2LOG(p, lp.size(), tid);

} /* -----  end of function PrintLine  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  PrintDeci
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void PrintDeci(e2::Int_e val, e2::Int_e deci, const char *_vname, e2::Int_e loc,
               const char *_path)
{
    int dec = NUMBERVAL(deci);

    std::thread::id tid = std::this_thread::get_id();
    e2::Bool ret = e2q::log.isDebug(tid);

    if (ret == e2::Bool::B_FALSE) {
        return;
    }

    char *p = nullptr;

    e2q::log.log(tid, &p);

    e2q::LogProto_t lp;
    lp.data(p);
    lp.logtype(e2q::LogType_t::PRO);
    if (val < 0) {
        lp.numtype(e2q::NumberType_t::NEGATIVE);
    }
    else {
        lp.numtype(e2q::NumberType_t::POSITIVE);
    }
    lp.value((std::uint64_t)abs(val));
    lp.deci((std::uint16_t)abs(dec));
    lp.loc((std::uint32_t)loc);
    lp.vname(_vname);
    lp.path(_path);

    E2LOG(p, lp.size(), tid);

} /* -----  end of function PrintDeci  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  PrintTime
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void PrintTime(e2::Int_e i, const char *_vname, e2::Int_e loc,
               const char *_path)
{
    if (i < 1) {
        return;
    }
    if (_vname == nullptr) {
        llog::bug("%d vname is null ", loc);
        return;
    }

    if (_path == nullptr) {
        llog::bug("%d path is null ", loc);
        return;
    }

    std::thread::id tid = std::this_thread::get_id();
    e2::Bool ret = e2q::log.isDebug(tid);
    if (ret == e2::Bool::B_FALSE) {
        return;
    }
    char *p = nullptr;
    e2q::log.log(tid, &p);

    e2q::LogProto_t lp;
    lp.data(p);
    lp.logtype(e2q::LogType_t::TIME);
    lp.numtype(e2q::NumberType_t::POSITIVE);

    lp.value((std::uint64_t)abs(i));
    std::uint16_t decis = 0;
    lp.deci(decis);
    lp.loc((std::uint32_t)loc);
    lp.vname(_vname);
    lp.path(_path);

    E2LOG(p, lp.size(), tid);

} /* -----  end of function PrintTime  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  StoreId
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   易失性的 id
 *   如果固定下一个周期还能使用的话，就需要自定义的 id
 * ============================================
 */
e2::Int_e StoreId(e2::Int_e loc, const char *_path)
{
    std::thread::id _id;
    AutoInc(_id, 0);

    e2::Int_e id = e2q::e2l_thread_map.StoreId(_id);

    // llog::info("id:", id, " codeline:", loc, " path:", std::string(_path));
    return VALNUMBER(id);
} /* -----  end of function StoreId  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  LastStoreId
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e LastStoreId(e2::Int_e loc, const char *_path)
{
    std::thread::id _id;
    AutoInc(_id, 0);
    e2::Int_e id = e2q::e2l_thread_map.Id(_id);
    return VALNUMBER(id);
} /* -----  end of function LastStoreId  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  isStore
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Bool isStore(e2::Int_e id)
{
    std::thread::id _id;
    id = NUMBERVAL(id);

    e2::Bool r = e2::Bool::B_FALSE;
    E2LSILK(r, _id, id);
    return r;

} /* -----  end of function isStore  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  fetch
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e fetch(e2::Int_e id)
{
    id = NUMBERVAL(id);

    std::thread::id _id;
    e2::Bool ret = e2::Bool::B_FALSE;
    E2LSILK(ret, _id, id);

    if (ret == e2::Bool::B_FALSE) {
        return VALNUMBER(-1);
    }
    e2::Int_e val = VALNUMBER(-1);
    try {
        val = e2q::e2l_silk.get(_id, id);
    }
    catch (std::exception &e) {
        std::cout << "e2l_silk " << std::endl;
    }

    return val;

} /* -----  end of function fetch  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  store
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void store(e2::Int_e id, e2::Int_e val)
{
    std::thread::id _id;
    id = NUMBERVAL(id);
    try {
        e2::Bool ret = e2::Bool::B_FALSE;
        E2LSILK(ret, _id, id);

        if (ret == e2::Bool::B_FALSE) {
            e2q::e2l_silk.insert(_id, id, val);
        }
        else {
            e2q::e2l_silk.update(_id, id, val);
        }
    }
    catch (std::exception &e) {
        std::cout << "e2l_silk 1 " << std::endl;
    }
} /* -----  end of function store  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TicketSize
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  这个不需要了
 * ============================================
 */
e2::Int_e TicketSize()
{
    return 0;
} /* -----  end of function TicketSize  ----- */
}  // namespace e2l
