/*
 * =====================================================================================
 *
 *       Filename:  RiskManager.hpp
 *
 *    Description:  RiskManager
 *
 *        Version:  1.0
 *        Created:  2022年08月02日 17时47分56秒
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

#ifndef RISKMANAGER_INC
#define RISKMANAGER_INC
#include <array>
#include <cstddef>
#include <future>
#include <memory>
#include <vector>

#include "TradePack/STLog.hpp"
#include "TradePack/foreign.hpp"
namespace e2q {

/*
 * =====================================================================================
 *        Class:  RiskManager
 *  Description:
 * =====================================================================================
 */
class RiskManager {
public:
    /* ====================  LIFECYCLE =======================================
     */
    RiskManager(); /* constructor */

    /* ====================  ACCESSORS =======================================
     */
    /**
     * ticks
     */
    void weight(size_t w);

    void unique(bool u);

    void sole(bool s);

    /**
     *  end ticks
     */

    void stLog(std::shared_ptr<STLog> ptr);
    /* ====================  MUTATORS ======================================= */

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

}; /* -----  end of class RiskManager  ----- */

/**
 * 接收策略反馈的数据
 *  在另一个进程了
 */
struct RiskBeam : public FuncBeamClass<FuncSignal> {
    void callback(std::shared_ptr<FuncSignal> beam)
    {
        if (beam == nullptr || beam->id != SigId::_strategy_id) {
            elog::bug("FuncSignal is null");

            return;
        }

        bridge::ReceiveType<ThreadDisruptorStashSharePtr> rfun =
            [](ThreadDisruptorStashSharePtr ptr) {
                auto thread_fun = [ptr]() {
                    auto call_fun = [ptr](SeqType start, SeqType end) {
                        elog::echo(ptr->data_ptr->name(), " aquire:", start,
                                  " row:", end);

                        std::array<SeqType, 3> data;
                        int ret = ptr->data_ptr->read(&data, 0);
                        if (ret != -1) {
                            elog::echo("row:", 0, " data 0:", data[0],
                                      " 1:", data[1], " 2:", data[2]);
                        }
                        else {
                            elog::echo("ret is :", ret);
                        }
                    };  // -----  end lambda  -----
                    ptr->data_ptr->wait_for(call_fun);
                };  // -----  end lambda  -----
                THREAD_FUN(thread_fun);
            };  // -----  end lambda -----

        beam->receive(rfun);
        elog::echo("risk end..");
    }
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  shareptr
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    void shareptr(_Resource_ptr ptr)
    {
        _source_ptr = ptr;
        _share_ptr =
            _source_ptr->ResourcePtr<ThreadDisruptorStash, ThreadDisruptor>(
                _col);
    } /* -----  end of function shareptr  ----- */
private:
    ThreadDisruptorStashSharePtr _share_ptr{nullptr};
    std::size_t _col = 3;
}; /* ----------  end of struct RiskBeam  ---------- */

typedef struct RiskBeam RiskBeam;
}  // namespace e2q
#endif /* ----- #ifndef RISKMANAGER_INC  ----- */

