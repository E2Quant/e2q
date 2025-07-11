/*
 * =====================================================================================
 *
 *       Filename:  MachineOS.cpp
 *
 *    Description:  MachineOS
 *
 *        Version:  1.0
 *        Created:  2024年02月01日 15时29分50秒
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
#include "FusionPack/MachineOS.hpp"

#include <unistd.h>

#include <cstddef>
#include <memory>
#include <string>

#include "E2LScript/ExternClazz.hpp"
#include "Toolkit/GlobalConfig.hpp"
#include "quickfix/SessionSettings.h"
#include "utility/Log.hpp"

namespace e2q {
/**
 * fix account extern for el2 script use
 */
e2q::FixAccount fix_application;

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  MachineOS::MachineOS
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
MachineOS::MachineOS()
{
    _resource = std::make_shared<Resource>();
    _beam_data = std::make_shared<BeamData>();
    _shu_ptr = std::make_shared<Shuttle>();
} /* -----  end of function MachineOS::MachineOS  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  MachineOS::enter
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void MachineOS::enter(std::string& e2l_script, std::string& edir, size_t n,
                      std::size_t quantId_start, std::size_t total_process)
{
    _node = n;
    GlobalProcessId = n;
    if (GlobalDBPtr == nullptr) {
        log::info("db init nullptr");
        return;
    }
    sleep(1);

    /**
     * 1. Strategy and Feed == machine os
     */

    /**
     * strategy thread
     */
    StrategyBase _sbase(this->_resource, this->_beam_data, quantId_start);
    _sbase.ProgramInit(e2l_script, edir);

    FIX_PTR_IS_NULL();

    /**
     * 2. start fix node
     */

    try {
        FIX::SessionSettings settings = EaSetting();
        /**
         * 动态设置 SenderCompID
         */

        FIX::FileStoreFactory storeFactory(settings);

        // #define APP_DEBUG 1

#ifdef APP_DEBUG
        FIX::ScreenLogFactory logFactory(settings);
        FIX::ThreadedSocketInitiator _initiator(fix_application, storeFactory,
                                                settings, logFactory);

#else

        FIX::ThreadedSocketInitiator _initiator(fix_application, storeFactory,
                                                settings);
#endif

        std::size_t bug_client = 0;
        do {
            log::info("initiator start, e2l path:", e2l_script);
            if (_initiator.isStopped() == false) {
                _initiator.stop();
            }
            settings = EaSetting();

            if (bug_client == 5) {
                break;
            }
            _initiator.start();
            sleep(1);
            bug_client++;
        } while (_initiator.isLoggedOn() ==
                 false); /* -----  end do-while  ----- */
        if (bug_client < 5) {
            log::info("node start ok!");
        }

        if (_initiator.isLoggedOn()) {
            auto fun = [this, &_sbase]() {
                this->ctrl();

                _sbase.runScript();
                fix_application.quit();
            };  // -----  end lambda  -----

            THREAD_FUN(fun);

            fix_application.wait();

            if (_initiator.isStopped() == false) {
                _initiator.stop();
            }
        }
        else {
            log::bug("fix not login");
        }
    }
    catch (std::exception& e) {
        log::bug(e.what());
        return;
    }

} /* -----  end of function MachineOS::enter  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  MachineOS::ctrl
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void MachineOS::ctrl()
{
    /**
     * 1. run func_beam for  ohlc or tick or ...
     */

    _resource->StashPtr<ContainerStash, Container>();

    FeedBalance fb(_shu_ptr, _resource);
    fb.CtrlPack(_beam_data);

    /**
     * end
     */

    /**
     * 2. 准备接收数据
     */

    fix_application.Init(_resource, _beam_data, _shu_ptr);
    /**
     * 3. signal
     */
    machine();
    /**
     * 4. 请求数据
     */
    if (FixPtr->_symbols.size() > 0) {
        fix_application.QuoteRequest(FixPtr->_symbols);
    }

} /* -----  end of function MachineOS::ctrl  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  MachineOS::machine
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void MachineOS::machine()
{
    SignalThread(this, SigId::_feedId);

} /* -----  end of function MachineOS::machine  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  MachineOS::EaSetting
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
FIX::SessionSettings MachineOS::EaSetting()
{
    FIX::SessionSettings settings(FixPtr->_fix_cfg);

    char* field = nullptr;
    char* val = nullptr;

    bool isDb = GlobalDBPtr->isInit();
    if (isDb == false) {
        return settings;
    }
    std::size_t gidx = e2q::GlobalDBPtr->getId();

    Pgsql* pg = GlobalDBPtr->ptr(gidx);
    if (pg == nullptr) {
        e2q::GlobalDBPtr->release(gidx);
        return settings;
    }

    std::string sql = log::format(
        "SELECT beginstring, targetcompid, sendercompid,"
        "filestorepath,datadictionary,host as SocketConnectHost,port as "
        "SocketConnectPort from fixsession WHERE id not IN (SELECT sessionid "
        "from account)  ORDER BY id DESC OFFSET %ld "
        "LIMIT 1",
        _node);

    // log::info(sql);
    bool r = pg->select_sql(sql);
    FIX::Dictionary dict_session;
    dict_session.setDouble("LogonTimeout", 30);
    dict_session.setInt("HeartBtInt", 30);
    dict_session.setBool("ResetOnLogon", true);
    dict_session.setBool("ResetOnDisconnect", false);
    dict_session.setBool("SendResetSeqNumFlag", true);
    // int idx = 0;
    int col = 0;
    std::string begin;
    std::string seder;
    std::string target;
    if (r) {
        for (pg->begin(); pg->end(); pg->next()) {
            int m = pg->PGResult(&field, &val);
            if (m == -1) {
                break;
            }

            if (pg->row()) {
                // idx++;
                col = 0;
            }
            if (field != nullptr) {
                if (col == 0) {
                    begin = std::string(val);
                }
                else if (col == 1) {
                    seder = std::string(val);
                }
                else if (col == 2) {
                    target = std::string(val);
                }
                else {
                    dict_session.setString(std::string(field),
                                           std::string(val));
                }
                col++;
            }
        }
        FIX::SessionID session(begin, seder, target);

        settings.set(session, dict_session);
    }
    else {
        log::bug(sql);
    }
    GlobalDBPtr->release(gidx);

    return settings;
} /* -----  end of function MachineOS::EaSetting  ----- */
}  // namespace e2q

