/*
 * =====================================================================================
 *
 *       Filename:  STLog.cpp
 *
 *    Description:  STLog
 *
 *        Version:  1.0
 *        Created:  2024年09月09日 11时46分38秒
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
#include "TradePack/STLog.hpp"

#include <cstddef>
#include <string>

#include "E2LScript/ExternClazz.hpp"

namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  STLog::TicketComment
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void STLog::TicketComment(std::size_t quantid, std::size_t ticket, int side,
                          int oe)
{
    std::size_t idx = GlobalDBPtr->getId();
    Pgsql *gsql = GlobalDBPtr->ptr(idx);
    if (gsql != nullptr) {
        std::string table = "public.";
        gsql->public_table(table);
        gsql->insert_table("analselog");
        gsql->insert_field("quantid", quantid);
        gsql->insert_field("key", ticket);

        gsql->insert_field("values", side);
        gsql->insert_field("type", oe);

        gsql->insert_field("ctime", e2q::ticket_now);
        gsql->insert_commit();
    }

    GlobalDBPtr->release(idx);

} /* -----  end of function STLog::TicketComment  ----- */
}  // namespace e2q
