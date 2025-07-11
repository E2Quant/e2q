/*
 * =====================================================================================
 *
 *       Filename:  StrategyBase.cpp
 *
 *    Description:  StrategyBase
 *
 *        Version:  1.0
 *        Created:  2022年11月20日 19时21分10秒
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
#include "StrategyPack/StrategyBase.hpp"

#include <unistd.h>

#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <memory>
#include <string>
#include <thread>
#include <utility>

#include "E2L/E2LType.hpp"
#include "E2L/general.hpp"
#include "E2LScript/util_inline.hpp"

namespace e2q {

// inline std::size_t ticket_offset = 0;
inline std::size_t e2l_thread_num = 1;
inline std::size_t ticket_now = 0;
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  StrategyBase:StrategyBase
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
StrategyBase::StrategyBase(_Resource_ptr ptr,
                           std::shared_ptr<BeamData> beam_data,
                           std::size_t quantId_start)
{
    _quantIdStart = quantId_start;
    if (ptr == nullptr) {
        log::bug("sourece ptr is nullptr!!!");
    }
    else {
        _source_ptr = std::move(ptr);
    }

    if (beam_data != nullptr) {
        StrategyBeam _sbeam;
        _sbeam.resource(_source_ptr);
        beam_data->assign<StrategyBeam, Connect_beam>(_sbeam);
    }

} /* -----  end of function StrategyBase:StrategyBase  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  StrategyBase::ProgramInit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void StrategyBase::ProgramInit(std::string &file, std::string &edir)
{
    Int_e argc = 0;

    _e2l_path = file;
    _program.init(_e2l_path.c_str(), edir);

    bool call = _program.CheckCall();

    if (call) {
        log::bug("call method is error!!");
        return;
    }
    _mem_size = _program.MemSize();

    /**
     * 主要是跑第一次 config 各种配置的
     * 初始化 bar, cash ...
     */
    if (FixPtr != nullptr && FixPtr->_ok == e2::InitOk::I_Proc) {
        _program.toScript(argc, argc);
    }
    else {
        log::bug("FixPtr is nullptr or ok != proc");
    }

    GlobalDBPtr->e2lThread(e2l_thread_num);
} /* -----  end of function StrategyBase::ProgramInit  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  StrategyBase::runScript
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   feedpack (ohlc or tick) to e2l script
 * ============================================
 */
void StrategyBase::runScript()
{
    if (_source_ptr == nullptr) {
        return;
    }

    size_t type = typeid(ContainerStash).hash_code();

    e2l_cnt = _source_ptr->fetch<ContainerStash>(type);
    if (e2l_cnt == nullptr) {
        /**
         * 有时候 FeedBalance::obtain() 还没有准备好，就找不到了，以后再优化
         */
        log::bug("e2l_cnt is null");
        return;
    }

    ticket_now = 0;
    std::size_t e2l_count = 0;
    std::size_t next_row = 0;

    std::srand(getpid());

    auto job_init = [this](std::size_t num, std::thread::id _id) {
        std::size_t _uuid = 0;

        AutoInc(_id, num);
        if (FixPtr->_quantId.count(_id) == 0) {
            _uuid = this->_quantIdStart + num;

            FixPtr->_quantId.insert({_id, {_uuid, num}});

            e2q::e2_analse.init(_id);

#ifndef KAFKALOG
            elog.init(_id);
#endif
        }
    };  // -----  end lambda  -----

    auto job = [this](std::size_t tk_size, size_t num, std::size_t number,
                      std::thread::id _id) {
        // 每次重来，都要初始化一下 storeid 这些值
        e2l_thread_map.AutoInit(_id, num);

        // try {
        this->_program.toScript(tk_size, num);
        e2l_thread_map.runs(_id);
        // }
        // catch (std::exception &e) {
        //     log::bug(e.what(), " num:", num);
        // }
    };  // -----  end ambda  -----

    ThreadPool<std::size_t> pool{e2l_thread_num};
    pool.job_init(job_init);
    pool.jobs(job);
    pool.init();

    do {
        next_row = e2l_cnt->data_ptr->aquire();

        for (; e2l_count < next_row; e2l_count++) {
            pool.emit(e2l_count);

            ticket_now = e2l_cnt->data_ptr->now();
        }
        //  运行得有点慢

        /**
         * 单个 tick 报价 或 以 index 报价
         * 驱动
         *
         */
        next_row = e2l_cnt->data_ptr->aquire();

        if (next_row == e2l_count) {
            e2l_cnt->data_ptr->wait_for();
        }

    } while (e2l_cnt->data_ptr->runing() ==
             false); /* -----  end do-while  ----- */

    pool.exits();

    elog.exist();

    // e2l_thread_map.dump();
    /**
     * 先不管，到时候 优化再做
     */

    log::bug("e2l end ............ ");
} /* -----  end of function StrategyBase::runScript  ----- */

}  // namespace e2q

