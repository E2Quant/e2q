/*
 * =====================================================================================
 *
 *       Filename:  PGConnectPool.hpp
 *
 *    Description:  PGConnectPool
 *
 *        Version:  1.0
 *        Created:  2024年10月10日 15时37分26秒
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

#ifndef PGCONNECTPOOL_INC
#define PGCONNECTPOOL_INC
#include <unistd.h>

#include <cstddef>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include "Toolkit/Norm.hpp"
#include "assembler/BaseType.hpp"
#include "libs/DB/pg.hpp"
namespace e2q {

/*
 * ================================
 *        Class:  PGConnectPool
 *  Description:
 * ================================
 */
class PGConnectPool {
public:
    /* =============  LIFECYCLE     =================== */
    PGConnectPool(std::size_t connect, std::string file)
        : _max_connect(connect), _properties(file)
    {
        if (file.length() > 0) {
            Pgsql *ptr = nullptr;
            for (std::size_t m = 0; m < _max_connect; m++) {
                ptr = MALLOC(Pgsql, file);
                std::pair<bool, Pgsql *> pg = std::make_pair(true, ptr);
                _pool.push_back(pg);
            }
        }
    }; /* constructor */
    ~PGConnectPool()
    {
        std::size_t num = 0;
        for (auto it : _pool) {
            while (it.first == false && num < 5) {
                sleep(1);
                num++;
                log::bug("pg is runing...");
#ifdef DEBUG
                for (auto uil : _used_idx_log) {
                    log::bug("not release:", uil.second.first,
                             " code line:", uil.second.second);
                }
#endif
            }
            RELEASE(it.second);
        }
    }
    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */
    bool isInit() { return _pool.size() > 0; }
    std::size_t getId(const char *file = __FILE__, long lineNumber = __LINE__);
    void e2lThread(std::size_t);
    Pgsql *ptr(std::size_t);
    void release(std::size_t);

    void auto_release();
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    std::size_t _max_connect = 5;
    std::string _properties = "";
    std::size_t _used_idx = 0;
    std::vector<std::pair<bool, Pgsql *>> _pool;

#ifdef DEBUG
    // idx -> <code file, code line>
    std::map<std::size_t, std::pair<std::string, long>> _used_idx_log;
#endif

    using Mutex = typename e2q::BasicLock::mutex_type;
    mutable Mutex _PoolMutex;
}; /* -----  end of class PGConnectPool  ----- */

}  // namespace e2q
#endif /* ----- #ifndef PGCONNECTPOOL_INC  ----- */
