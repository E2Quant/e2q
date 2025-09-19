/*
 * =====================================================================================
 *
 *       Filename:  StrategyBase.hpp
 *
 *    Description: StrategyBase Thread Base Class
 *
 *        Version:  1.0
 *        Created:  2022年07月28日 11时55分47秒
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

#ifndef STRATEGYBASE_INC
#define STRATEGYBASE_INC
#include <unistd.h>

#include <cstddef>
#include <memory>
#include <string>

#include "ControlPack/ThreadPool.hpp"
#include "E2LScript/ScriptThread.hpp"
#include "StrategyPack/Indicators.hpp"
#include "StrategyPack/foreign.hpp"
namespace e2q {

/*
 * =====================================================================================
 *        Class:  StrategyBase
 *  Description:
 * =====================================================================================
 */
class StrategyBase {
public:
    /* ====================  LIFECYCLE =======================================
     */
    StrategyBase() {};
    StrategyBase(_Resource_ptr ptr, std::shared_ptr<BeamData> beam_data,
                 std::size_t); /* constructor */

    /* ====================  ACCESSORS =======================================
     */

    void ProgramInit(std::string &file, std::string &);

    /* ====================  MUTATORS =======================================
     */

    void addInd(std::shared_ptr<Indicators> ptr);

    void runScript();
    /* ====================  OPERATORS =======================================
     */

protected:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */
    std::string _e2l_path;
    ScriptThread _program{e2lType::_EA};
    std::size_t _mem_size = 0;
    _Resource_ptr _source_ptr{nullptr};

    std::size_t _quantIdStart = 0;

    std::size_t thread_map_size = 0;
}; /* -----  end of class StrategyBase  ----- */

struct StrategyBeam : public ConnectBeamClass<ConnectSignal> {
    void resource(_Resource_ptr ptr)
    {
        _source_ptr = std::move(ptr);
        _master =
            _source_ptr->ResourcePtr<ThreadDisruptorStash, ThreadDisruptor>(
                _col);
        if (_master != nullptr) {
            _master->id = 1;
            _master->data_ptr->name("master");
            _master->data_ptr->full();
        }
        else {
            elog::bug("ThreadDisruptorStash is nullptr");
        }
    };
    void callback(std::shared_ptr<ConnectSignal> beam)
    {
        if (beam == nullptr || beam->id != SigId::_strategy_id) {
            // elog::bug("ConnectSignal is null");

            return;
        }
        if (_master == nullptr) {
            elog::bug("master is null");
            return;
        }
        size_t inc = 1;
        BeamStash(beam, ThreadDisruptor, this->_master, inc, "risk manager");
    };

private:
    std::size_t _col = 3;
    ThreadDisruptorStashSharePtr _master{nullptr};
}; /* ----------  end of struct StrategyBeam  ---------- */

/**
 * strategy and risk
 */
typedef struct StrategyBeam StrategyBeam;

}  // namespace e2q
#endif /* ----- #ifndef STRATEGYBASE_INC  ----- */

