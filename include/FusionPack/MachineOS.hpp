/*
 * =====================================================================================
 *
 *       Filename:  MachineOS.hpp
 *
 *    Description:  MachineOS
 *
 *        Version:  1.0
 *        Created:  2024年02月01日 15时23分35秒
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

#ifndef MACHINEOS_INC
#define MACHINEOS_INC

#include <memory>
#include <string>
#include <thread>

#include "E2LScript/ExternClazz.hpp"
#include "FeedPack/FixQuote.hpp"
#include "FusionPack/foreign.hpp"
#include "OMSPack/FixAccount.hpp"
#include "StrategyPack/pack.hpp"

#if __cplusplus >= 201703L

/* Now remove the trow */
#define throw(...)
#include <quickfix/Acceptor.h>
#include <quickfix/FileStore.h>
#include <quickfix/FixFields.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/ThreadedSocketInitiator.h>
#undef throw /* reset */
#endif

namespace e2q {

/*
 * ================================
 *        Class:  MachineOS
 *  Description:  Fix Node Process:  Strategy
 * ================================
 */
class MachineOS {
public:
    /* =============  LIFECYCLE     =================== */
    MachineOS(); /* constructor */
    ~MachineOS() {}
    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */
    void enter(std::string &, std::string &, size_t, std::size_t);
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    void machine();

    /**
     * 控制，有可能不选择这个 feeddata 之类的
     */
    void ctrl();

    FIX::SessionSettings EaSetting(bool offset = false);

    /* =============  DATA MEMBERS  =================== */
    std::string _script;
    size_t _node;
    std::vector<std::string> scripts;

    std::shared_ptr<BeamData> _beam_data = nullptr;
    _Resource_ptr _resource = nullptr;
    std::shared_ptr<Shuttle> _shu_ptr = nullptr;
}; /* -----  end of class MachineOS  ----- */

}  // namespace e2q
#endif /* ----- #ifndef MACHINEOS_INC  ----- */
