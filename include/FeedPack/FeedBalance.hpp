/*
 * =====================================================================================
 *
 *       Filename:  FeedBalance.hpp
 *
 *    Description: FeedBalance
 *
 *        Version:  1.0
 *        Created:  2022年07月28日 11时54分16秒
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

#ifndef FEEDBALANCE_INC
#define FEEDBALANCE_INC
#include <cstddef>
#include <memory>

#include "E2LScript/ExternClazz.hpp"
#include "FeedPack/OHLC.hpp"
#include "FeedPack/foreign.hpp"
#include "PlunkPack/Analyzers.hpp"
namespace e2q {

/*
 * =====================================================================================
 *        Class:  FeedBalance
 *  Description:
 *  审核 FeedData Container ，以决定是否启用，需要有API
 *   给上层的，比如 WEB UI 来操作
 *   可接入不同的 feedData
 * =====================================================================================
 */
class FeedBalance {
public:
    /* ====================  LIFECYCLE =======================================
     */
    FeedBalance(std::shared_ptr<Shuttle> cs, _Resource_ptr ptr)
        : _resource(ptr), _shu_ptr(cs){}; /* constructor */

    /* ====================  ACCESSORS =======================================
     */
    //    void resource(_Resource_ptr ptr) { _resource = std::move(ptr); }

    void CtrlPack(std::shared_ptr<BeamData>);
    /* ====================  MUTATORS =======================================
     */

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

    /**
     * 从 e2l scripte 决定采用 ohlc or tick or ....
     */
    void obtain();

    /* ====================  DATA MEMBERS
     * ======================================= */
    std::shared_ptr<BeamData> _data_ptr = nullptr;
    _Resource_ptr _resource = nullptr;

    std::shared_ptr<Shuttle> _shu_ptr = nullptr;

    // std::shared_ptr<FuncSignal> _Container_signal = nullptr;

}; /* -----  end of class FeedBalance  ----- */

}  // namespace e2q
#endif /* ----- #ifndef FEEDBALANCE_INC  ----- */

