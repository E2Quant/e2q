/*
 * =====================================================================================
 *
 *       Filename:  pg.cpp
 *
 *    Description:  pg
 *
 *        Version:  1.0
 *        Created:  2024年05月31日 15时03分11秒
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

#include "libs/DB/pg.hpp"

#include <cstdio>
#include <string>

#include "postgresql/libpq-fe.h"
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::Pgsql
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
Pgsql::Pgsql(std::string file)
{
    _properties = file;
    if (_properties.length() == 0) {
        log::bug("properties is empty");
        return;
    }
    init();
} /* -----  end of function Pgsql::Pgsql  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::init
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Pgsql::init()
{
    const char *_conninfo = _properties.c_str();

    _conn = PQconnectdb(_conninfo);
    /* Check to see that the backend connection was successfully made */
    if (_conn == nullptr || PQstatus(_conn) != CONNECTION_OK) {
        fprintf(stderr, "%s", PQerrorMessage(_conn));
        exit_nicely();
    }

} /* -----  end of function Pgsql::init  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::begin
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Pgsql::begin()
{
    _begin = 0;
    _end = 0;
} /* -----  end of function Pgsql::begin  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::next
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Pgsql::next()
{
    _end++;
    if (_end == _nfields) {
        _end = 0;
        _begin++;
    }
} /* -----  end of function Pgsql::next  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::end
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool Pgsql::end()
{
    if (_begin == _ntuples) {
        return false;
    }

    return true;
} /* -----  end of function Pgsql::end  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::PGResult
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int Pgsql::PGResult(char **field, char **val)
{
    int ret = -1;
    if (_res == nullptr) {
        log::echo("res is null");
        return ret;
    }

    *field = PQfname(_res, _end);
    *val = PQgetvalue(_res, _begin, _end);

    return _begin;
} /* -----  end of function Pgsql::PGResult  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::row
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool Pgsql::row()
{
    bool ret = false;
    if (_idx < _begin) {
        _idx = _begin;
        ret = true;
    }
    return ret;
} /* -----  end of function Pgsql::row  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::First
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Pgsql::OneHead(char **field, char **val)
{
    if (_res == nullptr || _nfields == 0) {
        log::echo("res is null");
        return;
    }

    *field = PQfname(_res, 0);
    *val = PQgetvalue(_res, 0, 0);

} /* -----  end of function Pgsql::OneHead  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::pversion
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Pgsql::pversion()
{
    if (_conn == nullptr) {
        return;
    }
    /* Check to see that the backend connection was successfully made */
    if (PQstatus(_conn) != CONNECTION_OK) {
        fprintf(stderr, "%s", PQerrorMessage(_conn));
        exit_nicely();
    }
} /* -----  end of function Pgsql::pversion  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::insert_return
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Pgsql::insert_return(std::string field)
{
    _insert_return = " RETURNING " + field;
} /* -----  end of function Pgsql::insert_return  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::delete_commit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool Pgsql::delete_commit(std::string sql)
{
    std::string _sql = sql + ";";

    bool r = exec(_sql);
    if (r == false) {
        _command_count = 0;
    }
    return r;
} /* -----  end of function Pgsql::delete_commit  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::select_sql
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool Pgsql::select_sql(std::string sql)
{
    _idx = 0;
    std::string _sql = sql + ";";
    bool r = exec(_sql);
    if (r == false) {
        _nfields = 0;
        _ntuples = 0;

        _begin = 0;
        _end = 0;
    }
    return r;
} /* -----  end of function Pgsql::select_sql  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::exec
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool Pgsql::exec(std::string sql)
{
    bool ret = false;
    if (_conn == nullptr) {
        log::bug("conn is nullptr");
        init();
    }
    if (debug) {
        log::info(sql);
    }
    if (_res != nullptr) {
        PQclear(_res);
    }

    _res = PQexec(_conn, sql.c_str());

    ExecStatusType est = PQresultStatus(_res);

    switch (est) {
        case PGRES_EMPTY_QUERY:
            log::echo("empty query");

            break;
        case PGRES_COMMAND_OK: {
            // update delete  insert
            // int cmds = PQcmdTuples(_res);

            _command_count = std::stoi(PQcmdTuples(_res));
            ret = true;
            break;
        }
        case PGRES_TUPLES_OK:
            // select
            _nfields = PQnfields(_res);

            _ntuples = PQntuples(_res);

            ret = true;
            break;
        case PGRES_COPY_OUT:
            break;
        case PGRES_COPY_IN:
            break;
        case PGRES_BAD_RESPONSE:
        case PGRES_NONFATAL_ERROR:
        case PGRES_FATAL_ERROR:
        default:
            fprintf(stderr, "exec : %s", PQerrorMessage(_conn));
            log::bug(sql);
            PQclear(_res);
            _res = nullptr;
            break;
    }

    _public = "public.";
    return ret;
} /* -----  end of function Pgsql::exec  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Pgsql::exit_nicely
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Pgsql::exit_nicely()
{
    if (_conn) {
        PQfinish(_conn);
        _conn = nullptr;
    }

} /* -----  end of function Pgsql::exit_nicely  ----- */
}  // namespace e2q
