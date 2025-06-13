/*
 * =====================================================================================
 *
 *       Filename:  pg.hpp
 *
 *    Description:  pg
 *
 *        Version:  1.0
 *        Created:  2024年05月31日 14时52分17秒
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

#ifndef PG_INC
#define PG_INC

#include <cstdint>
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// define something for Windows (32-bit and 64-bit, this part is common)
#ifdef _WIN64
// define something for Windows (64-bit only)
#else
// define something for Windows (32-bit only)
#endif
#elif __APPLE__
#include <TargetConditionals.h>
#include <libpq-fe.h>

#if TARGET_IPHONE_SIMULATOR
// iOS, tvOS, or watchOS Simulator
#elif TARGET_OS_MACCATALYST
// Mac's Catalyst (ports iOS API into Mac, like UIKit).
#elif TARGET_OS_IPHONE
// iOS, tvOS, or watchOS device
#elif TARGET_OS_MAC
// Other kinds of Apple platforms
#else
#error "Unknown Apple platform"
#endif
#elif __ANDROID__
// Below __linux__ check should be enough to handle Android,
// but something may be unique to Android.
#elif __linux__
#include <postgresql/libpq-fe.h>

// linux
#elif __unix__  // all unices not caught above
#include <postgresql/libpq-fe.h>
// Unix
#elif defined(_POSIX_VERSION)
// POSIX
#else
#error "Unknown compiler"
#endif

#include <cstddef>
#include <string>

#include "Toolkit/Log.hpp"
namespace e2q {

#define TypeIdName(t) *typeid(t).name()

#define DefTypeId(name, ts, fmt)                         \
    do {                                                 \
        inline const char name##_s = *typeid(ts).name(); \
        inline const char* name##_fmt = fmt;             \
    } while (0)

namespace TIN {
inline const char _signed = TypeIdName(signed char);
inline const char* _signed_fmt = "%d";

inline const char _short = TypeIdName(short);
inline const char* _short_fmt = "%d";

inline const char _int = TypeIdName(int);
inline const char* _int_fmt = "%d";

inline const char _long = TypeIdName(long);
inline const char* _long_fmt = "%lld";

inline const char _long_long = TypeIdName(long long);
inline const char* _longl_fmt = "%lld";

inline const char _uint16_t = TypeIdName(std::uint16_t);
inline const char* _uint16_fmt = "%ld";

inline const char _uint32_t = TypeIdName(std::uint32_t);
inline const char* _uint32_fmt = "%ld";

inline const char _uint64_t = TypeIdName(std::uint64_t);
inline const char* _uint64_fmt = "%lld";

inline const char _double = TypeIdName(double);
inline const char* _double_fmt = "%f";
inline const char* _double_fmts = "%.*f";

inline const char _doublel = TypeIdName(long double);
inline const char* _doublel_fmt = "%lf";
inline const char* _doublel_fmts = "%.*lf";

inline const char _float = TypeIdName(float);
inline const char* _float_fmt = "%f";
inline const char* _float_fmts = "%.*f";

inline const char _char_ptr = TypeIdName(const char*);
inline const char* _char_fmt = "'%s'";

inline const char _size_t = TypeIdName(std::size_t);
inline const char* _size_fmt = "%d";

}  // namespace TIN
/*
 * ================================
 *        Class:  Pgsql
 *  Description:
 * ================================
 */

class Pgsql {
public:
    /* =============  LIFECYCLE     =================== */
    Pgsql(std::string); /* constructor */
    ~Pgsql() { exit_nicely(); }
    /* =============  ACCESSORS     =================== */
    void init();
    int PGResult(char** field, char** val);
    void begin();
    bool end();
    void next();

    void OneHead(char** field, char** val);
    /* =============  MUTATORS      =================== */
    void pversion();
    void public_table(std::string& table) { _public = table; }
    void insert_table(std::string table)
    {
        _insert_sql += _public + table + "(";
    }
    template <typename T>
    void insert_field(std::string field, T t, std::size_t deci = 0)
    {
        std::string val = fmt(t, deci);
        insert_query(field, val);
    }
    void insert_query(std::string field, std::string val)

    {
        if (val.length() == 0) {
            log::bug(" field: ", field);

            return;
        }

        if (_insert_field.length() > 0) {
            _insert_field += ",";
            _insert_val += ",";
        }
        _insert_field += "\"" + field + "\"";

        _insert_val += val;
    }
    void insert_return(std::string field);

    bool insert_commit()
    {
        _insert_sql += _insert_field + ") VALUES (" + _insert_val + ") " +
                       _insert_return + ";";

        return insert_sql(_insert_sql);
    }

    bool insert_sql(std::string sql)
    {
        _nfields = 0;
        _ntuples = 0;

        bool ret = exec(sql);

        _insert_sql = "INSERT INTO ";
        _insert_field = "";
        _insert_val = "";
        _insert_return = "";
        if (ret == false) {
            _nfields = 0;
            _ntuples = 0;

            _begin = 0;
            _end = 0;
            _command_count = 0;
        }
        return ret;
    }
    void pgbegin() { PQexec(_conn, "BEGIN"); }
    void pgcommit() { PQexec(_conn, "COMMIT"); }

    void update_table(std::string table) { _update_sql += table + " SET "; }

    template <typename T>
    void update_field(std::string field, T t, std::size_t deci = 0)
    {
        std::string val = fmt(t, deci);
        if (val.length() == 0) {
            log::bug(" field: ", field);

            return;
        }
        std::string muti = "";
        if (_update_set.length() > 0) {
            muti = ",";
        }
        _update_set += muti + "\"" + field + "\"=" + val;
    }

    template <typename T>
    void update_condition(std::string field, T t, std::size_t deci = 0)
    {
        std::string val = fmt(t, deci);
        if (val.length() == 0) {
            log::bug(" field: ", field);
            return;
        }
        std::string muti = "";
        if (_update_condition.length() > 0) {
            muti = " AND ";
        }
        _update_condition += muti + "\"" + field + "\"=" + val;
    }

    void update_condition(std::string field, std::string rhs)
    {
        if (rhs.length() == 0) {
            log::bug(" field: ", field);
            return;
        }
        std::string muti = "";
        if (_update_condition.length() > 0) {
            muti = " AND ";
        }
        _update_condition += muti + "\"" + field + "\"=" + rhs;
    }

    void update_condition(std::string rhs)
    {
        if (rhs.length() == 0) {
            log::bug(" field: ", rhs);
            return;
        }
        std::string muti = "";
        if (_update_condition.length() > 0) {
            muti = " AND ";
        }
        _update_condition += muti + rhs;
    }

    void update_sql(std::string& sql) { _update_sql = sql; }

    bool update_commit()
    {
        if (_update_set.length() > 0) {
            _update_sql += _update_set;
        }

        if (_update_condition.length() > 0) {
            _update_sql += " WHERE " + _update_condition;
        }
        _update_sql += ";";

        bool ret = exec(_update_sql);
        if (ret == false) {
            _command_count = 0;
        }
        _update_sql = "UPDATE  ";
        _update_set = "";
        _update_condition = "";

        return ret;
    }
    bool delete_commit(std::string);

    int command_count() { return _command_count; }

    bool select_sql(std::string sql);
    bool row();
    std::size_t col();
    int field_size() { return _nfields; }
    int tuple_size() { return _ntuples; }

    void Debug() { debug = true; }
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */
    void exit_nicely();
    bool exec(std::string);
    template <typename T>
    std::string fmt(T t, std::size_t deci = 0)
    {
        const char Ttype = TypeIdName(t);

        std::string fmt_val = "";
        if (Ttype == TIN::_signed) {
            fmt_val += log::format(TIN::_signed_fmt, t);
        }
        else if (Ttype == TIN::_uint16_t) {
            fmt_val += log::format(TIN::_uint16_fmt, t);
        }
        else if (Ttype == TIN::_uint32_t) {
            fmt_val += log::format(TIN::_uint32_fmt, t);
        }
        else if (Ttype == TIN::_uint64_t) {
            fmt_val += log::format(TIN::_uint64_fmt, t);
        }
        else if (Ttype == TIN::_short) {
            fmt_val += log::format(TIN::_short_fmt, t);
        }
        else if (Ttype == TIN::_int) {
            fmt_val += log::format(TIN::_int_fmt, t);
        }

        else if (Ttype == TIN::_size_t) {
            fmt_val += log::format(TIN::_size_fmt, t);
        }

        else if (Ttype == TIN::_long) {
            fmt_val += log::format(TIN::_long_fmt, t);
        }
        else if (Ttype == TIN::_long_long) {
            fmt_val += log::format(TIN::_longl_fmt, t);
        }

        else if (Ttype == TIN::_double) {
            if (deci == 0) {
                fmt_val += log::format(TIN::_double_fmt, t);
            }
            else {
                fmt_val += log::format(TIN::_double_fmts, t, deci);
            }
        }

        else if (Ttype == TIN::_doublel) {
            if (deci == 0) {
                fmt_val += log::format(TIN::_doublel_fmt, t);
            }
            else {
                fmt_val += log::format(TIN::_doublel_fmts, t, deci);
            }
        }
        else if (Ttype == TIN::_float) {
            if (deci == 0) {
                fmt_val += log::format(TIN::_float_fmt, t);
            }
            else {
                fmt_val += log::format(TIN::_float_fmts, t, deci);
            }
        }
        else if (Ttype == TIN::_char_ptr) {
            fmt_val += log::format(TIN::_char_fmt, t);
        }

        else {
            fmt_val += t;
            // log::bug("not type, val:", t, "  Ttype:", Ttype);
        }
        return fmt_val;
    }
    /* =============  DATA MEMBERS  =================== */

    std::string _insert_sql = "INSERT INTO ";
    std::string _insert_field = "";
    std::string _insert_val = "";
    std::string _insert_return = "";

    std::string _update_sql = "UPDATE  ";
    std::string _update_set = "";
    std::string _update_condition = "";

    std::string _public = "public.";

    PGconn* _conn = nullptr;
    PGresult* _res = nullptr;

    int _command_count = 0;

    int _nfields;
    int _ntuples;

    int _idx = 0;
    int _begin = 0;
    int _end = 0;

    bool debug = false;
    std::string _properties;
}; /* -----  end of class Pgsql  ----- */

}  // namespace e2q
#endif /* ----- #ifndef PG_INC  ----- */
