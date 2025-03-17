/*
 * =====================================================================================
 *
 *       Filename:  Exchange.hpp
 *
 *    Description:  Exchange
 *
 *        Version:  1.0
 *        Created:  2024年01月19日 15时36分42秒
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

#ifndef EXCHANGE_INC
#define EXCHANGE_INC
#include <memory>
#include <stdexcept>
#include <string>

#include "FusionPack/foreign.hpp"
#include "OMSPack/Matcher/BaseMatcher.hpp"

#if __cplusplus >= 201703L

/* Now remove the trow */
#define throw(...)
#include <quickfix/Acceptor.h>
#include <quickfix/FileStore.h>
#include <quickfix/Initiator.h>
#include <quickfix/SocketAcceptor.h>

#include "quickfix/FixFields.h"
#undef throw /* reset */
#endif
namespace e2q {

/*
 * ================================
 *        Class:  Exchange
 *  Description:  模拟网络 请求 account application fix
 * ================================
 */
class Exchange {
public:
    /* =============  LIFECYCLE     =================== */
    Exchange(std::string &e2l); /* constructor */

    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */

    /**
     * 将 fix Risk 两个线程
     */
    void RiskFix();
    /**
     * 虚构交易者，触发 MarketData,link FeedData
     * trigger to FeedBalance
     * 离线数据，1 分钟构成 一天的数据，才比较方便做各种运算
     * 单线程，没有正式 的对手盘，就只能这样来模拟了
     */
    void imagine();

    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */
    void match();
    FIX::SessionSettings ExSetting();
    /* =============  DATA MEMBERS  =================== */
    _Resource_ptr _resource = nullptr;
    std::shared_ptr<BeamData> _beam_data = nullptr;
    std::shared_ptr<Shuttle> _shu_ptr = nullptr;
    std::shared_ptr<ScriptThread> _program = nullptr;

}; /* -----  end of class Exchange  ----- */

}  // namespace e2q
#endif /* ----- #ifndef EXCHANGE_INC  ----- */
