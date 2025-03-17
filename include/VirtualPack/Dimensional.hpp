/*
 * =====================================================================================
 *
 *       Filename:  Dimensional.hpp
 *
 *    Description:  Dimensional
 *
 *        Version:  1.0
 *        Created:  2023年06月26日 17时10分38秒
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

#ifndef DIMENSIONAL_INC
#define DIMENSIONAL_INC
#include <assert.h>

#include <cstddef>
#include <ios>
#include <map>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#include "VirtualPack/foreign.hpp"

#ifdef __ARMADILLO__
#include <armadillo>
using namespace arma;
#endif

namespace e2q {

/*
 * =====================================================================================
 *        Class:  Dimensional
 *  Description: 基本多维度设计
 * =====================================================================================
 */

template <typename... Ts>
class Dimensional : public Bucket {
public:
    /* ====================  LIFECYCLE =======================================
     */

    Dimensional() : Dimensional(nullptr){};
    Dimensional(const std::vector<std::string> *names); /* constructor */
    Dimensional(const Dimensional &_other) { *this = _other; }
    ~Dimensional(){};

    /* ====================  ACCESSORS =======================================
     */
    template <typename NumericType>
    constexpr void for_types(NumericType &&c)
    {
        (c.template operator()<Ts>(), ...);
    }

    /**
     * save columns data
     */
    void toData(std::tuple<Ts...> data);

    /**
     * resize columns
     */
    template <typename nt>
    void resize(std::string name);

    template <typename bt>
    void blend(std::string name);
    /**
     * rename column
     */
    void rename(size_t index, std::string name);

    /**
     * get columns name
     */
    const std::map<size_t, std::string> columns();

    template <typename NumericType>
    void add(size_t index, NumericType t);

    template <typename NumericType, size_t N>
    void add(size_t index, const NumericType (&arr)[N]);

    template <typename NumericType>
    void append(size_t index, NumericType t);

    template <typename NumericType, size_t N>
    void append(size_t index, const NumericType (&arr)[N]);
    /**
     * Access a single value for a row/column label pair.
     */
    void at();

    /**
     * The index (row labels) of this.
     */
    void index();

    void split();
    /* ====================  MUTATORS ======================================= */

    /* ====================  OPERATORS =======================================
     */

protected:
    /* ====================  METHODS ======================================= */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS ======================================= */

    /* ====================  DATA MEMBERS
     * ======================================= */
    std::map<size_t, std::string> columns_name;
}; /* -----  end of class Dimensional  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Dimensional:Dimensional
 *  ->  void *
 *  Parameters:
 *  - const std::vector<std::string> *names  // The column labels of this.
 *
 *  Description:
 *
 * ============================================
 */
template <typename... Ts>
Dimensional<Ts...>::Dimensional(const std::vector<std::string> *names)
{
    int m = 0;
    int nsize = -1;
    if (names != nullptr) {
        nsize = names->size();
    }

    auto ft = [this, names, &m, &nsize]<typename T>() {
        if (nsize > m) {
            this->resize<T>(names->at(m));
        }
        else {
            this->resize<T>(log::format("%d", m));
        }

        m++;
    };
    for_types(ft);

} /* -----  end of function Dimensional:Dimensional  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Dimensional<Ts...>::resize
 *  ->  void *
 *  Parameters:
 *  - string name
 *  Description:
 *  新增加一个值 add columns
 * ============================================
 */
template <typename... Ts>
template <typename nt>
void Dimensional<Ts...>::resize(std::string name)
{
#ifdef __ARMADILLO__
    static_assert(std::is_arithmetic<nt>::value, "NumericType must be numeric");

    Col<nt> vt;
#else
    std::vector<nt> vt;
#endif
    push_back(vt);

    size_t csize = columns_name.size();
    columns_name.insert({csize, name});

} /* -----  end of function Dimensional<Ts...>::resize  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Dimensional<Ts...>::blend
 *  ->  void *
 *  Parameters:
 *  - string name
 *  Description:
 *  任意类型 add columns
 * ============================================
 */
template <typename... Ts>
template <typename bt>
void Dimensional<Ts...>::blend(std::string name)
{
    std::vector<bt> vt;
    push_back(vt);

    size_t csize = columns_name.size();
    columns_name.insert({csize, name});
} /* -----  end of function Dimensional<Ts...>::blend  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Dimensional<Ts...>::rename
 *  ->  void *
 *  Parameters:
 *  - size_t  index
 *  - string name
 *  Description:
 *  修改 columns name
 * ============================================
 */
template <typename... Ts>
void Dimensional<Ts...>::rename(size_t index, std::string name)
{
    size_t csize = columns_name.size();
    if (index >= csize) {
        return;
    }
    columns_name[index] = name;

} /* -----  end of function Dimensional<Ts...>::rename  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Dimensional<Ts...>::columns
 *  ->  void *
 *  Parameters:
 *
 *  Description:
 *  返回 column name
 * ============================================
 */
template <typename... Ts>
const std::map<size_t, std::string> Dimensional<Ts...>::columns()
{
    return columns_name;
} /* -----  end of function Dimensional<Ts...>::columns  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Dimensional<Ts...>::add
 *  ->  void *
 *  Parameters:
 *  - NumericType t
 *  Description:
 *
 * ============================================
 */
template <typename... Ts>
template <typename NumericType>
void Dimensional<Ts...>::add(size_t index, NumericType t)
{
#ifdef __ARMADILLO__
    static_assert(std::is_arithmetic<NumericType>::value,
                  "NumericType must be numeric");
    Col<NumericType> vt;
#else
    std::vector<NumericType> vt;
#endif

} /* -----  end of function Dimensional<Ts...>::add  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Dimensional<Ts...>::add
 *  ->  void *
 *  Parameters:
 *  - size_t  index  columns number
 *  - NumericType arr[];
 *  Description:
 *
 * ============================================
 */
template <typename... Ts>
template <typename NumericType, size_t N>
void Dimensional<Ts...>::add(size_t index, const NumericType (&arr)[N])
{
#ifdef __ARMADILLO__
    static_assert(std::is_arithmetic<NumericType>::value,
                  "NumericType must be numeric");
    std::vector<Col<NumericType>> col = get_vector<Col<NumericType>>();
    bool is_outsize_max_size = col.size() > index;
    assert(is_outsize_max_size && "index > Col size");
    int m = 0;
    col[index].imbue([&arr, &m]() { return arr[m++]; });
#else
    std::vector<std::vector<NumericType>> vt =
        get_vector<std::vector<NumericType>>();
#endif

} /* -----  end of function Dimensional<Ts...>::add  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Dimensional<Ts...>::append
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename... Ts>
template <typename NumericType>
void Dimensional<Ts...>::append(size_t index, NumericType t)
{
} /* -----  end of function Dimensional<Ts...>::append  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Dimensional<Ts...>::append
 *  ->  void *
 *  Parameters:
 *  - size_t  index
 *  - NumericType arr[]
 *  Description:
 *
 * ============================================
 */
template <typename... Ts>
template <typename NumericType, size_t N>
void Dimensional<Ts...>::append(size_t index, const NumericType (&arr)[N])
{
#ifdef __ARMADILLO__
    static_assert(std::is_arithmetic<NumericType>::value,
                  "NumericType must be numeric");
    std::vector<Col<NumericType>> col = get_vector<Col<NumericType>>();
    bool is_outsize_max_size = col.size() > index;
    assert(is_outsize_max_size && "index > Col size");
    std::size_t old_cols = col[index].n_cols;
    size_t new_cols = old_cols + N;
    col[index].resize(new_cols);
    uword i = old_cols;
    int m = 0;
    while (i < new_cols) {
        col[index].at(i) = arr[m];
        m++;
        i++;
    }
#else
    std::vector<std::vector<NumericType>> vt =
        get_vector<std::vector<NumericType>>();
#endif
} /* -----  end of function Dimensional<Ts...>::append  ----- */

}  // namespace e2q

#endif /* ----- #ifndef DIMENSIONAL_INC  ----- */
