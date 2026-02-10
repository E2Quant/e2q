/*
 * =====================================================================================
 *
 *       Filename:  ScriptThread.hpp
 *
 *    Description:  ScriptThread
 *
 *        Version:  1.0
 *        Created:  2023年12月20日 14时29分22秒
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

#ifndef ScriptThread_INC
#define ScriptThread_INC
#include <assembler/BaseType.hpp>
#include <assembler/CodeGenContext.hpp>
#include <cstddef>

#include "E2LScript/ExternClazz.hpp"
#include "E2LScript/e2lLead.hpp"
#include "e2/e2l.hpp"
#include "libs/snowflake.hpp"
namespace e2q {

enum __e2lType {
    _EA,
    _OMS
}; /* ----------  end of enum __e2lType  ---------- */

typedef enum __e2lType e2lType;
/*
 * ================================
 *        Class:  ScriptThread
 *  Description:
 * ================================
 */
class ScriptThread {
public:
    /* =============  LIFECYCLE     =================== */
    ScriptThread(e2lType t) : _type(t) {}; /* constructor */

    /* =============  ACCESSORS     =================== */

    size_t MemSize();
    bool CheckCall();
    /* =============  MUTATORS      =================== */
    void init(const char* file, std::string edir);

    void e2lInit();
    int toScript(double argc, double argv);
    /**
     * script fixptr 的全局变量
     */
    void ScriptGlobal();
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    bool _isgc = false;
    CodeGenContext _context;
    e2lType _type;
    ParserCtx _ctx;

    using Mute = BasicLock::mutex_type;
    mutable Mute _SafeMutex;
}; /* -----  end of class ScriptThread  ----- */

}  // namespace e2q
#endif /* ----- #ifndef ScriptThread_INC  ----- */
