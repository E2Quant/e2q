/*
 * =====================================================================================
 *
 *       Filename:  FeedData.hpp
 *
 *    Description: FeedData
 *
 *        Version:  1.0
 *        Created:  2022年07月28日 11时47分57秒
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

#ifndef FEEDDATA_INC
#define FEEDDATA_INC

#include <cstddef>
#include <istream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "E2LScript/ExternClazz.hpp"
#include "OMSPack/Quote/Tunnel.hpp"
#include "Toolkit/Norm.hpp"
#include "libs/kafka/Consumer.hpp"
#if __cplusplus >= 201703L

/* Now remove the trow */
#define throw(...)
#include <quickfix/FixFields.h>

#undef throw /* reset */
#endif
namespace e2q {

/*
 * =====================================================================================
 *        Class:  FeedData
 *  Description:  只管理 tunnel 基类的信息
 * =====================================================================================
 */
class FeedData : public ConnectBeamClass<ConnectSignal> {
public:
    /* ====================  LIFECYCLE =======================================
     */
    FeedData() {

    }; /* constructor */
    /* ====================  ACCESSORS =======================================
     */

    /* ====================  MUTATORS =======================================
     */
    void resource(_Resource_ptr ptr);
    void callback(std::shared_ptr<ConnectSignal> beam);

    /**
     * 控制，有可能不选择这个 feeddata 之类的
     */
    void ctrl(
        func_type_ret<SeqType, SeqType, SeqType, SeqType, SeqType> fix_call);
    /* ====================  OPERATORS
     * =======================================
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

    _Resource_ptr _resource = nullptr;
    std::shared_ptr<BeamData> _data_ptr = nullptr;

    DisruptorStashSharePtr _master = nullptr;

}; /* -----  end of class FeedData  ----- */

}  // namespace e2q
#endif /* ----- #ifndef FEEDDATA_INC  ----- */

