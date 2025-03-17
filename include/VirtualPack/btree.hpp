/*
 * =====================================================================================
 *
 *       Filename:  btree.hpp
 *
 *    Description:  btree
 *
 *        Version:  1.0
 *        Created:  2024年03月19日 10时04分16秒
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

#ifndef BTREE_INC
#define BTREE_INC
#include <array>
#include <cstddef>
#include <functional>
#include <queue>
#include <utility>

#include "VirtualPack/foreign.hpp"
#include "assembler/BaseType.hpp"
namespace e2q {

typedef struct CompareItem BItem;

enum __ROTATE {
    _RANDOM = 0,
    _LEFT = 1,
    _RIGHT = 2
}; /* ----------  end of enum __ROTATE  ---------- */

typedef enum __ROTATE RoType;
/*
 * ================================
 *        Class:  Btree_node
 *  Description:
 *typename = typename std::enable_if<std::is_base_of_v<BItem, T>>::type
 * ================================
 */
template <typename T>
class Btree_node {
public:
    // typedef typename T::reference reference;
    /* =============  LIFECYCLE     =================== */
    Btree_node(){}; /* constructor */

    /* =============  ACCESSORS     =================== */

    struct base_fields {
        // A pointer to the node's parent.
        Btree_node *parent = nullptr;
        // A boolean indicating whether the node is a leaf or not.
        size_t leaf = 0;
        // node value
        std::queue<T> data;

    }; /* ----------  end of struct base_fields  ---------- */

    struct root_fields : public base_fields {
        Btree_node *right = nullptr;
        Btree_node *left = nullptr;

        size_t depth = 0;
    }; /* ----------  end of struct root_fields  ---------- */

    /* =============  MUTATORS      =================== */

    T &data() { return _fields.data.front(); }
    const T data() const { return _fields.data.front(); }
    const std::size_t length() const { return _fields.data.size(); }
    const bool empty() const { return _fields.data.empty(); }
    T erase()
    {
        T s = _fields.data.front();
        _fields.data.pop();
        return s;
    }

    void decrement() { _fields.depth--; }
    void increment() { _fields.depth++; }
    size_t depth() const { return _fields.depth; }
    void fill(const T &data) { _fields.data.push(data); };

    void leaf(size_t t) { _fields.leaf = t; }
    std::size_t leafs() const { return _fields.leaf; }

    bool leaf() const
    {
        if (_fields.right == nullptr && _fields.left == nullptr) {
            return false;
        }
        else {
            return true;
        }
    }
    Btree_node *parent() { return _fields.parent; }
    const Btree_node *parent() const { return _fields.parent; }

    void parent(Btree_node *p) { _fields.parent = std::move(p); }

    void root() { _fields.depth = 0; }
    bool is_root() const { return _fields.depth == 0; }

    void right(Btree_node *node)
    {
        if (node == nullptr) {
            _fields.right = nullptr;
            _fields.leaf--;
        }
        else {
            _fields.right = std::move(node);
            _fields.right->increment();
            _fields.leaf++;
        }
    }

    void left(Btree_node *node)
    {
        if (node == nullptr) {
            _fields.left = nullptr;
            _fields.leaf--;
        }
        else {
            _fields.left = std::move(node);
            _fields.left->increment();
            _fields.leaf++;
        }
    }

    Btree_node **right() { return &_fields.right; }
    Btree_node **left() { return &_fields.left; }

    Btree_node *rightmost() { return _fields.right; }
    Btree_node *leftmost() { return _fields.left; }

    const Btree_node *rightmost() const { return _fields.right; }
    const Btree_node *leftmost() const { return _fields.left; }

    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

    root_fields _fields;
}; /* -----  end of class Btree_node  ----- */

/*
 * ================================
 *        Class:  Btree
 *  Description:
 * ================================
 */
template <typename T, typename compare = std::less<T>,
          typename Lock = e2q::BasicLock>
class Btree {
    typedef Btree_node<T> node_type;
    typedef Btree<T, compare> self_type;

public:
    /* =============  LIFECYCLE     =================== */
    Btree(const compare &comp = compare())
        : _compare(comp),
          _root(nullptr){

          }; /* constructor */

    ~Btree() { clear(); }
    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */
    void find(const T &item);
    bool insert(const T &item);

    T erase();
    void clear();

    void lower_bound();
    void upper_bound();

    bool empty() const { return _root == nullptr; }
    T *root() { return _root; }
    void dump();

    std::array<std::size_t, 4> leaf_number();
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */
    T erase(node_type *node);

    bool internal_insert(node_type **node, node_type *parent, const T &item,
                         RoType r);

    void upper_bound(const node_type *);

    node_type *one_node(const T &item)
    {
        node_type *nn = MALLOC(Btree_node<T>);
        nn->fill(item);
        return nn;
    }

    void internal_clear(node_type **);
    void dump(const std::string &prefix, const node_type *node, bool isLeft);
    /* =============  DATA MEMBERS  =================== */
    compare _compare;
    node_type *_root;

    using Mutex = typename Lock::mutex_type;
    /**
     * Btree Mutex;
     */
    mutable Mutex BtreeMutex;

    /**
     * 0 - right
     * 1 - left
     * 2 - full
     * 3 - one
     */
    std::array<std::size_t, 4> _number{0, 0, 0, 0};

}; /* -----  end of class Btree  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Btree<T,compare>::insert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
bool Btree<T, compare, Lock>::insert(const T &item)
{
    // std::cout << "item:" << item << std::endl;
    Lock _lock(BtreeMutex);
    if (_root == nullptr) {
        _root = one_node(item);
        return true;
    }

    node_type **cur = &_root;
    // insert(cur, _root, item);
    internal_insert(cur, nullptr, item, RoType::_RANDOM);
    return false;
} /* -----  end of function Btree<T,compare>::insert  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Btree<T,compare>::internal_insert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
bool Btree<T, compare, Lock>::internal_insert(node_type **node,
                                              node_type *parent, const T &item,
                                              RoType r)
{
    node_type *current = *node;
    node_type *left;
    node_type *right;
    node_type *nn = nullptr;
    bool ret = false;
    if (*node == nullptr) {
        log::bug("node is nullptr");
        return ret;
    }
    // right
    if (_compare(current->data(), item)) {
        // 1. current > item
        if (current->rightmost() == nullptr) {
#ifdef DEBUG1
            log::info("-right---1---");
#endif
            nn = one_node(item);
            current->right(nn);
            ret = true;
        }
        else if (_compare(item, current->rightmost()->data()) &&
                 current->rightmost()->leftmost() == nullptr) {
// 2. item > current->right
#ifdef DEBUG1
            log::info("-right---2---");
#endif
            nn = one_node(item);

            nn->right(current->rightmost());
            current->right(nullptr);
            nn->left(current);

            if (parent == nullptr) {
                _root = std::move(nn);
            }
            else {
                if (parent->rightmost() == current) {
                    parent->right(nn);
                }
                else {
                    parent->left(nn);
                }
            }
        }
        else if (parent != nullptr &&
                 _compare(current->rightmost()->data(), item) &&
                 current->rightmost()->leftmost() == nullptr &&
                 current->rightmost()->rightmost() != nullptr) {
            // 3. rotate
            //  current > item  and current->right > item
#ifdef DEBUG1
            log::info("- rotate right--3--- r:", r);
#endif
            right = std::move(current->rightmost());
            current->right(nullptr);

            if (right == nullptr) {
                log::bug("right is nullptr");
                return false;
            }

            if (parent->rightmost() == current) {
                parent->right(right);
            }
            else {
                parent->left(right);
            }

            right->left(current);

            if (right->rightmost() == nullptr) {
                nn = one_node(item);
                right->right(nn);
                ret = true;
            }
            else {
                //  log::echo("=====");
                ret = internal_insert(right->right(), right, item,
                                      RoType::_RIGHT);
            }
        }
        else {
            ret = internal_insert(current->right(), current, item, r);
        }
    }
    // left
    else if (_compare(item, current->data())) {
        // item > current
        if (current->leftmost() == nullptr) {
#ifdef DEBUG1
            log::info("-left--1----");
#endif
            nn = one_node(item);
            current->left(nn);
            ret = true;
        }
        else if (_compare(current->leftmost()->data(), item) &&
                 current->leftmost()->rightmost() == nullptr) {
            // 2.  current->left > item
#ifdef DEBUG1
            log::info("-left--2----");
#endif
            nn = one_node(item);
            nn->left(current->leftmost());
            current->left(nullptr);
            nn->right(current);

            if (parent == nullptr) {
                _root = std::move(nn);
            }
            else {
                if (parent->rightmost() == current) {
                    parent->right(nn);
                }
                else {
                    parent->left(nn);
                }
            }
        }
        else if (parent != nullptr &&
                 _compare(item, current->leftmost()->data()) &&
                 current->leftmost()->rightmost() == nullptr &&
                 current->leftmost()->leftmost() != nullptr) {
#ifdef DEBUG1
            log::info("-rotate - left--3----");
#endif
            // 3. rotate
            left = std::move(current->leftmost());
            current->left(nullptr);

            if (left == nullptr) {
                log::bug("left is nullptr");
                return false;
            }
            // 当前是 parent 的左边还是右边
            if (parent->rightmost() == current) {
                parent->right(left);
            }
            else {
                parent->left(left);
            }

            left->right(current);

            if (left->leftmost() == nullptr) {
#ifdef DEBUG1
                log::info("-left--31----");
#endif
                nn = one_node(item);
                left->left(nn);
                ret = true;
            }
            else {
                //  log::info("-left--32----");
                ret = internal_insert(left->left(), left, item, RoType::_LEFT);
            }
        }
        else {
            ret = internal_insert(current->left(), current, item, r);
        }
    }
    else {
        // item == current
        current->fill(item);
        ret = true;
    }
    return ret;
} /* -----  end of function Btree<T,compare>::internal_insert  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Btree<T,compare>::lower_bound
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
void Btree<T, compare, Lock>::lower_bound()
{
} /* -----  end of function Btree<T,compare>::lower_bound  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Btree<T,compare>::upper_bound
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
void Btree<T, compare, Lock>::upper_bound()
{
    if (_root == nullptr) {
        return;
    }
    upper_bound(_root->leftmost());
    std::cout << ":[" << _root->length() << "]" << _root->data();
    upper_bound(_root->rightmost());
    std::cout << "\n";
} /* -----  end of function Btree<T,compare>::upper_bound  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Btree<T,compare>::upper_bound
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
void Btree<T, compare, Lock>::upper_bound(const node_type *node)
{
    if (node == nullptr) {
        return;
    }

    upper_bound(node->leftmost());
    std::cout << ":[" << node->length() << "]" << node->data();
    upper_bound(node->rightmost());

} /* -----  end of function Btree<T,compare>::upper_bound  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Btree<T,compare>::dump
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
void Btree<T, compare, Lock>::dump()
{
    Lock _lock(BtreeMutex);

    _number = {0, 0, 0, 0};
    dump("", _root, false);
} /* -----  end of function Btree<T,compare>::dump  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Btree<T,compare>::dump
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
void Btree<T, compare, Lock>::dump(const std::string &prefix,
                                   const node_type *node, bool isLeft)
{
    if (node != nullptr) {
        std::cout << prefix;
        std::cout << (isLeft ? "|--" : "L--");
        // print the value of the node
        if (node->empty()) {
            std::cout << "(" << node->depth() << ") 0" << std::endl;
        }
        else {
            std::cout << "(" << node->depth() << ") " << node->data()
                      << std::endl;
        }
        if (node->rightmost() != nullptr) {
            _number[0]++;
        }
        if (node->leftmost() != nullptr) {
            _number[1]++;
        }
        if (node->rightmost() != nullptr && node->leftmost() != nullptr) {
            _number[2]++;
        }
        else if (node->rightmost() != nullptr || node->leftmost() != nullptr) {
            _number[3]++;
        }
        // enter the next tree level - left and right branch
        dump(prefix + (isLeft ? "|   " : "    "), node->rightmost(), true);
        dump(prefix + (isLeft ? "|   " : "    "), node->leftmost(), false);
    }

} /* -----  end of function Btree<T,compare>::dump  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Btree<T,compare>::erase
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
T Btree<T, compare, Lock>::erase()
{
    Lock _lock(BtreeMutex);

    T ret = erase(_root);
    if (_root->empty()) {
        node_type **right = _root->right();
        RELEASE(_root);

        if (*right == nullptr) {
            return ret;
        }
        _root = *right;
        _root->root();
    }

    return ret;
} /* -----  end of function Btree<T,compare>::erase  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Btree<T,compare>::erase
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
T Btree<T, compare, Lock>::erase(node_type *node)
{
    T ret;
    node_type **cur;
    if (node->leftmost() != nullptr) {
        ret = erase(node->leftmost());

        if (node->leftmost()->length() == 0 &&
            node->leftmost()->leaf() == false) {
            cur = node->left();
            RELEASE(*cur);
            node->left(nullptr);
        }
        return ret;
    }
    if (node->empty() == false) {
        ret = node->erase();
        return ret;
    }

    if (node->rightmost() != nullptr) {
        ret = erase(node->rightmost());

        if (node->rightmost()->empty()) {
            node_type **right = node->rightmost()->right();

            cur = node->right();

            RELEASE(*cur);
            node->right(*right);
        }
    }

    return ret;
} /* -----  end of function Btree<T,compare>::erase  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Btree<T,compare>::clear
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
void Btree<T, compare, Lock>::clear()
{
    internal_clear(&_root);
} /* -----  end of function Btree<T,compare>::clear  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Btree<T,compare>::internal_clear
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
void Btree<T, compare, Lock>::internal_clear(node_type **cur)
{
    node_type *node = (*cur);
    if (node != nullptr) {
        if (node->leftmost() != nullptr) {
            internal_clear(node->left());
            node->left(nullptr);
        }

        if (node->rightmost() != nullptr) {
            internal_clear(node->right());
            node->right(nullptr);
        }

        if (node->leaf() == false) {
            RELEASE(*cur);
        }
        else {
            log::bug("error clear");
        }
    }
} /* -----  end of function Btree<T,compare>::internal_clear  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Btree<T,compare,Lock>::leaf_number
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>

std::array<std::size_t, 4> Btree<T, compare, Lock>::leaf_number()
{
    return _number;
} /* -----  end of function Btree<T,compare,Lock>::leaf_number  ----- */
}  // namespace e2q
#endif /* ----- #ifndef BTREE_INC  ----- */
