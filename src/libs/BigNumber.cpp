/*
 * =====================================================================================
 *
 *       Filename:  BigNumber.cpp
 *
 *    Description:  BigNumber.cpp
 *
 *        Version:  1.0
 *        Created:  2023年08月21日 14时37分54秒
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
#include "libs/BigNumber.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <string>
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  setScale
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BigNumber::setScale(int scale)
{
    _scale = scale;
    _deci = std::pow(10, _scale);
} /* -----  end of function setScale  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber::BigNumber(const std::string &value)
{
    mergeString(value);
} /* -----  end of function BigNumber  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::mergeString
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BigNumber::mergeString(const std::string &value)
{
    clear();
    std::remove_copy(value.begin(), value.end(), std::back_inserter(_value),
                     ',');

    if (ISNUMBER(_value)) {
        _number = std::stol(_value) * _deci;
    }
    else if (ISFLOAT(_value)) {
        float fval = std::stof(_value) * float(_deci);
        _number = long(fval);
    }
    else {
        _isNumber = false;
    }

} /* -----  end of function BigNumber::mergeString  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::BigNumber
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber::BigNumber(int value)
{
    mergeInt(value);
} /* -----  end of function BigNumber::BigNumber  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::mergeInt
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BigNumber::mergeInt(int value)
{
    clear();
    _number = long(value * _deci);
    _value = std::to_string(value);
} /* -----  end of function BigNumber::mergeInt  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::BigNumber
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber::BigNumber(long value, double deci)
{
    _deci = deci;
    mergeLong(value);
} /* -----  end of function BigNumber::BigNumber  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::mergeLong
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BigNumber::mergeLong(long value)
{
    clear();
    _number = value * _deci;
    _value = std::to_string(value);
} /* -----  end of function BigNumber::mergeLong  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::BigNumber
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber::BigNumber(float value)
{
    mergeFloat(value);
} /* -----  end of function BigNumber::BigNumber  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::mergeFloat
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BigNumber::mergeFloat(float value)
{
    clear();
    _number = long(value * float(_deci));
    _value = std::to_string(value);
} /* -----  end of function BigNumber::mergeFloat  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::BigNumber
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber::BigNumber(double value)
{
    mergeDouble(value);
} /* -----  end of function BigNumber::BigNumber  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::mergeDouble
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BigNumber::mergeDouble(double value)
{
    clear();
    _number = value * _deci;
    _value = std::to_string(value);
} /* -----  end of function BigNumber::mergeDouble  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::BigNumber
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber::BigNumber(long value)
{
    mergeNumber(value);
} /* -----  end of function BigNumber::BigNumber  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::mergeNumber
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BigNumber::mergeNumber(long value)
{
    clear();
    _number = value;
    _value = std::to_string(value);
} /* -----  end of function BigNumber::mergeNumber  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  isnan
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool BigNumber::isnan()
{
    return _value.empty() || _isNumber == false;
} /* -----  end of function isnan  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::number
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
long BigNumber::number()
{
    return _number;
} /* -----  end of function BigNumber::number  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::scale
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int BigNumber::scale()
{
    return _scale;
} /* -----  end of function BigNumber::scale  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::deci
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
double BigNumber::deci()
{
    return _deci;
} /* -----  end of function BigNumber::deci  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::value
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::string BigNumber::value()
{
    return _value;
} /* -----  end of function BigNumber::value  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::FloatValue
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
float BigNumber::FloatValue()
{
    if (_isNumber && _deci > 0) {
        return float(_number / _deci);
    }
    else if (_isNumber && _deci == 0) {
        return float(_number);
    }
    assert("/ by zero");
    return 0;
} /* -----  end of function BigNumber::FloatValue  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::DoubleValue
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
double BigNumber::DoubleValue()
{
    if (_number == 0 || _deci == 0) {
        return 0;
    }
    if (_isNumber && _deci > 0) {
        return double(_number / _deci);
    }
    else if (_isNumber && _deci == 0) {
        return double(_number);
    }
    else {
        return 0;
    }
} /* -----  end of function BigNumber::DoubleValue  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::Int64Value
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int64_t BigNumber::Int64Value()
{
    if (_number == 0 || _deci == 0) {
        return 0;
    }
    if (_isNumber && _deci > 0) {
        return int64_t(_number / _deci);
    }
    else if (_isNumber && _deci == 0) {
        return int64_t(_number);
    }
    else {
        return 0;
    }
} /* -----  end of function BigNumber::Int64Value  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::clear
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BigNumber::clear()
{
    _isNumber = true;
    _number = 0;
    _value = "";
} /* -----  end of function BigNumber::clear  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator=
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber &BigNumber::operator=(const BigNumber &_other)
{
    if (this != &_other) {
        _number = _other._number;
        _isNumber = _other._isNumber;
        _deci = _other._deci;
        _scale = _other._scale;
        _value = _other._value;
    }

    return *this;
}
BigNumber &BigNumber::operator=(const long &bnum)
{
    mergeLong(bnum);

    return *this;
}
BigNumber &BigNumber::operator=(const float &bnum)
{
    mergeFloat(bnum);

    return *this;
}
BigNumber &BigNumber::operator=(const std::string &bstr)
{
    mergeString(bstr);

    return *this;
}

/* -----  end of function BigNumber::operator=  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator+
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber BigNumber::operator+(const BigNumber &other) const
{
    return SELF(_number, +, other._number);
}

BigNumber BigNumber::operator+(const long &val) const
{
    return OPERATOR(Double, _number, +, val);
}

BigNumber BigNumber::operator+(const std::string &val) const
{
    return OPERATOR(String, _number, +, val);
}

BigNumber BigNumber::operator+(const float &val) const
{
    return OPERATOR(Float, _number, +, val);
} /* -----  end of function BigNumber::operator+  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator-
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber BigNumber::operator-(const BigNumber &other) const
{
    return SELF(_number, -, other._number);
}
BigNumber BigNumber::operator-(const long &val) const
{
    return OPERATOR(Double, _number, -, val);
}
BigNumber BigNumber::operator-(const float &val) const
{
    return OPERATOR(Float, _number, -, val);
}
BigNumber BigNumber::operator-(const std::string &val) const
{
    return OPERATOR(String, _number, -, val);
}
/* -----  end of function BigNumber::operator-  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator*
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber BigNumber::operator*(const BigNumber &other) const
{
    return SELF(_number, *, other._number);
}
BigNumber BigNumber::operator*(const long &val) const
{
    return OPERATOR(Long, _number, *, val);
}
BigNumber BigNumber::operator*(const float &val) const
{
    return OPERATOR(Float, _number, *, val);
}
BigNumber BigNumber::operator*(const std::string &val) const
{
    return OPERATOR(String, _number, *, val);
}
/* -----  end of function BigNumber::operator*  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator/
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber BigNumber::operator/(const BigNumber &other) const
{
    return SELF(_number, /, other._number);
}
BigNumber BigNumber::operator/(const long &val) const
{
    return OPERATOR(Long, _number, /, val);
}
BigNumber BigNumber::operator/(const float &val) const
{
    return OPERATOR(Float, _number, /, val);
}
BigNumber BigNumber::operator/(const std::string &val) const
{
    return OPERATOR(String, _number, /, val);
}
/* -----  end of function BigNumber::operator/  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator%
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber BigNumber::operator%(const BigNumber &other) const
{
    return SELF(_number, %, other._number);
}
BigNumber BigNumber::operator%(const long &val) const
{
    return OPERATOR(Long, _number, %, val);
}
BigNumber BigNumber::operator%(const float &val) const
{
    return OPERATOR(Float, _number, %, val);
}
BigNumber BigNumber::operator%(const std::string &val) const
{
    return OPERATOR(String, _number, %, val);
}
/* -----  end of function BigNumber::operator%  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator+=
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber &BigNumber::operator+=(const BigNumber &other)
{
    *this = *this + other;
    return *this;
}

BigNumber &BigNumber::operator+=(const long &val)
{
    *this = *this + val;
    return *this;
}

BigNumber &BigNumber::operator+=(const float &val)
{
    *this = *this + val;
    return *this;
}

BigNumber &BigNumber::operator+=(const std::string &val)
{
    *this = *this + val;
    return *this;
}
/* -----  end of function BigNumber::operator+=  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator-=
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber &BigNumber::operator-=(const BigNumber &other)
{
    *this = *this - other;
    return *this;
}

BigNumber &BigNumber::operator-=(const long &val)
{
    *this = *this - val;
    return *this;
}

BigNumber &BigNumber::operator-=(const float &val)
{
    *this = *this - val;
    return *this;
}

BigNumber &BigNumber::operator-=(const std::string &val)
{
    *this = *this - val;
    return *this;
}
/* -----  end of function BigNumber::operator-=  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator*=
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber &BigNumber::operator*=(const BigNumber &other)
{
    *this = *this * other;
    return *this;
}

BigNumber &BigNumber::operator*=(const long &val)
{
    *this = *this * val;
    return *this;
}

BigNumber &BigNumber::operator*=(const float &val)
{
    *this = *this * val;
    return *this;
}

BigNumber &BigNumber::operator*=(const std::string &val)
{
    *this = *this * val;
    return *this;
}
/* -----  end of function BigNumber::operator*=  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator/=
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber &BigNumber::operator/=(const BigNumber &other)
{
    *this = *this / other;
    return *this;
}

BigNumber &BigNumber::operator/=(const long &val)
{
    *this = *this / val;
    return *this;
}

BigNumber &BigNumber::operator/=(const float &val)
{
    *this = *this / val;
    return *this;
}

BigNumber &BigNumber::operator/=(const std::string &val)
{
    *this = *this / val;
    return *this;
}
/* -----  end of function BigNumber::operator/=  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator%=
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
BigNumber &BigNumber::operator%=(const BigNumber &other)
{
    *this = *this % other;
    return *this;
}

BigNumber &BigNumber::operator%=(const long &val)
{
    *this = *this % val;
    return *this;
}

BigNumber &BigNumber::operator%=(const float &val)
{
    *this = *this % val;
    return *this;
}

BigNumber &BigNumber::operator%=(const std::string &val)
{
    *this = *this % val;
    return *this;
}
/* -----  end of function BigNumber::operator%=  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator<
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool BigNumber::operator<(const BigNumber &other) const
{
    return _number < other._number;
}
bool BigNumber::operator<(const long &val) const
{
    BigNumber bn = val;
    return _number < bn._number;
}
bool BigNumber::operator<(const float &val) const
{
    BigNumber bn = val;
    return _number < bn._number;
}
bool BigNumber::operator<(const std::string &val) const
{
    BigNumber bn = val;
    return _number < bn._number;
}
/* -----  end of function BigNumber::operator<  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator>
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool BigNumber::operator>(const BigNumber &other) const
{
    return _number > other._number;
}
bool BigNumber::operator>(const long &val) const
{
    BigNumber bn = val;
    return _number > bn._number;
}
bool BigNumber::operator>(const float &val) const
{
    BigNumber bn = val;
    return _number > bn._number;
}
bool BigNumber::operator>(const std::string &val) const
{
    BigNumber bn = val;
    return _number > bn._number;
}
/* -----  end of function BigNumber::operator>  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator<=
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool BigNumber::operator<=(const BigNumber &other) const
{
    return _number <= other._number;
}
bool BigNumber::operator<=(const long &val) const
{
    BigNumber bn = val;
    return _number <= bn._number;
}
bool BigNumber::operator<=(const float &val) const
{
    BigNumber bn = val;
    return _number <= bn._number;
}
bool BigNumber::operator<=(const std::string &val) const
{
    BigNumber bn = val;
    return _number <= bn._number;
}
/* -----  end of function BigNumber::operator<=  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator>=
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool BigNumber::operator>=(const BigNumber &other) const
{
    return _number >= other._number;
}
bool BigNumber::operator>=(const long &val) const
{
    BigNumber bn = val;
    return _number >= bn._number;
}
bool BigNumber::operator>=(const float &val) const
{
    BigNumber bn = val;
    return _number >= bn._number;
}
bool BigNumber::operator>=(const std::string &val) const
{
    BigNumber bn = val;
    return _number >= bn._number;
}
/* -----  end of function BigNumber::operator>=  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator==
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool BigNumber::operator==(const BigNumber &other) const
{
    return _number == other._number;
}
bool BigNumber::operator==(const long &val) const
{
    BigNumber bn = val;
    return _number == bn._number;
}
bool BigNumber::operator==(const float &val) const
{
    BigNumber bn = val;
    return _number == bn._number;
}
bool BigNumber::operator==(const std::string &val) const
{
    BigNumber bn = val;
    return _number == bn._number;
}
/* -----  end of function BigNumber::operator==  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BigNumber::operator!=
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool BigNumber::operator!=(const BigNumber &other) const
{
    return _number != other._number;
}
bool BigNumber::operator!=(const long &val) const
{
    BigNumber bn = val;
    return _number != bn._number;
}
bool BigNumber::operator!=(const float &val) const
{
    BigNumber bn = val;
    return _number != bn._number;
}
bool BigNumber::operator!=(const std::string &val) const
{
    BigNumber bn = val;
    return _number != bn._number;
}
/* -----  end of function BigNumber::operator!=  ----- */

}  // namespace e2q
