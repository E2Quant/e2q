/*
 * =====================================================================================
 *
 *       Filename:  PGConnectPool.c
 *
 *    Description:  PGConnectPool
 *
 *        Version:  1.0
 *        Created:  2024年10月10日 16时22分00秒
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
#include "libs/DB/PGConnectPool.hpp"

#include <cstddef>

#include "utility/Log.hpp"
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  PGConnectPool::getId
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::size_t PGConnectPool::getId()
{
    BasicLock _lock(_PoolMutex);

    std::size_t idx = 0;
    std::size_t number = _pool.size();
    for (idx = 0; idx < number; idx++) {
        if (_pool.at(idx).first) {
            _pool.at(idx).first = false;
            break;
        }
    }
    if (idx == number) {
        Pgsql* ptr = MALLOC(Pgsql, _properties);
        std::pair<bool, Pgsql*> pg = std::make_pair(false, ptr);

        _pool.push_back(pg);
    }
    if (idx > _used_idx) {
        _used_idx = idx;
    }
    return idx;
} /* -----  end of function PGConnectPool::getId  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  PGConnectPool::e2lThread
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void PGConnectPool::e2lThread(std::size_t num)
{
    for (; _max_connect < num; _max_connect++) {
        Pgsql* ptr = MALLOC(Pgsql, _properties);
        std::pair<bool, Pgsql*> pg = std::make_pair(true, ptr);

        _pool.push_back(pg);
    }
} /* -----  end of function PGConnectPool::e2lThread  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  PGConnectPool::ptr
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
Pgsql* PGConnectPool::ptr(std::size_t idx)
{
    if (idx >= _pool.size()) {
        log::bug("error idx:", idx, " pool size:", _pool.size());
        return nullptr;
    }
    return _pool.at(idx).second;
} /* -----  end of function PGConnectPool::ptr  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  PGConnectPool::release
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void PGConnectPool::release(std::size_t idx)
{
    BasicLock _lock(_PoolMutex);
    if (idx >= _pool.size()) {
        log::bug("error idx:", idx, " pool size:", _pool.size());
        return;
    }
    _pool.at(idx).first = true;
} /* -----  end of function PGConnectPool::release  ----- */
}  // namespace e2q
