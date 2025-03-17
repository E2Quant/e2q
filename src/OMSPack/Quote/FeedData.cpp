/*
 * =====================================================================================
 *
 *       Filename:  FeedData.cpp
 *
 *    Description:  FeedData
 *
 *        Version:  1.0
 *        Created:  2024年01月22日 18时04分28秒
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
#include "OMSPack/Quote/FeedData.hpp"

#include <cstddef>
#include <cstdio>

#include "E2L/E2LType.hpp"
#include "Toolkit/Norm.hpp"
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FeedData::resource
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FeedData::resource(_Resource_ptr ptr)
{
    if (ptr == nullptr) {
        log::bug("resource is null");
        return;
    }
    _resource = std::move(ptr);

    _master =
        _resource->ResourcePtr<DisruptorStash, Disruptor>(trading_protocols);
    _master->id = 1;
    _master->data_ptr->name("FeedData master [ kafka -> disruptor ]");
} /* -----  end of function FeedData::resource  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FeedData::callback
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FeedData::callback(std::shared_ptr<ConnectSignal> beam)
{
    if (beam == nullptr || beam->id != SigId::_strategy_id) {
        log::bug("callback error");
        return;
    }
    size_t inc = 1;
    BeamStash(beam, Disruptor, this->_master, inc, "FeedData slave");

} /* -----  end of function FeedData::callback  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FeedData::ctrl
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FeedData::ctrl(func_type_ret<SeqType, SeqType, SeqType, SeqType> fix_call)
{
    if (_master == nullptr) {
        log::bug("_master is nullptr");
        return;
    }

    auto ctrl_fun = [this,
                     &fix_call](std::array<SeqType, trading_protocols> &data) {
        this->_master->data_ptr->wait_next();

        SeqType sym_id = data[e2q::Trading::t_stock];

        if (fix_call != nullptr && sym_id > 0) {
            // 计算 复权的
            SeqType price = data[e2q::Trading::t_price];
            SeqType now = data[e2q::Trading::t_time];

            SeqType ret = fix_call(sym_id, now, price);

            if (ret > 0) {
                data[Trading::t_adjprice] = ret;
            }
        }
        this->_master->data_ptr->deposit(data);
    };  // -----  end lambda  -----

    auto check_master_slave = [this]() {
        this->_master->data_ptr->wait_slave(FinFabr->_offer_time);
    };  // -----  end lambda  -----

    THREAD_FUN(check_master_slave);

    if (FinFabr->_csv_kafka == e2::MKType::mk_csv) {
        /**
         * 请求的和系统内的是不是一致
         */
        std::string dir = FinFabr->_source;

        Tunnel tunnel(dir);

        tunnel.handle(ctrl_fun);
    }
    else if (FinFabr->_csv_kafka == e2::MKType::mk_kafka) {
        KafkaFeed kf(FinFabr->_source, FinFabr->_topic);
        kf.handle(ctrl_fun);
    }
    else {
        log::bug("csv_kafka error");
    }

    this->_master->data_ptr->quit();

} /* -----  end of function FeedData::ctrl  ----- */

}  // namespace e2q
