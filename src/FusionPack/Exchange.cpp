/*
 * =====================================================================================
 *
 *       Filename:  Exchange.cpp
 *
 *    Description:  Exchange
 *
 *        Version:  1.0
 *        Created:  2024年01月19日 17时35分23秒
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
#include "FusionPack/Exchange.hpp"

#include <memory>
#include <string>

#include "OMSPack/Matcher/TraderAlgorithms.hpp"
#include "OMSPack/SessionGlobal.hpp"
#include "quickfix/Dictionary.h"
#include "quickfix/SessionID.h"

namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Exchange::Exchange
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
Exchange::Exchange(std::string& e2l)
{
    _program = std::make_shared<ScriptThread>(e2lType::_OMS);
    _program->init(e2l.c_str());

    bool call = _program->CheckCall();
    Int_e argc = 1;

    if (call) {
        log::bug("call method is error");
        return;
    }

    _program->toScript(argc, argc);

} /* -----  end of function Exchange::Exchange  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Exchange::RiskFix
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Exchange::RiskFix()
{
    FIN_FABR_IS_NULL();

    if (GlobalMatcher == nullptr) {
        GlobalMatcher = std::make_shared<TraderAlgorithms>();
        GlobalMatcher->InitSequence();
    }
    _resource = std::make_shared<Resource>();
    _beam_data = std::make_shared<BeamData>();
    _shu_ptr = std::make_shared<Shuttle>();

    FixBeam _fixbeam;
    _fixbeam.shareptr(_resource);
    _beam_data->assign<FixBeam, Func_beam>(_fixbeam);

    try {
        e2q::FixApplication application(_program);

        application.toFeedData(_resource, _beam_data);

        FIX::SessionSettings settings = ExSetting();

        FIX::FileStoreFactory storeFactory(settings);

        // #define FIX_DEBUG 1

#ifdef FIX_DEBUG
        FIX::ScreenLogFactory logFactory(settings);
        FIX::SocketAcceptor acceptor(application, storeFactory, settings,
                                     logFactory);

#else
        FIX::SocketAcceptor acceptor(application, storeFactory, settings);

#endif
        acceptor.start();

        /**
         * 提前 启动
         */
        auto fun = [&application]() {
            application.FeedDataHandle();
        };  // -----  end lambda  -----

        THREAD_FUN(fun);

        SignalThread(this, SigId::_strategy_id);

        while (application.end() == false) {
            FIX::process_sleep(2);
        }

        if (acceptor.isStopped() == false) {
            acceptor.stop();
        }
    }
    catch (std::exception& e) {
        log::bug("error:", e.what());
    }

} /* -----  end of function Exchange::RiskFix  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Exchange::match
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Exchange::match()
{
    GlobalMatcher->display();
} /* -----  end of function Exchange::match  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Exchange::ExSetting
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
FIX::SessionSettings Exchange::ExSetting()
{
    FIX::SessionSettings settings(FinFabr->_fix_cfg);

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
    std::string sql =
        "SELECT  beginstring, sendercompid,targetcompid, "
        "filestorepath,datadictionary from fixsession ORDER BY id;";

    bool r = pg->select_sql(sql);
    FIX::Dictionary dict_session;
    dict_session.setDouble("LogonTimeout", 30);
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
                FIX::SessionID session(begin, seder, target);

                settings.set(session, dict_session);
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
    }
    GlobalDBPtr->release(gidx);
    return settings;
} /* -----  end of function Exchange::ExSetting  ----- */
}  // namespace e2q
