/*
 * =====================================================================================
 *
 *       Filename:  Fusion.cpp
 *
 *    Description:  Fusion
 *
 *        Version:  1.0
 *        Created:  2023年09月19日 10时46分42秒
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
#include "FusionPack/Fusion.hpp"

#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <csignal>
#include <cstddef>
#include <fstream>
#include <ios>
#include <memory>
#include <string>

#include "FusionPack/Exchange.hpp"
#include "FusionPack/MachineOS.hpp"
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Fusion::Fusion
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
Fusion::Fusion()
{
    log::echo("version:", version::version_full);

} /* -----  end of function Fusion::Fusion  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Fusion::oms
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Fusion::oms(std::string e2l_script)
{
    if (_Properties.length() > 0) {
        GlobalDBPtr =
            std::make_shared<PGConnectPool>(pg_max_connect, _Properties);
    }

    Exchange ex(e2l_script);
    ex.RiskFix();
} /* -----  end of function Fusion::oms  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Fusion::ea
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Fusion::ea(std::string e2l_script, size_t n, std::size_t quantId_start)
{
    if (_Properties.length() > 0) {
        GlobalDBPtr = std::make_shared<PGConnectPool>(1, _Properties);
    }

    MachineOS mos;
    mos.enter(e2l_script, n, quantId_start);
} /* -----  end of function Fusion::ea  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Fusion::properties
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Fusion::properties(std::string &path)
{
    std::ifstream stream(path.c_str());
    if (path.length() == 0) {
        log::bug("db properties not exits!");
        return;
    }
    if (!stream.good()) {
        log::bug("open error:", path);
        return;
    }

    std::ios_base::iostate exceptionMask =
        stream.exceptions() | std::ios::failbit;
    stream.exceptions(exceptionMask);

    if (!stream.is_open()) {
        log::bug("open error:", path);
        return;
    }
    std::getline(stream, _Properties);
    stream.close();

} /* -----  end of function Fusion::properties  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Fusion::enter
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Fusion::enter(const char *e2l_script)
{
    if (e2l_script == nullptr) {
        log::bug("path is not found");
        return;
    }
    if (_resource == nullptr) {
        log::bug("_resource is null");
        return;
    }

    auto handler = [](int sig) {
        int retval;

        if (sig == SIGCHLD) {
            // 等待子程序的結束狀態
            wait(&retval);

            log::info("CATCH SIGNAL PID=", getpid());
        }
    };  // -----  end lambda  -----

    log::echo("start fork");
    int exit_status;

    signal(SIGCHLD, handler);

    pid_t PID = fork();

    switch (PID) {
        // PID == -1 代表 fork 出錯
        case -1:
            perror("fork()");
            exit(-1);

            // PID == 0 代表是子程序
        case 0: {
            printf("[Child] I'm Child process\n");
            printf("[Child] Child's PID is %d\n", getpid());
            /* MachineOS mos(node_next); */
            /* mos.enter(e2l_script); */
            break;
        }

        // PID > 0 代表是父程序
        default: {
            printf("[Parent] I'm Parent process\n");
            printf("[Parent] Parent's PID is %d\n", getpid());

            wait(&exit_status);
        }
    }
    /* int proce = 1; */
    /* pid_t pids[proce]; */
    /* int m = 0; */
    /* for (m = 0; m < proce; m++) { */
    /*     pids[m] = fork(); */
    /*     switch (pids[m]) { */
    /*         // PID == -1 代表 fork 出錯 */
    /*         case -1: */
    /*             perror("fork()"); */
    /*             exit(-1); */

    /*         // PID == 0 代表是子程序 */
    /*         case 0: { */
    /*             printf("[Child] I'm Child process\n"); */
    /*             printf("[Child] Child's PID is %d\n", getpid()); */
    /*             MachineOS mos; */
    /*             mos.enter(e2l_script); */
    /*             sleep(5); */
    /*             break; */
    /*         } */
    /*         // PID > 0 代表是父程序 */
    /*         default: */

    /*             break; */
    /*     } */
    /* } */

    /* int exit_status; */
    /* pid_t pid; */
    /* for (m = 0; m < proce; m++) { */
    /*     pid = wait(&exit_status); */
    /*     log::echo("child pid ", pid, " status 0x%x", (long)pid, exit_status);
     */
    /* } */

    /**
     *
     * 2. Risk for all Investors
     *
     */

    /* RiskBeam _rbeam; */
    /* _rbeam.shareptr(_resource); */
    /* _beam_data->assign<RiskBeam, Func_beam>(_rbeam); */

    /* auto fun_script = [&_sbase]() { */
    /*     _sbase.runScript(); */
    /* };  // -----  end lambda  ----- */

    /* THREAD_FUN(fun_script); */

    /*
     *
     * 3. Exchange
     *
     */
    /* next(); */

    /* Exchange ex(_shu_ptr); */
    /* ex.callback(_beam_data); */
    /* ex.resource(_resource); */
    /* ex.drift(); */

} /* -----  end of function Fusion::enter  ----- */

void Fusion::next()
{
    /*
     *  StrategyBeam link RiskBeam
     *
     */

    /*auto _fun_Signal = [this](SigId id) { */
    /*    /1** */
    /*     * */
    /*     * 先跑 func */
    /*     *1/ */
    /*    std::shared_ptr<FuncSignal> risk_sig = */
    /*        std::make_shared<FuncSignal>(this->_shu_ptr); */
    /*    risk_sig->id = id; */

    /*    for (auto fn : this->_beam_data->fetch<Func_beam>()) { */
    /*        fn(risk_sig); */
    /*    } */
    /*    /1** */
    /*     * 再跑 connect ,否则没有数据 */
    /*     *1/ */
    /*    std::shared_ptr<ConnectSignal> st_sig = */
    /*        std::make_shared<ConnectSignal>(this->_shu_ptr); */
    /*    st_sig->id = id; */

    /*    for (auto cn : this->_beam_data->fetch<Connect_beam>()) { */
    /*        cn(st_sig); */
    /*    } */
    /*};  // -----  end lambda  ----- */

    /*/1** */
    /* * */
    /* *1/ */

    /*THREAD_FUN(_fun_Signal, SigId::_strategy_id); */
}
}  // namespace e2q
