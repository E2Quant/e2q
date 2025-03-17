/*
 * =====================================================================================
 *
 *       Filename:  BigNumber.hpp
 *
 *    Description:  BigNumber
 *
 *        Version:  1.0
 *        Created:  2023年08月21日 13时55分57秒
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

#ifndef BIGNUMBER_INC
#define BIGNUMBER_INC
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <regex>
#include <string>

#include "Toolkit/pack.hpp"
namespace e2q {
#define ISNUMBER(a)                          \
    ({                                       \
        bool __ret = false;                  \
        do {                                 \
            std::regex nu("^-?\\d+$");       \
            __ret = std::regex_match(a, nu); \
        } while (0);                         \
        __ret;                               \
    })

#define ISFLOAT(a)                                                  \
    ({                                                              \
        bool __ret = false;                                         \
        do {                                                        \
            std::regex nu("^[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)$"); \
            __ret = std::regex_match(a, nu);                        \
        } while (0);                                                \
        __ret;                                                      \
    })

/*
 * =====================================================================================
 *        Class:  BigNumber
 *  Description:
 * =====================================================================================
 */
class BigNumber {
public:
    /* ====================  LIFECYCLE
     * =======================================
     */
    BigNumber() = default; /* constructor */
    BigNumber(const BigNumber& _bn) { *this = _bn; };

    ~BigNumber() { clear(); }

    /* ====================  ACCESSORS
     * =======================================
     */

    void setScale(int scale);

    BigNumber(const std::string& value);
    void mergeString(const std::string& value);

    BigNumber(int value);
    void mergeInt(int value);

    /* BigNumber(long long value); */
    /* void mergeInt64(int64_t value); */

    BigNumber(long value, double deci);
    void mergeLong(long value);

    BigNumber(float value);
    void mergeFloat(float value);

    BigNumber(double value);
    void mergeDouble(double value);

    BigNumber(long value);
    void mergeNumber(long number);
    /* ====================  MUTATORS
     * ======================================= */
    bool isnan();
    long number();
    int scale();
    double deci();
    std::string value();
    float FloatValue();
    double DoubleValue();
    int64_t Int64Value();
    /* ====================  OPERATORS
     * =======================================
     */

    BigNumber& operator=(const BigNumber&);
    BigNumber& operator=(const long&);
    BigNumber& operator=(const float&);
    BigNumber& operator=(const std::string&);

    BigNumber operator+(const BigNumber&) const;
    BigNumber operator-(const BigNumber&) const;
    BigNumber operator*(const BigNumber&) const;
    BigNumber operator/(const BigNumber&) const;
    BigNumber operator%(const BigNumber&) const;
    BigNumber operator+(const long&) const;
    BigNumber operator-(const long&) const;
    BigNumber operator*(const long&) const;
    BigNumber operator/(const long&) const;
    BigNumber operator%(const long&) const;
    BigNumber operator+(const float&) const;
    BigNumber operator-(const float&) const;
    BigNumber operator*(const float&) const;
    BigNumber operator/(const float&) const;
    BigNumber operator%(const float&) const;
    BigNumber operator+(const std::string&) const;
    BigNumber operator-(const std::string&) const;
    BigNumber operator*(const std::string&) const;
    BigNumber operator/(const std::string&) const;
    BigNumber operator%(const std::string&) const;

    // Arithmetic-assignment operators:
    BigNumber& operator+=(const BigNumber&);
    BigNumber& operator-=(const BigNumber&);
    BigNumber& operator*=(const BigNumber&);
    BigNumber& operator/=(const BigNumber&);
    BigNumber& operator%=(const BigNumber&);
    BigNumber& operator+=(const long&);
    BigNumber& operator-=(const long&);
    BigNumber& operator*=(const long&);
    BigNumber& operator/=(const long&);
    BigNumber& operator%=(const long&);
    BigNumber& operator+=(const float&);
    BigNumber& operator-=(const float&);
    BigNumber& operator*=(const float&);
    BigNumber& operator/=(const float&);
    BigNumber& operator%=(const float&);
    BigNumber& operator+=(const std::string&);
    BigNumber& operator-=(const std::string&);
    BigNumber& operator*=(const std::string&);
    BigNumber& operator/=(const std::string&);
    BigNumber& operator%=(const std::string&);

    // Relational operators:
    bool operator<(const BigNumber&) const;
    bool operator>(const BigNumber&) const;
    bool operator<=(const BigNumber&) const;
    bool operator>=(const BigNumber&) const;
    bool operator==(const BigNumber&) const;
    bool operator!=(const BigNumber&) const;
    bool operator<(const long&) const;
    bool operator>(const long&) const;
    bool operator<=(const long&) const;
    bool operator>=(const long&) const;
    bool operator==(const long&) const;
    bool operator!=(const long&) const;
    bool operator<(const float&) const;
    bool operator>(const float&) const;
    bool operator<=(const float&) const;
    bool operator>=(const float&) const;
    bool operator==(const float&) const;
    bool operator!=(const float&) const;
    bool operator<(const std::string&) const;
    bool operator>(const std::string&) const;
    bool operator<=(const std::string&) const;
    bool operator>=(const std::string&) const;
    bool operator==(const std::string&) const;
    bool operator!=(const std::string&) const;

protected:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS =======================================
     */
    void clear();
    /* ====================  DATA MEMBERS
     * ======================================= */
    /**
     * it's not numbe ,set false
     */
    bool _isNumber = true;
    /**
     * 无精度数字
     */
    long _number = 0;
#ifdef NUMBER_DECI

    std::size_t _scale = std::log10(NUMBER_DECI);
    double_t _deci = NUMBER_DECI;

#else
    /**
     * 精度长度
     */
    std::size_t _scale = 3;
    /**
     * 精度
     */
    double_t _deci = std::pow(10, _scale);
#endif

    /**
     * 保留原始 string
     */
    std::string _value = "";
}; /* -----  end of class BigNumber  ----- */

#define SELF(sour, oper, dest)                 \
    ({                                         \
        BigNumber __ret;                       \
        do {                                   \
            __ret.mergeNumber(sour oper dest); \
        } while (0);                           \
        __ret;                                 \
    })

#define OPERATOR(type, sour, oper, dest)                \
    ({                                                  \
        BigNumber __ret;                                \
        do {                                            \
            __ret.merge##type(dest);                    \
            __ret.mergeNumber(sour oper __ret._number); \
        } while (0);                                    \
        __ret;                                          \
    })

}  // namespace e2q
#endif /* ----- #ifndef BIGNUMBER_INC  ----- */
