/*
 * =====================================================================================
 *
 *       Filename:  ScriptThread.cpp
 *
 *    Description:  ScriptThread
 *
 *        Version:  1.0
 *        Created:  2023年12月20日 14时41分45秒
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

#include "E2LScript/ScriptThread.hpp"

#include <cstddef>
#include <string>
#include <vector>

#include "E2L/E2LType.hpp"
#include "Toolkit/GlobalConfig.hpp"
#include "assembler/BaseType.hpp"
#include "assembler/CodeGenContext.hpp"

namespace e2q {
using namespace e2;

/**
 * extern
 */

/**
 * Stash memory for ea
 */
std::shared_ptr<ContainerStash> e2l_cnt = nullptr;

/**
 * Stash StrategyMaster memory for ea
 */
// ThreadDisruptorStashSharePtr ext_sb_ptr = nullptr;

/**
 * fun list
 */
std::vector<E2lFun_t> funList;
std::vector<std::string> stFunList;
/**
 * fix struct
 */
std::shared_ptr<FinancialInstrument> FixPtr = nullptr;

std::shared_ptr<FinancialFabricate> FinFabr = nullptr;
/**
 *  extern end
 */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ScriptThread::init
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void ScriptThread::init(const char* file, std::string edir)
{
    if (file == nullptr) {
        log::bug("e2 language file is empty");
        return;
    }

    switch (_type) {
        case e2lType::_EA: {
            FixPtr = std::make_shared<e2q::FinancialInstrument>();
            FixPtr->_offers = e2::Offers::OF_Tick;
            FixPtr->_ok = e2::InitOk::I_Proc;
            FixPtr->_gmt = 0;
            FixPtr->_analy.cash = 999000.0;

            break;
        }
        case e2lType::_OMS: {
            FinFabr = std::make_shared<FinancialFabricate>();
            FinFabr->_ok = e2::InitOk::I_Proc;
            break;
        }
        default:
            break;
    }
    if (edir.length() > 0) {
        _ctx.search_path(edir.c_str());
    }
    _ctx.toparse(file);

    e2lInit();
} /* -----  end of function ScriptThread::init  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ScriptThread::e2lInit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void ScriptThread::e2lInit()
{
    createCoreFunctions(_context);

    switch (_type) {
        case e2lType::_EA: {
            EAE2LFun();
            break;
        }
        case e2lType::_OMS: {
            OMSE2LFun();
            break;
        }
        default:
            break;
    }

    for (E2lFun_t it : funList) {
        _context.ExternBuildInt(std::get<0>(it), std::get<1>(it),
                                std::get<2>(it), std::get<3>(it));
    }

    if (llvm_ir) {
        _context.toDebug();
    }

    _isgc = _context.generateCode(_ctx.RootBlock());
    if (_isgc) {
        _context.runCode();
    }
    else {
        e2q::log::bug("isgc is error");
    }
} /* -----  end of function ScriptThread::e2lInit  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ScriptThread::toScript
 *  ->  void *
 *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int ScriptThread::toScript(double argc, double argv)
{
    int ret = -1;
    if (_isgc) {
        ret = _context.runFunction(argc, argv);
    }
    else {
        e2q::log::bug("generateCode is error");
    }
    return ret;
} /* -----  end of function ScriptThread::toScript  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ScriptThread::MemSize
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
size_t ScriptThread::MemSize()
{
    const std::vector<ScriptList_t> sl = _context.ScriptList();
    std::size_t num = 0;
    for (ScriptList_t st : sl) {
        if (st.nt == e2::NodeType::_methodcall && st.name == "fetch") {
            num++;
        }

        if (st.nt == e2::NodeType::_function) {
            stFunList.push_back(st.name);
        }
        /* if (st.nt == e2::NodeType::_variable || */
        /*     st.nt == e2::NodeType::_identifier) { */
        /*     log::echo(st.name); */
        /* } */
    }
    return num;
} /* -----  end of function ScriptThread::MemSize  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ScriptThread::CheckCall
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool ScriptThread::CheckCall()
{
    const std::vector<ScriptError_t> se = _context.ScriptError();
    bool ret = false;
    if (se.size() > 0) {
        ret = true;
    }
    return ret;
} /* -----  end of function ScriptThread::CheckCall  ----- */

}  // namespace e2q
