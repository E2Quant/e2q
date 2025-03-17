/*
 * =====================================================================================
 *
 *       Filename:  Matrix.hpp
 *
 *    Description:  Matrix
 *
 *        Version:  1.0
 *        Created:  2022年11月16日 11时14分25秒
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

#ifndef MATRIX_INC
#define MATRIX_INC
#include <bits/c++config.h>

//#include <armadillo>
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

#include "ControlPack/pack.hpp"
using namespace std;
// using namespace arma;

namespace e2q {

/*
 * =====================================================================================
 *        Class:  Matrix
 *  Description:
 * =====================================================================================
 */
class Matrix : public Bucket {
public:
    /* ====================  LIFECYCLE =======================================
     */
    Matrix(); /* constructor */
    ~Matrix(){};
    /* ====================  ACCESSORS =======================================
     */

    template <typename... Ts, typename C>
    constexpr void for_types(C &&c)
    {
        (c.template operator()<Ts>(), ...);
    }
    /* ====================  MUTATORS =======================================
     */

    void push(const std::shared_ptr<Bucket> _one);

    /* ====================  OPERATORS =======================================
     */
    template <typename... Ts>
    void init();
    template <typename... Ts>
    void column(Ts... args);
    template <typename T>
    void column(T &t, const string name);
    template <typename... Ts>
    void column(tuple<Ts...> args_tuple);

    void column_name(vector<string> col) { _column_names = col; }

    template <typename T>
    void add(T &t, size_t i);
    template <typename T>
    void add(T &t, size_t i, int row_select);

    template <typename T>
    void resize_row();

    template <typename T>
    void resize_col();

    template <int Is = -1, typename... Ts>
    void update(Ts... args);
    template <int Is = -1, typename... Ts>
    void update(tuple<Ts...> args_tuple);

    /* template <typename T> */
    /* const Col<T> get_col(std::size_t c); */
    /* template <typename T> */
    /* const Col<T> get_col(const string name); */

    int indexOf(const string name);

protected:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS =======================================
     */
    std::size_t abs_n(int n, std::size_t c);
    /* ====================  DATA MEMBERS
     * ======================================= */
    vector<string> _column_names;
    vector<std::size_t> _column_ids;
}; /* -----  end of class Matrix  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Matrix::init
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  __PRETTY_FUNCTION__
 * ============================================
 */
template <typename... Ts>
void Matrix::init()
{
    auto ft = [this]<typename T>() { this->resize_col<T>(); };
    for_types<Ts...>(ft);

} /* -----  end of function Matrix::init  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Matrix::column
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename... Ts>
void Matrix::column(Ts... args)
{
    auto args_tuple = move(tuple<Ts...>(forward<Ts>(args)...));
    column(args_tuple);
}

/* -----  end of function Matrix::column  ----- */

template <typename T>
void Matrix::column(T &t, const string name)
{
    int e = indexOf(name);
    if (e == -1) {
        cout << "exist :" << name << endl;
        return;
    }
    std::size_t n = _column_ids[e];
    add(t, n);

} /* -----  end of function Matrix::column  ----- */
template <typename... Ts>
void Matrix::column(tuple<Ts...> args_tuple)
{
    auto fa = [this]<typename T, std::size_t I = 0>(T t)
    {
        if (_column_ids[I] == 0) {
            resize_row<T>();
        }
    };

    for_each_in_tuple(fa, args_tuple);

    auto fe = [this]<typename T, std::size_t I = 0>(T t) { add(t, I); };

    for_each_in_tuple(fe, args_tuple);
}
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Matrix::add
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T>
void Matrix::add(T &t, size_t i)
{
    add(t, i, -1);
}
template <typename T>
void Matrix::add(T &t, std::size_t i, int row_select)
{
    /* std::vector<Mat<T>> *mt_exist = &get_vector<Mat<T>>(); */
    /* if (mt_exist->size() == 0) { */
    /*     return; */
    /* } */
    /* else { */
    /*     Mat<T> *e = &mt_exist->at(0); */
    /*     size_t rows = e->n_rows; */
    /*     size_t row_size = abs_n(row_select, rows); */
    /*     size_t col_size = _column_ids[i]; */
    /*     e->at(row_size, col_size) = t; */
    /* } */
} /* -----  end of function Matrix::add  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Matrix::resize_col
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T>
void Matrix::resize_col()
{
    /* std::vector<Mat<T>> *mt_exist = &get_vector<Mat<T>>(); */
    /* size_t col_size = 0; */
    /* if (mt_exist->size() == 0) { */
    /*     Mat<T> v(1, 1, fill::zeros); */
    /*     push_back(v); */
    /* } */
    /* else { */
    /*     Mat<T> *e = &mt_exist->at(0); */

    /*     col_size = e->n_cols; */
    /*     size_t rows = e->n_rows; */
    /*     e->resize(rows, col_size + 1); */
    /* } */
    /* _column_ids.push_back(col_size); */
} /* -----  end of function Matrix::resize_col  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Matrix::resize_row
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T>
void Matrix::resize_row()
{
    /* std::vector<Mat<T>> *mt_exist = &get_vector<Mat<T>>(); */
    /* if (mt_exist->size() == 0) { */
    /*     Mat<T> v(1, 1, fill::zeros); */
    /*     push_back(v); */
    /* } */
    /* else { */
    /*     Mat<T> *e = &mt_exist->at(0); */
    /*     size_t col_size = e->n_cols; */
    /*     size_t rows = e->n_rows; */
    /*     Row<T> row = e->tail_rows(1); */
    /*     bool isnew = true; */
    /*     row.each_col([&isnew](auto &a) { isnew = isnew && (a[0] == 0); }); */

    /*     if (isnew == false) { */
    /*         rows++; */
    /*         /1* cout << "isnew: " << isnew << " row size:" << rows *1/ */
    /*         /1*      << " col size:" << col_size << endl; *1/ */
    /*         e->resize(rows, col_size); */
    /*     } */
    /* } */
} /* -----  end of function Matrix::resize_row  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Matrix::update
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <int Is, typename... Ts>
void Matrix::update(Ts... args)
{
    auto args_tuple = move(tuple<Ts...>(forward<Ts>(args)...));
    update<Is>(args_tuple);
} /* -----  end of function Matrix::update  ----- */

template <int Is, typename... Ts>
void Matrix::update(tuple<Ts...> args_tuple)
{
    auto fe = [this]<typename T, std::size_t N = 0>(T t) { add(t, N, Is); };

    for_each_in_tuple(fe, args_tuple);
} /* -----  end of function Matrix::update  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Matrix::get_col
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
/* template <typename T> */
/* const Col<T> Matrix::get_col(size_t c) */
/* { */
/*     Col<T> gc; */
/*     vector<Mat<T>> gv = get_vector<Mat<T>>(); */

/*     if (gv.size() == 0 || _column_ids.size() <= c) { */
/*         return gc; */
/*     } */
/*     Mat<T> gm = gv.at(0); */
/*     size_t gcnum = gm.n_cols; */
/*     size_t gid = _column_ids[c]; */
/*     if (gcnum <= gid) { */
/*         return gc; */
/*     } */

/*     gc = gm.col(gid); */
/*     return gc; */
/*}  -----  end of function Matrix::get_col  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Matrix::get_col
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
/* template <typename T> */
/* const Col<T> Matrix::get_col(const string name) */
/* { */
/*     Col<T> gc; */

/*     int n = indexOf(name); */
/*     if (n < 0) { */
/*         return gc; */
/*     } */
/*     size_t m = abs(n); */

/*     return get_col<T>(m); */
/* } */
/* -----  end of function Matrix::get_col  ----- */
}  // namespace e2q
#endif /* ----- #ifndef MATRIX_INC  ----- */
