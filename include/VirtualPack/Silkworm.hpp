/*
 * =====================================================================================
 *
 *       Filename:  Silkworm.hpp
 *
 *    Description:  Silkworm
 *
 *        Version:  1.0
 *        Created:  2023年07月14日 14时32分44秒
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

#ifndef SILKWORM_INC
#define SILKWORM_INC
#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "VirtualPack/BinTree.hpp"
namespace e2q {

#define SilkType std::pair<size_t, size_t>

/**
 * 2D 内存数据
 *
 */
template <typename T>
struct Silk {
    /**
     * pair first == data[N] start
     * pair second == data[M] offset
     * offset =  total
     */
    std::vector<SilkType> await_list;
    /**
     * a jagged 2D dynamic array with variable column sizes
     */
    T* data;
}; /* ----------  end of struct __Silkworm  ---------- */

/**
 * message id for anyone
 *
 */
namespace _ENUM {

enum __rw { r = 0, w, rw }; /* ----------  end of enum __rw  ---------- */
typedef enum __rw _RW;

enum ___rcmajor {
    // 行向量
    row = 0,
    // 列向量
    col = 1
}; /* ----------  end of enum ___rcmajor  ---------- */
typedef enum ___rcmajor _MAJOR;
}  // namespace _ENUM

template <typename T>
struct Mulberry {
    size_t index;          // silkworm index
    size_t first;          // index start _data
    size_t offset;         // index offset == total _data, ptr offset -= 1
    size_t row;            // _data  row;
    size_t column;         // _data column
    bool overlap = false;  // overlap data, 当满之后，是否可以对数据 进行覆盖
    _ENUM::_MAJOR major;   // row-major  col-major
    _ENUM::_RW rw;         // ctrl _data
    T* data;               // data ptr
    std::function<void(size_t, size_t, size_t)> callback;  // release data
}; /* ----------  end of struct __Mulberry  ---------- */

/*
 * ===  FUNCTION  ==============================
 *         Name:  MAJOR
 *  Description: major select
 * =============================================
 */
#define MAJOR(row_val, col, mul)                          \
    ({                                                    \
        size_t __ret_val = 0;                             \
        do {                                              \
            if (mul.major == e2q::_ENUM::row) {           \
                __ret_val = (row_val * mul.column + col); \
            }                                             \
            else {                                        \
                __ret_val = (col * mul.row + row_val);    \
            }                                             \
        } while (0);                                      \
        __ret_val;                                        \
    })

/**
 * split struct
 */
struct Scope {
    /**
     *  column // 从哪一列开始
     *
     *  size_c // 占用的列总数
     */
    size_t column, size_c;
};

struct __Coordinate {
    size_t row;
    size_t column;
}; /* ----------  end of struct Coordinate  ---------- */

typedef struct __Coordinate Coordinate;

#define ATOMICPTR(ptr)                                       \
    ({                                                       \
        size_t __ret = 0;                                    \
        do {                                                 \
            size_t v = ptr->load(std::memory_order_acquire); \
            if (v == 0) {                                    \
                __ret = _mulberry.row - 1;                   \
            }                                                \
            else {                                           \
                __ret = v - 1;                               \
            }                                                \
        } while (0);                                         \
        __ret;                                               \
    })

#define ATOMICADD(ptr)                                                      \
    ({                                                                      \
        do {                                                                \
            size_t v =                                                      \
                (ptr->load(std::memory_order_acquire) + 1) % _mulberry.row; \
            ptr->store(v, std::memory_order_release);                       \
        } while (0);                                                        \
    })

#define ATOMICADDS(ptr, num)                                              \
    ({                                                                    \
        do {                                                              \
            if (num > 0) {                                                \
                size_t v = (ptr->load(std::memory_order_acquire) + num) % \
                           _mulberry.row;                                 \
                ptr->store(v, std::memory_order_release);                 \
            }                                                             \
        } while (0);                                                      \
    })

#define ATOMICEQ(s, ptr)                                          \
    ({                                                            \
        bool __ret = false;                                       \
        do {                                                      \
            __ret = (*s == ptr->load(std::memory_order_acquire)); \
        } while (0);                                              \
        __ret;                                                    \
    })

#define ATOMICEQPTR(ptr0, ptr1)                                \
    ({                                                         \
        bool __ret = false;                                    \
        do {                                                   \
            size_t p0 = ptr0->load(std::memory_order_acquire); \
            size_t p1 = ptr1->load(std::memory_order_acquire); \
            __ret = (p0 == p1);                                \
        } while (0);                                           \
        __ret;                                                 \
    })

/**
 * _tailIndex, row, _writingIndex, _mulberry.row
 */
#define ATOMICEBETWEEN(ptr0, val, ptr1, mul)                   \
    ({                                                         \
        bool __ret = false;                                    \
        do {                                                   \
            size_t p0 = ptr0->load(std::memory_order_acquire); \
            size_t p1 = ptr1->load(std::memory_order_acquire); \
            bool one = (p0 <= val && val < mul);               \
            bool two = (0 <= val && val < p1);                 \
            __ret = (one || two);                              \
            if (__ret == false) {                              \
                elog::bug("_tailIndex:", p0, " val:", val,     \
                          " _writingIndex:", p1);              \
            }                                                  \
        } while (0);                                           \
        __ret;                                                 \
    })

#define ATOMICSIZT_T(ptr)                                 \
    ({                                                    \
        size_t __ret = 0;                                 \
        do {                                              \
            __ret = ptr->load(std::memory_order_acquire); \
        } while (0);                                      \
        __ret;                                            \
    })

/**
 * 对比 ptr0 -gt ptr1, and (ptr0-ptr1 or mul - ptr1 + ptr0);
 *  _writedIndex, _tailIndex, _mulberry.row
 */
#define ATOMICDIFF(ptr0, ptr1, mul)                           \
    ({                                                        \
        size_t __ret = 0;                                     \
        do {                                                  \
            size_t o = *ptr0;                                 \
            size_t t = ptr1->load(std::memory_order_acquire); \
            __ret = o > t ? o - t : (mul - t + o);            \
        } while (0);                                          \
        __ret;                                                \
    })

/*
 * =====================================================================================
 *        Class:  SilkPermit
 *  Description:  silktree permission circular buffer
 *              操作某一小块 Silkworm 的类
 *    read
 *    |
 *    v
 *   +----------+
 *   |xxxxxxx   |
 *   +----------+
 *           ^
 *           |
 *        write
 * =====================================================================================
 */
template <typename T, typename Lock = e2q::BasicLock>
class SilkPermit {
public:
    /* ====================  LIFECYCLE
     * =======================================
     */
    SilkPermit() {}
    SilkPermit(const Mulberry<T> mul) : _mulberry(mul)
    {
        _writingIndex = std::make_shared<std::atomic_size_t>(0);
        _tailIndex = std::make_shared<std::atomic_size_t>(0);

        _writedIndex = std::make_shared<size_t>(0);
        _writedTotal = 0;
    }; /* constructor */
    // SilkPermit() = default; /* constructor */
    SilkPermit(const SilkPermit& _bn) { *this = _bn; };

    /*
     * ===  FUNCTION
     * ======================================================================
     *         Name:  SilkPermit
     *  Description:  release btree
     * =====================================================================================
     */
    ~SilkPermit() { released(); };
    /* ====================  ACCESSORS
     * =======================================
     */
    size_t rows() const { return _mulberry.row; }

    size_t columns() const { return _mulberry.column; }
    bool isnull() const { return _mulberry.data == nullptr; }
    int writed()
    {
        if (_writedIndex == nullptr) {
            elog::bug("writedIndex is nullptr");
            return -1;
        }
        std::size_t m = ATOMICSIZT_T(_writingIndex);
        if (m == 0) {
            // elog::bug("_writingIndex == 0");
            return 0;
        }
        return *_writedIndex;
    }

    /**
     * 写入总数
     */
    std::uint64_t total() { return _writedTotal; }

    /*
     * ===  FUNCTION
     * ======================================================================
     *         Name:  push
     *  Description:  push add to row,column
     * =====================================================================================
     */
    int push(size_t row, size_t column, const T& data)
    {
        Lock _lock(SilkPMutex);
        if (isnull()) {
            elog::bug("data no init!");
            return -1;
        }
        if (_mulberry.rw == _ENUM::r) {
            elog::bug("data is read only");
            return -1;
        }
        if (row >= _mulberry.row || column >= _mulberry.column) {
            elog::bug("row must -lt rows, column must -lt column");
            return -1;
        }
        if (ATOMICEQ(_writedIndex, _writingIndex)) {
            elog::bug("data not init");
            return -1;
        }

        size_t now = hash_row();

        if (row > now) {
            // it's the last row
            elog::bug("row -gt bottom ");

            return -1;
        }
        size_t top = *_writedIndex;
        size_t realRow =
            (top < row ? (_mulberry.row - row + top) % _mulberry.row
                       : top - row);

        size_t index = 0;

        index = MAJOR(realRow, column, _mulberry);

        *(_mulberry.data + index) = std::move(data);

        return 0;
    }
    template <typename std::size_t Nc>
    int push(size_t row, const std::array<T, Nc>& data)
    {
        Lock _lock(SilkPMutex);
        if (isnull()) {
            elog::bug("data no init!");
            return -1;
        }
        if (_mulberry.rw == _ENUM::r) {
            elog::bug("data is read only");
            return -1;
        }
        if (row >= _mulberry.row || data.size() != _mulberry.column) {
            elog::bug("row must -lt rows, column must -lt column");
            return -1;
        }
        if (ATOMICEQ(_writedIndex, _writingIndex)) {
            elog::bug("data not init");
            return -1;
        }

        size_t now = hash_row();

        if (row > now) {
            // it's the last row
            elog::bug("row -gt bottom ");

            return -1;
        }
        size_t top = *_writedIndex;
        size_t realRow =
            (top < row ? (_mulberry.row - row + top) % _mulberry.row
                       : top - row);

        size_t index = 0;

        for (size_t column = 0; column < data.size(); column++) {
            index = MAJOR(realRow, column, _mulberry);

            *(_mulberry.data + index) = std::move(data[column]);
        }
        return 0;
    }
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  insert
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    template <typename std::size_t Nc = 10, size_t Nr = 10>
    std::vector<int> insert(const std::array<std::array<T, Nc>, Nr>& data)
    {
        std::vector<int> ids;
        if (isnull()) {
            elog::bug("data no init!");
            return ids;
        }
        int ret = 0;

        for (std::array<T, Nc> dn : data) {
            ret = insert(dn);
            ids.push_back(ret);
        }

        return ids;
    }

    template <typename std::size_t Nc = 10>
    int insert(const std::array<T, Nc>& data)
    {
        if (check(Nc) == -1) {
            return -1;
        }
        Lock _lock(SilkPMutex);

        size_t index = 0;
        size_t column = 0;
        for (T m : data) {
            index = MAJOR(ATOMICSIZT_T(_writingIndex), column, _mulberry);
            *(_mulberry.data + index) = std::move(m);
            column++;
        }

        ATOMICADD(_writingIndex);
        *_writedIndex = ATOMICPTR(_writingIndex);

        _writedTotal++;
        return ATOMICSIZT_T(_writingIndex);
    }

    int insert(const std::shared_ptr<T[]> ptr, size_t size)
    {
        if (check(size) == -1) {
            return -1;
        }
        Lock _lock(SilkPMutex);

        size_t index = 0;
        size_t column = 0;
        for (size_t m = 0; m < size; m++) {
            index = MAJOR(ATOMICSIZT_T(_writingIndex), column, _mulberry);
            *(_mulberry.data + index) = std::move(ptr[m]);
            column++;
        }

        ATOMICADD(_writingIndex);
        *_writedIndex = ATOMICPTR(_writingIndex);

        _writedTotal++;

        return ATOMICSIZT_T(_writingIndex);
    }
    /* -----  end of function insert  ----- */
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  check
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */

    int check(size_t Nc)
    {
        if (isnull()) {
            elog::bug("data no init!");
            return -1;
        }
        if (Nc != _mulberry.column) {
            elog::bug("Nm must eq column, ", Nc, "  c:", _mulberry.column);

            return -1;
        }

        if (_mulberry.rw == _ENUM::r) {
            elog::bug("data is read only");
            return -1;
        }
        size_t next_write = ((*_writedIndex) + 1) % _mulberry.row;
        size_t tail = ATOMICSIZT_T(_tailIndex);

        if (next_write == tail) {
            if (_mulberry.overlap) {
                ATOMICADD(_tailIndex);
            }
            else {
                elog::bug("data is full");
                return -1;
            }
        }

        return 0;
    }

    /* -----  end of function check  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  cursor
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *  while read out 1 data and next + 1
     *  circular cursor, 从后面加收内存空间
     * ============================================
     */
    int cursor()
    {
        if (isnull()) {
            elog::bug("data no init!");
            return -1;
        }
        if (_mulberry.rw == _ENUM::r) {
            elog::bug("data is read only");
            return -1;
        }

        // one writed not clear
        if (ATOMICEQ(_writedIndex, _tailIndex)) {
            elog::info("data is empty");
            return -1;
        }
        ATOMICADD(_tailIndex);

        return 0;
    } /* -----  end of function cursor  ----- */
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  cursor
     *  ->  void *
     *  Parameters:
     *  - size_t  num 擦除多少数据
     *  Description:
     *
     * ============================================
     */
    int cursor(size_t num)
    {
        if (isnull()) {
            elog::bug("data no init!");
            return -1;
        }
        if (_mulberry.rw == _ENUM::r) {
            elog::bug("data is read only");
            return -1;
        }

        // one writed not clear
        if (ATOMICEQ(_writedIndex, _tailIndex)) {
            elog::bug("data is empty");
            return -1;
        }

        /**
         * 可擦除的数据
         */
        size_t real_num = ATOMICDIFF(_writedIndex, _tailIndex, _mulberry.row);

        if (num > real_num) {
            elog::bug("num:", num, " real_num:", real_num);
            num = real_num;
        }

        ATOMICADDS(_tailIndex, num);

        return 0;

    } /* -----  end of function cursor  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  tail
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description: return the tail value
     *   --- follow the row change ---
     * ============================================
     */
    template <typename std::size_t Nc = 10>
    int tail(std::array<T, Nc>* data)
    {
        tail(data, -1);
        return 0;
    }

    template <typename std::size_t Nc = 10, size_t Nr = 10>
    int tail(std::array<std::array<T, Nc>, Nr>* data)
    {
        int row = -1;
        int ret = 0;
        for (auto it = data->begin(); it != data->end(); ++it) {
            ret = tail(it, row);
            if (ret == -1) {
                break;
            }
            row--;
            if (abs(row) >= _mulberry.row) {
                break;
            }
        }
        return row;
    }
    template <typename std::size_t Nc = 10>
    int tail(std::array<T, Nc>* data, const int row)
    {
        if (isnull()) {
            elog::bug("data no init!");
            return -1;
        }
        if (Nc != _mulberry.column) {
            elog::bug("Nm must eq column");
            return -1;
        }

        if (ATOMICEQ(_writedIndex, _writingIndex)) {
            elog::bug("data not init");
            return -1;
        }
        size_t _row = abs(row);
        if (_row == 0) {
            return -1;
        }

        size_t total = hash_row() + 1;
        if (total < _row) {
            // it's the top row
            elog::bug("row -gt top ");
            return -1;
        }
        _row = total - _row;

        // elog::echo("_row:", _row);

        size_t writed = *_writedIndex;

        size_t realRow =
            (writed < _row ? (_mulberry.row - _row + writed) % _mulberry.row
                           : writed - _row);

        size_t index = 0, column = 0;

        for (column = 0; column < _mulberry.column; column++) {
            index = MAJOR(realRow, column, _mulberry);
            data->at(column) = *(_mulberry.data + index);
        }

        return 0;
    }
    /* -----  end of function tail  ----- */
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  read
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *  --- follow the row change ---
     * ============================================
     */
    template <typename std::size_t Nc = 10>
    int read(std::array<T, Nc>* data)
    {
        if (isnull()) {
            elog::bug("data no init!");
            return -1;
        }
        if (Nc != _mulberry.column) {
            elog::bug("Nm must eq column");
            return -1;
        }

        if (ATOMICEQ(_writedIndex, _writingIndex)) {
            // elog::bug("data not init");
            return -1;
        }
        size_t row = 0;
        return read(data, row);

    } /* -----  end of function read  ----- */

    template <typename std::size_t Nc = 10, size_t Nr = 10>
    int read(std::array<std::array<T, Nc>, Nr>* data)
    {
        if (isnull()) {
            elog::bug("data no init!");
            return -1;
        }
        if (Nc != _mulberry.column) {
            elog::bug("Nm must eq column");
            return -1;
        }

        if (ATOMICEQ(_writedIndex, _writingIndex)) {
            elog::bug("data not init");
            return -1;
        }
        size_t row = 0;
        int ret = 0;
        for (auto it = data->begin(); it != data->end(); ++it) {
            ret = read(it, row);
            if (ret == -1) {
                break;
            }
            row++;
            if (row >= _mulberry.row) {
                break;
            }
        }
        return row;
    }
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  read
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    template <typename std::size_t Nc = 10>
    int read(std::array<T, Nc>* data, const size_t row)
    {
        if (isnull()) {
            elog::bug("data no init!");
            return -1;
        }
        if (Nc != _mulberry.column) {
            elog::bug("Nm must eq column, Nc:", Nc,
                      "  column:", _mulberry.column);
            return -1;
        }
        if (ATOMICEQ(_writedIndex, _writingIndex)) {
            // elog::bug("data not init");
            return -1;
        }

        size_t now = hash_row();
        size_t writed = *_writedIndex;
        size_t realRow =
            (writed < row ? (_mulberry.row - row + writed) % _mulberry.row
                          : writed - row);

        if (row > now) {
            // it's the last row
            elog::bug("row -gt bottom row:", row, " now:", now);
            return -1;
        }

        size_t index = 0, column = 0;

        for (column = 0; column < _mulberry.column; column++) {
            index = MAJOR(realRow, column, _mulberry);
            data->at(column) = *(_mulberry.data + index);
        }

        return realRow;
    } /* -----  end of function read  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  read
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    template <typename std::size_t Nc = 10>
    int read(std::array<T, Nc>* data, const size_t row,
             func_type_ret<bool, T> fun)

    {
        if (isnull()) {
            elog::bug("data no init!");
            return -1;
        }
        if (Nc != _mulberry.column) {
            elog::bug("Nm must eq column, Nc:", Nc,
                      "  column:", _mulberry.column);
            return -1;
        }
        if (ATOMICEQ(_writedIndex, _writingIndex)) {
            elog::bug("data not init");
            return -1;
        }

        size_t now = hash_row();
        size_t writed = *_writedIndex;
        size_t realRow =
            (writed < row ? (_mulberry.row - row + writed) % _mulberry.row
                          : writed - row);

        if (row > now) {
            // it's the last row
            elog::bug("row -gt bottom row:", row, " now:", now);
            return -1;
        }

        size_t index = 0, column = 0;
        T val = 0;
        bool ret = true;
        for (column = 0; column < _mulberry.column; column++) {
            index = MAJOR(realRow, column, _mulberry);
            val = *(_mulberry.data + index);
            if (fun != nullptr) {
                ret = fun(val);
            }
            if (ret == false) {
                return -1;
            }
            data->at(column) = val;
        }

        return realRow;
    } /* -----  end of function read  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  fetch
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:  fetch data ptr
     *  --- follow the row change ---
     * ============================================
     */
    T* fetch(const size_t row, const size_t column) const
    {
        if (isnull()) {
            elog::bug("data no init!");
            return nullptr;
        }
        size_t index = 0;

        if (row >= _mulberry.row || column >= _mulberry.column) {
            elog::bug("row must -lt rows, column must -lt column");
            return nullptr;
        }
        if (ATOMICEQ(_writedIndex, _writingIndex)) {
            elog::bug("data not init");
            return nullptr;
        }
        size_t now = hash_row();
        size_t top = *_writedIndex;
        size_t realRow =
            (top < row ? (_mulberry.row - row + top) % _mulberry.row
                       : top - row);

        if (row > now) {
            // it's the last row
            elog::bug("row -gt bottom ");
            return nullptr;
        }

        index = MAJOR(realRow, column, _mulberry);

        return _mulberry.data + index;
    }

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  fetch
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    int fetch(T** t, const size_t row, const size_t column)
    {
        if (isnull()) {
            elog::bug("data no init!");
            return -1;
        }
        *t = this->fetch(row, column);
        if (*t == nullptr) {
            return -1;
        }
        return 0;
    } /* -----  end of function fetch  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  at
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *   --- follow the row change ---
     * ============================================
     */
    T at(const size_t row, const size_t column) const
    {
        if (isnull()) {
            assert("data no init!");
        }
        T* t = fetch(row, column);
        if (t == nullptr) {
            //    assert("at in row, column is nullptr");
            return 0;
        }
        return *t;
    } /* -----  end of function at  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  fixed
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *   ---  fixed data
     * ============================================
     */
    template <typename std::size_t Nc = 10>
    int fixed(std::array<T, Nc>* data, const size_t row)

    {
        if (isnull()) {
            elog::bug("data no init!");
            return -1;
        }

        if (ATOMICEBETWEEN(_tailIndex, row, _writingIndex, _mulberry.row) ==
            false) {
            elog::bug("row must -lt rows, column must -lt column,  row:", row,
                      " mulberry row:", _mulberry.row);
            return -1;
        }
        if (ATOMICEQ(_writedIndex, _writingIndex) == true) {
            elog::bug("atomiceq-> writedIndex:", *_writedIndex,
                      " writingIndex:",
                      _writingIndex->load(std::memory_order_acquire),
                      " tail:", _tailIndex->load(std::memory_order_acquire));
            return -1;
        }

        size_t index = 0, column = 0;

        for (column = 0; column < _mulberry.column; column++) {
            index = MAJOR(row, column, _mulberry);
            data->at(column) = *(_mulberry.data + index);
        }
        return 0;
    } /* -----  end of function fixed  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  hash_row
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *  如果不主动删除 now 会一直等于 _mulberry.row - 1
     *  因为装满了
     * ============================================
     */

    size_t const hash_row() const
    {
        size_t tail = ATOMICSIZT_T(_tailIndex);
        size_t writed = *_writedIndex;
        size_t now =
            writed < tail ? (_mulberry.row - tail + writed) : (writed - tail);

        // elog::info("tail:", tail, " write:", writed);
        return now;
    } /* -----  end of function hash_row  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  Overload
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    void over_load(bool ol)
    {
        _mulberry.overlap = ol;
    } /* -----  end of function Overload  ----- */
    /* ====================  MUTATORS
     * ======================================= */
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  relax
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    void relax()
    {
        size_t s = 0, o = _mulberry.column;
        _scope = {s, o};
    } /* -----  end of function relax  ----- */
    // T *get() { return _mulberry.data[_row]; }
    // const T *get() const { return _mulberry.data[_row]; }

    /* ====================  OPERATORS
     * =======================================
     */

    const SilkPermit<T>& operator[](size_t row)
    {
        elog::echo("[] row:", row);
        _rc = {row, 0};

        return *this;
    }

    const T operator[](int column) const
    {
        elog::echo("[] column:", column);
        return at(_rc.row, column);
    }

    const SilkPermit<T>& operator[](Scope scope)
    {
        _scope = scope;

        return *this;
    }

    // 之后再做
    // void operator[](Coordinate c) { elog::echo("x:", c.x, " y:", c.y); }

    /* SilkPermit operator+(SilkPermit &_other) */
    /* { */
    /*     std::cout << "+ object  \n"; */

    /*     SilkPermit mins; */
    /*     mins[0][0] = _mulberry.data[_row][0] + _other.get()[0]; */
    /*     _row = 0; */
    /*     return mins; */
    /* }; */
    SilkPermit& operator=(const SilkPermit& _other)
    {
        if (this != &_other) {
            released();

            _mulberry = _other._mulberry;

            _mulberry.rw = _ENUM::r;
            _mulberry.callback = nullptr;

            _writedIndex = _other._writedIndex;
            _writingIndex = _other._writingIndex;
            _tailIndex = _other._tailIndex;
        }

        return *this;
    }
    SilkPermit& operator=(const std::shared_ptr<SilkPermit> _other)
    {
        if (this != _other.get()) {
            released();

            _mulberry = _other->_mulberry;

            _mulberry.rw = _ENUM::r;
            _mulberry.callback = nullptr;

            _writedIndex = _other->_writedIndex;
            _writingIndex = _other->_writingIndex;
            _tailIndex = _other->_tailIndex;
        }

        return *this;
    }
    // 以后再开发
    /* SilkPermit &operator=(const T &v) */
    /* { */
    /*     e2q::elog::echo(" == int"); */
    /*     if (isnull()) { */
    /*         elog::bug("data no init!"); */
    /*         return; */
    /*     } */
    /*     if (_mulberry.rw == _ENUM::r) { */
    /*         elog::bug("data is read only"); */
    /*         return; */
    /*     } */

    /*     if (ATOMICEQ(_writedIndex, _writingIndex)) { */
    /*         elog::bug("data not init"); */
    /*         return; */
    /*     } */

    /*     // size_t now = length(); */

    /*     //   a[0][1] = v; */
    /*     return *this; */
    /* } */

protected:
    /* ====================  METHODS =======================================
     */
    void released()
    {
        if (_mulberry.callback != nullptr && _mulberry.rw == _ENUM::rw) {
            _mulberry.callback(_mulberry.first, _mulberry.offset,
                               _mulberry.index);
            /* #ifdef DEBUG */
            /*             elog::echo("release SilkPermit ... rw"); */
            /* #endif */
        }
        _mulberry.callback = nullptr;
        _mulberry.data = nullptr;
        _mulberry.index = 0;
        _mulberry.first = 0;
        _mulberry.offset = 0;
    }
    /* ====================  DATA MEMBERS
     * ======================================= */
    Mulberry<T> _mulberry;

    /**
     * 已经写入的 index
     */
    std::shared_ptr<size_t> _writedIndex = nullptr;
    // std::size_t _writedIndex = 0;
    /**
     * 写入总数
     */
    std::uint64_t _writedTotal = 0;

    /**
     * 下一个准备 写的 index
     */
    std::shared_ptr<std::atomic_size_t> _writingIndex;
    std::shared_ptr<std::atomic_size_t> _tailIndex;

    Scope _scope{0, _mulberry.column};
    Coordinate _rc{0, 0};

    using Mutex = typename Lock::mutex_type;
    /**
     * SilkPermit Mutex;
     */
    // SilkPermit Mutex;
    mutable Mutex SilkPMutex;

private:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */

}; /* -----  end of class SilkPermit  ----- */

/*
 * =====================================================================================
 *        Class:  Silkworm
 *  Description:  内存管理 并且生成 SilkPermit
 * =====================================================================================
 */
template <typename T, typename Lock = BasicLock>
class Silkworm {
public:
    /* ====================  LIFECYCLE
     * =======================================
     */
    Silkworm(std::size_t row, std::size_t column) : _rows(row), _columns(column)
    {
        _silk_tree_ptr = std::make_shared<BinTree<Silk<T>>>();
        _total = _rows * _columns;

        addOneBinTree();
    };
    /* constructor */

    Silkworm(const Silkworm<T>&& _other) { *this = std::move(_other); }

    Silkworm& operator=(const Silkworm<T>& _other)
    {
        if (this != &_other) {
            _columns = _other._columns;
        }

        return *this;
    }

    ~Silkworm()
    {
        Silk<T>* ptr = nullptr;
        // elog::echo("_current_id:", _current_id);
        for (int m = 0; m <= _current_id; m++) {
            int i = _silk_tree_ptr->fetch(&ptr, m);
            if (i == -1 || ptr == nullptr) {
                elog::bug("ptr is null");
                continue;
            }
            if (ptr->data != nullptr) {
                free(ptr->data);
            }
            ptr->data = nullptr;
        }
    }

    /* ====================  ACCESSORS
     * =======================================
     */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  release
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    void release(Mulberry<T>* mul)
    {
        size_t first = mul->first;
        size_t offset = mul->offset;
        size_t index = mul->index;
        release(first, offset, index);
    }

    void release(size_t first, size_t offset, size_t index)
    {
        if (offset <= 0) {
            return;
        }
        Lock _lock(SilkMutex);

        Silk<T>* ptr = nullptr;
        int i = _silk_tree_ptr->fetch(&ptr, index);
        if (i == -1 || ptr == nullptr) {
            elog::bug("ptr is null");
            return;
        }

        std::size_t id = 0;
        for (SilkType p : ptr->await_list) {
            if (p.second == 0) {
                continue;
            }
            if (first == (p.first + p.second)) {
                ptr->await_list.at(id).second += offset;

                break;
            }
            else if ((first + offset) == p.first) {
                ptr->await_list.at(id).first = first;
                ptr->await_list.at(id).second += offset;

                break;
            }

            id++;
        }

        if (id == ptr->await_list.size()) {
            size_t m = 0;
            for (size_t m = 0; m < ptr->await_list.size(); m++) {
                if (ptr->await_list.at(m).second == 0) {
                    ptr->await_list.at(m).first = first;
                    ptr->await_list.at(m).second = offset;
                    break;
                }
            }
            if (m == ptr->await_list.size()) {
                std::pair<std::size_t, std::size_t> new_p =
                    std::make_pair(first, offset);

                ptr->await_list.push_back(new_p);
            }
        }

    } /* -----  end of function release  ----- */
    /* ====================  MUTATORS
     * ======================================= */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  make_shared
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    template <typename SUBCLASS,
              typename = typename std::enable_if<
                  is_base_template_of<SilkPermit, SUBCLASS>::value>::type>
    std::shared_ptr<SUBCLASS> make_share(size_t column)
    {
        Mulberry<T> mulberry;
        BuildMulbeery(mulberry, _rows, column);
        return std::make_shared<SUBCLASS>(mulberry);
    }

    template <typename SUBCLASS,
              typename = typename std::enable_if<
                  is_base_template_of<SilkPermit, SUBCLASS>::value>::type>
    std::shared_ptr<SUBCLASS> make_share(size_t row, size_t column)
    {
        Mulberry<T> mulberry;
        BuildMulbeery(mulberry, row, column);
        return std::make_shared<SUBCLASS>(mulberry);
    } /* -----  end of function make_shared  ----- */
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  apply
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    template <typename SUBCLASS,
              typename = typename std::enable_if<
                  is_base_template_of<SilkPermit, SUBCLASS>::value>::type>
    SUBCLASS apply(size_t row, size_t column)
    {
        Mulberry<T> mulberry;

        BuildMulbeery(mulberry, row, column);
        return SUBCLASS(mulberry);
    }

    template <typename SUBCLASS,
              typename = typename std::enable_if<
                  is_base_template_of<SilkPermit, SUBCLASS>::value>::type>
    SUBCLASS apply(const size_t column)
    {
        Mulberry<T> mulberry;
        BuildMulbeery(mulberry, _rows, column);
        return SUBCLASS(mulberry);
    } /* -----  end of function apply  ----- */

    /* ====================  OPERATORS
     * =======================================
     */

protected:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS =======================================
     */
    void BuildMulbeery(Mulberry<T>& mulberry, const size_t row,
                       const size_t column)
    {
        // Mulberry<T> mulberry;
        // if (mulberry.row == 0 || mulberry.column == 0) {
        mulberry.row = row;
        mulberry.column = column;
        // }
        mulberry.overlap = true;

        mulberry.data = nullptr;
        mulberry.rw = _ENUM::rw;
        mulberry.major = _ENUM::col;
        size_t need_total = row * column;

        assert(need_total <= _total && "row * column is too large");
        Lock _lock(SilkMutex);
        int n = ranking(&mulberry, _current_id, need_total);

        assert(n >= 0 && " not found mulbeery! ");

        mulberry.callback = [this](size_t a, size_t b, size_t c) {
            this->release(a, b, c);
        };

        //  return mulberry;
    }
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  reset
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    void reset(Silk<T>* ptr)
    {
        if (ptr->await_list.size() <= 1) {
            return;
        }
        std::sort(ptr->await_list.begin(), ptr->await_list.end(),
                  [](SilkType a, SilkType b) { return a.first < b.first; });
        for (std::size_t u = 1; u < ptr->await_list.size(); u++) {
            std::size_t pre_id = u - 1;
            std::size_t pre_val = ptr->await_list.at(pre_id).first +
                                  ptr->await_list.at(pre_id).second;
            if (pre_val == 0) {
                continue;
            }
            if (pre_val == ptr->await_list.at(u).first) {
                ptr->await_list.at(u).first = ptr->await_list.at(pre_id).first;
                ptr->await_list.at(u).second +=
                    ptr->await_list.at(pre_id).second;
                ptr->await_list.at(pre_id).first = 0;
                ptr->await_list.at(pre_id).second = 0;
            }
        }

    } /* -----  end of function reset  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  ranking
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    int ranking(Mulberry<T>* mul, int start_id, std::size_t need_total)
    {
        int id = -1, next = 0;
        int run_id = start_id;
        Silk<T>* ptr = nullptr;
        for (; run_id <= _current_id; run_id++) {
            int i = _silk_tree_ptr->fetch(&ptr, run_id);
            id = -1;
            if (i == -1 || ptr == nullptr) {
                continue;
            }

            for (SilkType p : ptr->await_list) {
                if (p.second >= need_total) {
                    if (id == -1) {
                        id = next;
                    }
                    if (ptr->await_list.at(id).second > p.second) {
                        id = next;
                    }
                }
                next++;
            }
            if (id > -1) {
                mul->index = id;
                mul->first = ptr->await_list.at(id).first;
                mul->offset = need_total;

                // not allow read not init data
                mul->data = ptr->data + mul->first;

                ptr->await_list.at(id).first += need_total;
                ptr->await_list.at(id).second -= need_total;
            }
        }
        if (id == -1) {
            elog::info("new bin tree");
            addOneBinTree();
            run_id = ranking(mul, _current_id, need_total);
        }

        return run_id;

    } /* -----  end of function ranking  ----- */
    void addOneBinTree()
    {
        Silk<T> silk;
        SilkType await_pair = std::make_pair(0, _total);
        size_t msize = sizeof(T) * _total;

        silk.data = (T*)malloc(msize);
        silk.await_list.push_back(await_pair);

        _current_id = _silk_tree_ptr->push(silk);
    }
    /* ====================  DATA MEMBERS
     * ======================================= */
    size_t _rows = 0;
    size_t _columns = 0;
    size_t _offset = 0;  // ptr offset
    std::shared_ptr<BinTree<Silk<T>>> _silk_tree_ptr = nullptr;

    int _current_id = 0;
    size_t _total;

    using Mutex = typename Lock::mutex_type;

    mutable Mutex SilkMutex;
}; /* -----  end of class Silkworm  ----- */

}  // namespace e2q
#endif /* ----- #ifndef SILKWORM_INC  ----- */
