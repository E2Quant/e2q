/*
 * =====================================================================================
 *
 *       Filename:  Fusion.hpp
 *
 *    Description:  Fusion
 *
 *        Version:  1.0
 *        Created:  2023年09月19日 10时40分34秒
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

#ifndef FUSION_INC
#define FUSION_INC
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "FusionPack/Exchange.hpp"
#include "FusionPack/foreign.hpp"
namespace e2q {

/*
 * ================================
 *        Class:  Fusion
 *  Description:
 * ================================
 */
class Fusion {
public:
    /* =============  LIFECYCLE     =================== */
    Fusion(); /* constructor */

    /* =============  ACCESSORS     =================== */
    void oms(std::string e2l_script, int);
    void ea(std::string e2l_script, size_t n, std::size_t quantId_start,
            std::size_t tpro);

    void properties(std::string &);
    void enter(const char *);
    /* =============  MUTATORS      =================== */
    void next();
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */
    /* =============  DATA MEMBERS  =================== */
    std::string _Properties = "";

    std::shared_ptr<BeamData> _beam_data = nullptr;

    _Resource_ptr _resource = nullptr;

    std::vector<std::string> scripts;

    std::shared_ptr<Shuttle> _shu_ptr = nullptr;
}; /* -----  end of class Fusion  ----- */

}  // namespace e2q
#endif /* ----- #ifndef FUSION_INC  ----- */
