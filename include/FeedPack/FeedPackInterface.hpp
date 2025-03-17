/*
 * =====================================================================================
 *
 *       Filename:  FeedPackInterface.hpp
 *
 *    Description:  FeedPackInterface
 *
 *        Version:  1.0
 *        Created:  2023年07月31日 10时56分42秒
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

#ifndef FEEDPACKINTERFACE_INC
#define FEEDPACKINTERFACE_INC
#include "../ControlPack/pack.hpp"
#include "FeedBalance.hpp"
namespace e2q {

/*
 * =====================================================================================
 *        Class:  FeedPackInterface
 *  Description:
 * =====================================================================================
 */
class FeedPackInterface : public PackInterface<FeedBalance> {
public:
    /* ====================  LIFECYCLE =======================================
     */
    FeedPackInterface(); /* constructor */

    /* ====================  ACCESSORS =======================================
     */

    /* ====================  MUTATORS ======================================= */

    /* ====================  OPERATORS =======================================
     */
    /**
     * 接收数据
     */
    const FeedBalance& fetch();
    /**
     * 填入数据
     */
    void embed(const FeedBalance& fb);

    /**
     * triger
     */
    void wait();

protected:
    /* ====================  METHODS ======================================= */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS ======================================= */

    /* ====================  DATA MEMBERS
     * ======================================= */

}; /* -----  end of class FeedPackInterface  ----- */

/* struct fpif_embed : SlotVisitor<void, FeedPackInterface> { */
/*     template <class T> */
/*     void operator()(T& _in) */
/*     { */
/*         std::cout << "d============d" << std::endl; */

/*         FeedBalance fb; */
/*         FeedPackInterface fpif; */
/*         fpif.embed(fb); */
/*         (_in)(fpif); */
/*     } */

/* }; /1* ----------  end of struct fpif_embed  ---------- *1/ */

}  // namespace e2q

#endif /* ----- #ifndef FEEDPACKINTERFACE_INC  ----- */
