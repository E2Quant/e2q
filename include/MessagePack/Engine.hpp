/*
 * =====================================================================================
 *
 *       Filename:  Engine.hpp
 *
 *    Description:  Engine
 *
 *        Version:  1.0
 *        Created:  2024年05月19日 18时18分25秒
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

#ifndef ENGINE_INC
#define ENGINE_INC
#include <cstddef>
#include <functional>
#include <queue>
#include <type_traits>
#include <utility>
#include <vector>

#include "MessagePack/foreign.hpp"
#include "assembler/BaseType.hpp"
namespace e2q {
/**
 * QuantCup 1:   Price-Time Matching Engin
 *
 * pending
 */
/**
 * 对手盘 撮合， closeOrder 是 Simulation 系统产生的模拟 order
 * 采用一笔交易，降低复杂性，以后有机会优化再看看多笔交易的情况
 * 如果价格达到了，一次性把一个  level 交易完
 * 如果不撮合整个 level 就会出现不可控的偶然性
 *
 * 或者自动生成 多空 单
 */
struct LeItem : public CompareItem {
    //  LeItem &operator+=(LeItem &rhs);
    // LeItem &operator-=(LeItem &rhs);

    // bool operator>=(const LeItem &rhs) const;

    bool volatility();
    e2::Int_e value() const;
    long getOpenQuantity() const;
    bool check() const;
    LeItem *next = nullptr;
}; /* ----------  end of struct LeItem  ---------- */

template <typename T,
          typename =
              typename std::enable_if<std::is_base_of_v<LeItem, T>>::type>
struct LinkEntry {
    struct LinkEntry *pre = nullptr;
    struct LinkEntry *next = nullptr;

    LinkEntry(std::size_t i = 0) : _level(i) {}
    ~LinkEntry()
    {
        // elog::echo("LinkEntry level:", _level, " abndons:", _abandons);
    }
    std::size_t level() { return _level; }
    bool abandon()
    {
        if (_link_header == nullptr) {
            _abandons++;
        }
        return _link_header == nullptr;
    };
    void add(T *t)
    {
        if (_link_header == nullptr) {
            _link_worker = _link_header = t;
        }
        else {
            _link_header->next = t;
            _link_header = t;
        }

        _depth++;
    }
    const T *data() { return _link_worker; };
    std::size_t depth() { return _depth; }
    bool volat()
    {
        if (_link_worker == nullptr) {
            return true;
        }
        return _link_worker->volatility();
    }
    bool has_next()
    {
        return _link_worker == nullptr || _link_worker == _link_header;
    }
    T *leave()
    {
        if (_link_worker != nullptr && _link_worker->volatility()) {
            _link_worker = _link_worker->next;
            _depth--;

            if (_link_worker == nullptr) {
                _link_header = nullptr;
            }
        }
        return _link_worker;
    }
    void clear() { _link_header = _link_worker = nullptr; }

private:
    std::size_t _depth = 0;  // Book depth _data.size()
    T *_link_worker = nullptr;
    T *_link_header = nullptr;

    std::size_t _abandons = 0;
    std::size_t _level = 0;  // Price levels
}; /* ----------  end of struct LinkEntry  ---------- */

/**
 *
 *
 *
 */
template <typename T>
struct PointLinkList {
    PointLinkList()
    {
        _head = MALLOC(LinkEntry<T>);
        _head->next = _head;
        _head->pre = _head;
        _tail = _head;
    }

    ~PointLinkList()
    {
        // elog::echo("release PointLinkList!");
        struct LinkEntry<T> *_del = _head;
        struct LinkEntry<T> *_tmp = nullptr;
        while (_del != nullptr) {
            if (_del->next) {
                _tmp = _del->next;
            }
            _del->clear();
            RELEASE(_del);
            if (_tmp == _head) {
                break;
            }
            _del = _tmp;
        }
    }
    LinkEntry<T> **root() { return &_head; }
    LinkEntry<T> **end() { return &_tail; }

    LinkEntry<T> *root_const() const { return _head; }
    LinkEntry<T> *end_const() const { return _tail; }
    std::size_t length() { return _size; };
    std::size_t depth() { return _head->depth(); }
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  shift
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    void shift()
    {
        _head = _tail;
        _tail = _tail->pre;
    } /* -----  end of function shift  ----- */
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  head
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    bool head()
    {
        return follow(&_head);
    } /* -----  end of function head  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  tail
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    bool tail()
    {
        LinkEntry<T> *_add = MALLOC(LinkEntry<T>, _size);
        if (_add == nullptr) {
            elog::bug("LinkEntry new is nullptr!");
            return false;
        }
        _tail->next->pre = _add;

        _add->next = _tail->next;
        _add->pre = _tail;

        _tail->next = _add;
        _tail = _add;
        return true;
    } /* -----  end of function tail  ----- */
      /*
       * ===  FUNCTION  =============================
       *
       *         Name:  follow
       *  ->  void *
       *  Parameters:
       *  - size_t  arg
       *  Description:
       *
       * ============================================
       */
    bool follow(LinkEntry<T> **pf)
    {
        LinkEntry<T> *p = *pf;
        LinkEntry<T> *_add = MALLOC(LinkEntry<T>, _size);
        if (_add == nullptr) {
            elog::bug("LinkEntry new is nullptr!");
            return false;
        }
        if (_size == 1) {
            _add->next = p;
            _add->pre = p;
            p->pre = _add;
            p->next = _add;
        }
        else {
            p->pre->next = _add;

            _add->next = p;
            _add->pre = p->pre;

            p->pre = _add;
        }
        *pf = _add;

        _size++;
        return true;
    } /* -----  end of function follow  ----- */

private:
    struct LinkEntry<T> *_head = nullptr;
    struct LinkEntry<T> *_tail = nullptr;

    std::size_t _size = 1;

}; /* ----------  end of struct PointLinkList  ---------- */

/*
 * ================================
 *        Class:  Engine
 *  Description:
 * ================================
 */
template <typename T, typename compare = std::greater<T>,
          typename Lock = e2q::BasicLock>
class Engine {
    typedef PointLinkList<T> node_type;

public:
    /* =============  LIFECYCLE     =================== */
    Engine()
    {
        _node = MALLOC(node_type);

        /*
         * init node
         * 后以再优化这个
         *
         */
        _node->head();
        _worker = _node->root();
    } /* constructor */
    ~Engine() { RELEASE(_node); }

    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */
    bool insert(T *);

    std::vector<T> fetch(T &);

    void roll()
    {
        if (*_worker == nullptr) {
            elog::info("worker is nullptr");

            return;
        }
        (*_worker)->leave();
    }

    T *spread()
    {
        if (*_worker == nullptr) {
            elog::info("worker is nullptr");
            return nullptr;
        }
        T *tmp = (*_worker)->leave();
        if (tmp == nullptr) {
            (*_worker) = (*_worker)->next;
        }

        return tmp;
    }

    bool empty()
    {
        bool ept = (*_worker == nullptr || _node == nullptr);
        if (ept) {
            return true;
        }

        ept = (*_worker)->has_next() && (*_worker)->volat();

        if (ept) {
            if ((*_worker) != _node->end_const()) {
                ept = false;
            }
        }

        return ept;
    }

    std::size_t size() { return _node->length(); }
    std::size_t depth() { return _node->depth(); }
    std::pair<e2::Int_e, long> topValue()
    {
        LinkEntry<T> *_dump = _node->root_const();
        std::pair<e2::Int_e, long> v{0, 0};
        if (_dump == nullptr) {
            return v;
        }
        const T *data = _dump->data();
        if (data == nullptr) {
            return v;
        }
        if (data->check()) {
            return v;
        }
        v = std::make_pair(data->value(), data->getOpenQuantity());
        return v;
    }
    void const dump() const;
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */
    bool internal_insert(LinkEntry<T> **, T *);
    /* =============  DATA MEMBERS  =================== */

    node_type *_node = nullptr;
    LinkEntry<T> **_worker = nullptr;

    compare _compare;

    using Mutex = typename Lock::mutex_type;
    /**
     * Engine Mutex;
     */
    mutable Mutex EMutex;
}; /* -----  end of class Engine  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Engine::insert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
bool Engine<T, compare, Lock>::insert(T *t)
{
    Lock _lock(EMutex);
    roll();
    bool ret = internal_insert(_node->root(), t);

    return ret;
} /* -----  end of function Engine::insert  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Engine<T,compare,Lock>::internal_insert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
bool Engine<T, compare, Lock>::internal_insert(LinkEntry<T> **work, T *t)
{
    LinkEntry<T> *worker = *work;
    if (worker->abandon()) {
        worker->add(t);
        return true;
    }
    if (_compare(*worker->data(), *t)) {
        // work->data > t

        if (worker == _node->end_const()) {
            // work == end
            if (_node->root_const()->abandon()) {
                _node->shift();
            }
            else {
                _node->tail();
            }
            if (_node->end_const()->abandon()) {
                (*_node->end())->add(t);
            }

            return true;
        }
        else {
            worker = worker->next;
            internal_insert(&worker, t);
        }
    }
    else if (_compare(*t, *worker->data())) {
        //   t > work->data

        _node->follow(work);

        if ((*work)->abandon()) {
            (*work)->add(t);
        }

        return true;
    }

    else {
        // t == work->data
        worker->add(t);

        return true;
    }
    return false;
} /* -----  end of function Engine<T,compare,Lock>::internal_insert  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Engine<T,compare,Lock>::fetch
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
std::vector<T> Engine<T, compare, Lock>::fetch(T &t)
{
    LinkEntry<T> *work = *_worker;
    std::vector<T> ret = work->leave(t);

    /* T leave = work->data - t; */
    /* ret.push_back(leave); */
    return ret;
} /* -----  end of function Engine<T,compare,Lock>::fetch  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Engine<T,compare,Lock>::dump
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
template <typename T, typename compare, typename Lock>
void const Engine<T, compare, Lock>::dump() const
{
    LinkEntry<T> *_dump = _node->root_const();
    if (_dump == nullptr) {
        elog::info("node is nullptr");
        return;
    }
    std::size_t level = 0;
    do {
        const T *data = _dump->data();

        if (data != nullptr) {
            std::cout << "level:" << level << " level no.: " << _dump->level()
                      << " depth:" << _dump->depth() << " -- " << data->value()
                      << " orderPending:" << (*data) << std::endl;
        }
        level++;
        _dump = _dump->next;
    } while (_dump != _node->root_const());
} /* -----  end of function Engine<T,compare,Lock>::dump  ----- */
}  // namespace e2q
#endif /* ----- #ifndef ENGINE_INC  ----- */
