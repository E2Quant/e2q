/*
 * =====================================================================================
 *
 *       Filename:  Tunnel.hpp
 *
 *    Description:  Tunnel
 *
 *        Version:  1.0
 *        Created:  2022年08月09日 16时07分20秒
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

#ifndef TUNNEL_INC
#define TUNNEL_INC
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "OMSPack/foreign.hpp"
namespace e2q {

/*
 * =====================================================================================
 *        Class:  Tunnel
 *  Description:
 * =====================================================================================
 */
class Tunnel {
public:
    /* ====================  LIFECYCLE =======================================
     */
    Tunnel(){};
    Tunnel(std::string& dir) : _dir(dir)
    {
        if (_dir.length() == 0) {
            elog::bug("dir or symbol is empty");
        }
    };

    /* constructor */
    /* ====================  ACCESSORS =======================================
     */

    void handle(TradType tt);
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

    /* ====================  DATA MEMBERS
     * ======================================= */
    // std::vector<std::size_t> _symbol;

    std::string _dir;
    std::string _line;

}; /* -----  end of class Tunnel  ----- */
}  // namespace e2q
#endif /* ----- #ifndef TUNNEL_INC  ----- */
