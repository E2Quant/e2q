/*
 * =====================================================================================
 *
 *       Filename:  Loom.hpp
 *
 *    Description:  Loom
 *
 *        Version:  1.0
 *        Created:  2023年07月10日 11时31分35秒
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

#ifndef LOOM_INC
#define LOOM_INC
#include <algorithm>
#include <array>
#include <cstddef>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "MessagePack/foreign.hpp"
namespace e2q {

typedef std::vector<NumberType> NumberTypes;
/*
 * =====================================================================================
 *        Class:  Loom
 *  Description: 有运算逻辑的内存资源操作
 * =====================================================================================
 */
class Loom : public SilkPermit<NumberType> {
public:
    /* ====================  LIFECYCLE =======================================
     */
    Loom(); /* constructor */
    Loom(Mulberry<NumberType> mul) : SilkPermit(mul) {}

    // Loom(const Loom& _other) { *this = _other; };

    //~Loom() { released(); };
    /* ====================  ACCESSORS =======================================
     */
    Loom& where(std::function<void(int x)> const& functor);
    void push(const std::array<SeqType, ohlc_column>&){};

    /**
     *  std::cout<<"foo"<<std::endl;
     *    f(1); // calls lambda which takes an int and returns void
     *    query([](int a){std::cout<<"lambda "<<a<<std::endl;});
     */
    void query(void (*f)(int));

    void groupby(std::function<void()> const& functor);

    /* ====================  Binary operator functions
     * =======================================
     */

    void sub();
    void mul();
    void div();
    void mod();
    void pow();
    void dot();

    void lt();
    void gt();
    void le();
    void ge();
    void ne();
    void eq();

    void abs();
    void min();
    void max();
    void mean();
    void mode();
    void prod();
    void sum();
    void nique();

    /* ====================  Reindexing / selection / label manipulation
     * =======================================
     */

    void drop();
    void drop_duplicates();

    /* ====================  Missing data handling
     * =======================================
     */
    void fill();
    void isna();
    void isnull();
    void zeros();

    /* ====================  Reshaping, sorting, transposing
     * =======================================
     */
    void sort(std::function<void()> const& functor);
    void sort_values();
    void sort_index();
    /* ====================  Combining / comparing / joining / merging
     * =======================================
     */

    void join();
    void merge();
    void update();
    /* ====================  MUTATORS ======================================= */
    /**
     * 设置标识，返回指针
     */
    Loom t();
    void names(const std::list<std::string>& names);
    /**
     * 返回 column name
     */
    const std::list<std::string>& names();

    /**
     *  _block.size
     */
    size_t size() const;

    /**
     * return [row_len, col_len]
     */
    const std::pair<size_t, size_t> shape() const;

    /**
     *  is empty
     */
    bool empty();

    /* Loom head(size_t n); */
    /* Loom tail(size_t n); */
    /* ====================  OPERATORS ==========================
     */
    //    Loom& operator=(const Loom&);

    /**
     * unary ~
     */
    Loom operator~() const;
    Loom operator!() const;

    const Loom& operator[](Scope scope)
    {
        SilkPermit<NumberType>::operator[](scope);

        return *this;
    }

    /**
     * arithmetic
     */
    NumberTypes operator+(const Loom&) const;
    NumberType operator-(const Loom&) const;
    NumberType operator*(const Loom&) const;
    NumberType operator/(const Loom&) const;
    NumberType operator%(const Loom&) const;
    NumberType operator&(const Loom&) const;
    NumberType operator|(const Loom&) const;
    NumberType operator^(const Loom&) const;

    Loom operator+(const NumberType&) const;
    Loom operator-(const NumberType&) const;
    Loom operator*(const NumberType&) const;
    Loom operator/(const NumberType&) const;
    Loom operator%(const NumberType&) const;
    Loom operator&(const NumberType&) const;
    Loom operator|(const NumberType&) const;
    Loom operator^(const NumberType&) const;
    /**
     * assignment
     */
    Loom& operator+=(const Loom& lm);
    Loom& operator-=(const Loom& lm);
    Loom& operator*=(const Loom& lm);
    Loom& operator/=(const Loom& lm);
    Loom& operator%=(const Loom& lm);
    Loom& operator&=(const Loom& lm);
    Loom& operator|=(const Loom& lm);
    Loom& operator^=(const Loom& lm);

    /**
     * comparison
     */
    bool operator<(const Loom&) const;
    bool operator>(const Loom&) const;
    bool operator<=(const Loom&) const;
    bool operator>=(const Loom&) const;
    bool operator==(const Loom&) const;
    bool operator!=(const Loom&) const;

protected:
    /* ====================  METHODS ======================================= */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS ======================================= */

    /* ====================  DATA MEMBERS
     * ======================================= */
    std::list<std::string> _columns_name;

    size_t _head_num = 0;
    size_t _tail_num = 0;
}; /* -----  end of class Loom  ----- */

/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator+(const Loom<Ts...>& left, const Loom<Ts...>& right); */

/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator+(const Loom<Ts...>& left, const T right); */

/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator+(const T left, const Loom<Ts...>& right); */

/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator-(const Loom<Ts...>& left, const Loom<Ts...>& right); */
/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator-(const Loom<Ts...>& left, const T right); */
/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator-(const T left, const Loom<Ts...>& right); */

/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator*(const Loom<Ts...>& left, const Loom<Ts...>& right); */
/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator*(const Loom<Ts...>& left, const T right); */
/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator*(const T left, const Loom<Ts...>& right); */

/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator/(const Loom<Ts...>& left, const Loom<Ts...>& right); */
/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator/(const Loom<Ts...>& left, const T right); */
/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator/(const T left, const Loom<Ts...>& right); */

/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator<(const Loom<Ts...>& left, */
/*                               const Loom<Ts...>& right); */

/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator<=(const Loom<Ts...>& left, */
/*                                const Loom<Ts...>& right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator>(const Loom<Ts...>& left, */
/*                               const Loom<Ts...>& right); */

/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator>=(const Loom<Ts...>& left, */
/*                                const Loom<Ts...>& right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator==(const Loom<Ts...>& left, */
/*                                const Loom<Ts...>& right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator!=(const Loom<Ts...>& left, */
/*                                const Loom<Ts...>& right); */

/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator<(const Loom<Ts...>& left, const T right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator<=(const Loom<Ts...>& left, const T right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator>(const Loom<Ts...>& left, const T right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator>=(const Loom<Ts...>& left, const T right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator==(const Loom<Ts...>& left, const T right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator!=(const Loom<Ts...>& left, const T right); */

/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator<(const T left, const Loom<Ts...>& right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator<=(const T left, const Loom<Ts...>& right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator>(const T left, const Loom<Ts...>& right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator>=(const T left, const Loom<Ts...>& right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator==(const T left, const Loom<Ts...>& right); */
/* template <typename... Ts, typename T> */
/* Loom<unsigned char> operator!=(const T left, const Loom<Ts...>& right); */

/* template <typename... Ts, typename T> */
/* Loom<Ts...> operator^(const Loom<Ts...>& left, const T right); */

/* ------------- methos  ---------------*/

}  // namespace e2q
#endif /* ----- #ifndef LOOM_INC  ----- */
