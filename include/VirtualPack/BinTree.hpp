/*
 * =====================================================================================
 *
 *       Filename:  bin_tree.hpp
 *
 *    Description:  bin_tree
 *
 *        Version:  1.0
 *        Created:  2023年08月03日 09时49分55秒
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

#ifndef BIN_TREE_INC
#define BIN_TREE_INC
#include <atomic>
#include <bitset>
#include <cstddef>
#include <iostream>
#include <limits>
#include <memory>
#include <mutex>
#include <utility>

#include "VirtualPack/foreign.hpp"
namespace e2q {

/*
 * =====================================================================================
 *        Class:  BinTree
 *  Description:
 * =====================================================================================
 */
template <std::size_t N = 10>
using bintype = std::bitset<N>;

template <typename T, std::size_t N = 10>
struct __bTree {
    bintype<N> bin;
    T data;
    std::shared_ptr<struct __bTree> rnext;
    std::shared_ptr<struct __bTree> lnext;
}; /* ----------  end of struct bTree  ---------- */

template <typename T, std::size_t N = 10>
using BTree = struct __bTree<T, N>;

/**
 * 二进制的二叉树
 */
template <typename T, std::size_t N = 10, typename Lock = e2q::BasicLock>
class BinTree {
public:
    /* ====================  LIFECYCLE =======================================
     */
    BinTree() { _id = 0; }; /* constructor */
    ~BinTree(){};
    /* ====================  ACCESSORS =======================================
     */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  push
     *  ->  int
     *  Parameters:
     *  - const  T& data
     *  Description:
     *
     * ============================================
     */
    int push(const T& data)
    {
        Lock _lock(treeMutex);

        const bintype<N> b{_id};
        if (b.count() == b.size()) {
            return -1;
        }
        _id++;
        std::shared_ptr<BTree<T, N>> btptr = std::make_shared<BTree<T, N>>();
        btptr->bin = b;
        btptr->data = std::move(data);

        if (_btree_ptr == nullptr) {
            _btree_ptr = std::move(btptr);
            return b.to_ulong();
        }

        BTree<T, N>* ptr = _btree_ptr.get();
        int m = b.size();
        bool start = false;

        while (m >= 0) {
            if (b[m] && start == false) {
                start = true;
            }
            if (start) {
                if (b[m]) {
                    if (ptr->lnext == nullptr) {
                        ptr->lnext = std::move(btptr);

                        return b.to_ulong();
                    }
                    else {
                        ptr = ptr->lnext.get();
                    }
                }
                else {
                    if (ptr->rnext == nullptr) {
                        ptr->rnext = std::move(btptr);
                        return b.to_ulong();
                    }
                    else {
                        ptr = ptr->rnext.get();
                    }
                }
            }

            if (m == 0) {
                std::cout << "same id\n";
                break;
            }
            m--;
        }
        return -1;
    } /* -----  end of function push  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  fetch
     *  ->  int
     *  Parameters:
     *  - BTree<T, N>** data
     *  - size_t  id
     *  Description:
     *
     * ============================================
     */

    int fetch(T** data, std::size_t id)
    {
        const bintype<N> b{id};
        BTree<T, N>* ptr = _btree_ptr.get();
        if (ptr == nullptr) {
            log::echo("ptr == nullptr");
            return -1;
        }
        int m = b.size();
        bool start = false;

        if (b == 0 && ptr->bin.to_ullong() == id) {
            *data = &ptr->data;
            return 0;
        }
        while (m >= 0) {
            if (b[m] && start == false) {
                start = true;
            }
            if (start) {
                if (b[m]) {
                    if (ptr->lnext == nullptr) {
                        return -1;
                    }
                    else {
                        ptr = ptr->lnext.get();
                    }
                }
                else {
                    if (ptr->rnext == nullptr) {
                        return -1;
                    }
                    else {
                        ptr = ptr->rnext.get();
                    }
                }

                if (ptr->bin.to_ullong() == id) {
                    *data = &ptr->data;

                    return 0;
                }
            }
            if (m == 0) {
                break;
            }
            m--;
        }
        log::echo("empty");

        return -1;
    } /* -----  end of function fetch  ----- */
      /*
       * ===  FUNCTION  =============================
       *
       *         Name:  fetch
       *  ->  int
       *  Parameters:
       *  - T& data
       *  - size_t  id
       *  Description:
       *
       * ============================================
       */

    int fetch(T& data, std::size_t id)
    {
        T* ptr = nullptr;

        int r = fetch(&ptr, id);
        if (r == 0) {
            data = std::move(*ptr);
        }

        return r;
    } /* -----  end of function fetch  ----- */

    /* ====================  MUTATORS
       ======================================= */
    void printf() { printf(_btree_ptr.get()); }
    std::size_t id() { return _id.load(std::memory_order_acquire); }
    /* ====================  OPERATORS
     * =======================================
     */

protected:
    /* ===========identifier=========  METHODS
     * =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS =======================================
     */
    void printf(const BTree<T, N>* ptr)
    {
        if (ptr == nullptr) {
            return;
        }
        std::cout << "bin:" << ptr->bin << std::endl;

        BTree<T, N>* left = ptr->lnext.get();
        std::cout << "left" << std::endl;
        printf(left);
        BTree<T, N>* right = ptr->rnext.get();
        std::cout << "right" << std::endl;
        printf(right);
    }
    /* ====================  DATA MEMBERS
     * ======================================= */
    std::shared_ptr<BTree<T, N>> _btree_ptr = nullptr;
    std::atomic_size_t _id;

    using Mutex = typename Lock::mutex_type;

    mutable Mutex treeMutex;
}; /* -----  end of class BinTree  ----- */

}  // namespace e2q
#endif /* ----- #ifndef BIN_TREE_INC  ----- */
