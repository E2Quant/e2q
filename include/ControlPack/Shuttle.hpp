/*
 * =====================================================================================
 *
 *       Filename:  Shuttle.hpp
 *
 *    Description:  Shuttle
 *
 *        Version:  1.0
 *        Created:  2023年07月26日 17时46分47秒
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

#ifndef SHUTTLE_INC
#define SHUTTLE_INC

#include <concepts>
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include "ControlPack/Bucket.hpp"
namespace e2q {

template <typename Ret, typename... Args>
using Slot = std::function<Ret(Args...)>;

template <typename Ret, typename... Args>
using SlotVisitor = visitor_base<Slot<Ret, Args...>>;

/*
 * =====================================================================================
 *        Class:  Shuttle
 *  Description:  用作引导不同的类
 * =====================================================================================
 */
class Shuttle : public Bucket {
public:
    /* ====================  LIFECYCLE =======================================
     */
    Shuttle(){}; /* constructor */
    /* ====================  ACCESSORS =======================================
     */

    /* ====================  MUTATORS ======================================= */

    /* ====================  OPERATORS =======================================
     */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  connect
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *   读数据的一端
     * ============================================
     */
    template <typename Ret, typename... Args>
    size_t connect(const Slot<Ret, Args...> &slot)
    {
        size_t index = push_back(slot);
        return index;
    }

    template <typename Ret, typename... Args>
    size_t connect(Slot<Ret, Args...> &&slot)
    {
        size_t index = push_back(std::move(slot));
        return index;
    } /* -----  end of function connect  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  run
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *   数据写入端
     * ============================================
     */

    template <typename T, typename Ret, typename... Args,
              typename = typename std::enable_if<
                  std::is_base_of<SlotVisitor<Ret, Args...>, T>::value>::type>
    void run(T &&visitor)
    {
        visit(std::move(visitor));
    } /* -----  end of function run  ----- */

protected:
    /* ====================  METHODS ======================================= */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS ======================================= */

    /* ====================  DATA MEMBERS
     * ======================================= */
}; /* -----  end of class Shuttle  ----- */

}  // namespace e2q
#endif /* ----- #ifndef SHUTTLE_INC  ----- */
