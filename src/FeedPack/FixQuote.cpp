/*
 * =====================================================================================
 *
 *       Filename:  FixQuote.cpp
 *
 *    Description:  FixQuote
 *
 *        Version:  1.0
 *        Created:  2024年02月03日 12时10分39秒
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

#include "FeedPack/FixQuote.hpp"
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixQuote::callback
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixQuote::callback(std::shared_ptr<ConnectSignal> beam)
{
    if (beam == nullptr || beam->id != SigId::_feedId) {
        log::bug("ConnectSignal is null");

        return;
    }
    if (_master == nullptr || _master->data_ptr == nullptr) {
        log::bug("master is null");
        return;
    }
    _master->id = _id;
    _master->data_ptr->name("FixQuote master [fixaccout -> disruptor]");

    size_t inc = 1;

    BeamStash(beam, Disruptor, this->_master, inc, "slave");

} /* -----  end of function FixQuote::callback  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixQuote::resource
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  从全局内存管理中，申请 ohlc 内存入口
 * ============================================
 */
void FixQuote::resource(_Resource_ptr ptr, std::size_t sleep_time)
{
    _rptr = ptr;
    if (_rptr == nullptr) {
        log::bug("resource is null");
        return;
    }
    _master = _rptr->ResourcePtr<DisruptorStash, Disruptor>(trading_protocols);

    auto check_master_slave = [this](std::size_t sleep_time) {
        this->_master->data_ptr->wait_slave(sleep_time);
    };  // -----  end lambda  -----

    THREAD_FUN(check_master_slave, sleep_time);
} /* -----  end of function FixQuote::resource  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixQuote::handle
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   tunnel tick to ohlc or save tick
 * ============================================
 */
void FixQuote::handle(std::array<SeqType, trading_protocols> &data)
{
    if (_master == nullptr || _master->data_ptr == nullptr) {
        log::bug("master is nullptr!");
        return;
    }
    _master->data_ptr->wait_next();
    int ret = _master->data_ptr->deposit(data);
    if (ret == -1) {
        log::echo("deposit ret:", ret);
    }
} /* -----  end of function Fixudo uote::handle  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  FixQuote::quit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void FixQuote::quit()
{
    if (_master != nullptr && _master->data_ptr != nullptr) {
        _master->data_ptr->quit();
    }
} /* -----  end of function FixQuote::quit  ----- */
}  // namespace e2q
