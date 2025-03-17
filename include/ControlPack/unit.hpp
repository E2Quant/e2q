/*
 * =====================================================================================
 *
 *       Filename:  unit.hpp
 *
 *    Description:  unit
 *
 *        Version:  1.0
 *        Created:  2023年09月18日 17时53分33秒
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

#ifndef UNIT_INC
#define UNIT_INC
#include <cstddef>

#include "ControlPack/foreign.hpp"
namespace e2q {

/**
 *  Message Share Stash struct
 */

#define MsgStash(arg)                                         \
    struct arg##Stash : public MessageShare::StashPack<arg> { \
        size_t type{typeid(arg##Stash).hash_code()};          \
    }; /* ----------  end of struct OHLCType  ---------- */   \
    typedef std::shared_ptr<arg##Stash> arg##StashSharePtr;

/**
 * from  disruptor tunnel to ohlc
 */
MsgStash(Disruptor);

/**
 * from mutile master to one slave
 */
MsgStash(ThreadDisruptor);

#define BeamStash(bptr, dpt, master, inc, desc)                                \
    do {                                                                       \
        bridge::StashType<dpt##StashSharePtr, std::size_t> __fun =             \
            [this](std::size_t x) {                                            \
                dpt##StashSharePtr _call_ptr = std::make_shared<dpt##Stash>(); \
                _call_ptr->data_ptr = std::make_shared<dpt>();                 \
                _call_ptr->data_ptr->from(master->data_ptr);                   \
                _call_ptr->data_ptr->name(desc);                               \
                _call_ptr->id = x + 1;                                         \
                this->_master->data_ptr->follows(_call_ptr->data_ptr->me());   \
                return _call_ptr;                                              \
            };                                                                 \
        bptr->stash(__fun, inc);                                               \
    } while (0)

}  // namespace e2q
#endif /* ----- #ifndef UNIT_INC  ----- */
