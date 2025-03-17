/*
 * =====================================================================================
 *
 *       Filename:  Disruptor.hpp
 *
 *    Description:  Disruptor
 *
 *        Version:  1.0
 *        Created:  2023年09月04日 10时53分31秒
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

#ifndef DISRUPTOR_INC
#define DISRUPTOR_INC
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "MessagePack/foreign.hpp"
namespace e2q {

typedef std::shared_ptr<Trigger> trigger_ptr;

typedef std::function<void(SeqType start, SeqType end)> wait_for_t;

class eof : public std::exception {
public:
    virtual const char *what() const noexcept { return "eof"; }
};

/*
 * ================================
 *        Class:  Sequence
 *  Description:
 * ================================
 */
class Sequence {
public:
    /* =============  LIFECYCLE     =================== */
    Sequence(); /* constructor */

    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */

    SeqType aquire();
    void store(SeqType value);
    SeqType atomic_increment_and_get(SeqType inc);
    bool alert();
    SeqType status();
    void alert(SeqType);

    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    atomic_seqtype _sequence;
    atomic_seqtype _alert;
}; /* -----  end of class Sequence  ----- */

typedef std::shared_ptr<Sequence> SequecePtr;
/*
 * ================================
 *        Class:  Barrier
 *  Description:
 * ================================
 */
class Barrier {
public:
    /* =============  LIFECYCLE     =================== */
    Barrier(){}; /* constructor */

    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */
    SeqType get_min();
    SeqType last();
    const std::size_t size() const { return _limit_seq.size(); };
    SeqType wait_for(SeqType pos) const;

    void follows(const SequecePtr seq);
    bool quit();

    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    mutable SeqType _last_min = 0;

    // wait_for 的 id 值
    std::vector<SequecePtr> _limit_seq;
}; /* -----  end of class Barrier  ----- */

/*
 * ================================
 *        Class:  Mutex
 *  Description:
 * ================================
 */
class Mutex {
public:
    /* =============  LIFECYCLE     =================== */
    Mutex(){}; /* constructor */

    /* =============  ACCESSORS     =================== */
    void lock();

    void unlock();

    int sub();
    void release(SeqType);
    void trigger(const trigger_ptr _other);
    void sequence(SequecePtr _other);
    SeqType status();
    /* =============  MUTATORS      =================== */

    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    // int locks = 0, unlocks = 0;
    trigger_ptr _trigger = nullptr;

    // 自己的 id 值
    SequecePtr _sequence_ptr = nullptr;
}; /* -----  end of class Mutex  ----- */

/*
 * ================================
 *        Class:  Lock
 *  Description:
 * ================================
 */
template <typename Mutex>
class Lock {
public:
    using mutex_type = Mutex;
    /* =============  LIFECYCLE     =================== */
    Lock(){}; /* constructor */
    explicit Lock(Mutex &m_) : m(m_) { m.lock(); }

    ~Lock() { m.unlock(); }
    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */

    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    Mutex &m;

}; /* -----  end of class Lock  ----- */

using LockMutex = Lock<Mutex>;

/*
 * ================================
 *        Class:  Disruptor
 *  Description:  https://github.com/LMAX-Exchange/disruptor/
 *
 *  SilkPermit pow 2 = row
 *  ((N & (~N + 1)) == N)
 *  RingBuffer's size must be a positive power of 2"
 *
 *  通过 LMAX的架构 分裂者 模式 master->slave  内存管理
 * ================================
 */
class Disruptor : public SilkPermit<SeqType, LockMutex> {
public:
    /* =============  LIFECYCLE     =================== */
    Disruptor(){}; /* constructor */
    Disruptor(const Mulberry<SeqType> mul) : SilkPermit(mul)
    {
        _trigger = std::make_shared<Trigger>();
        _wait_number = std::make_shared<Sequence>();

        _sequence_ptr = std::make_shared<Sequence>();
        _sequence_ptr->alert(E2Q_RUNING);

        SilkPMutex.trigger(_trigger);
        SilkPMutex.sequence(_sequence_ptr);

        _mulberry.overlap = false;
        /* if (_mulberry.rw == _ENUM::rw) { */
        /*     log::echo("disruptor is rw"); */
        /* } */
        /* else { */
        /*     log::echo("disruptor is r"); */
        /* } */
    }

    Disruptor(const Mulberry<SeqType> mul, trigger_ptr tg)
        : SilkPermit(mul), _trigger(tg)
    {
        _sequence_ptr = std::make_shared<Sequence>();
        _sequence_ptr->alert(E2Q_RUNING);
        _wait_number = std::make_shared<Sequence>();

        SilkPMutex.trigger(_trigger);
        SilkPMutex.sequence(_sequence_ptr);

        _mulberry.overlap = false;
        if (_mulberry.rw == _ENUM::rw) {
            log::echo("disruptor twins is rw");
        }
        else {
            log::echo("disruptor is r");
        }
    }

    Disruptor(const Disruptor &_other) { *this = _other; };
    ~Disruptor() {}
    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */
    void name(std::string);
    const std::string name();
    bool isRead();
    /**
     * 不支持修改某一个数据
     */
    void push(size_t row, size_t column, SeqType &data) = delete;

    void follows(const SequecePtr seq);
    std::size_t distance();
    void from(const std::shared_ptr<Disruptor> _other);

    void from(const Disruptor &_other);
    const SequecePtr me();
    void wait_for(wait_for_t);

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  wait_slave
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     * 因为是 wait_next -> deposit 这样的话，
     * 会存在一个 状态是 E2Q_RUNING, slave 会一直跑下去
     * 现在加一个 wait 如果是 slave  都跑完目前数据了，就先让他们停一会儿
     * ============================================
     */
    void wait_slave(std::size_t); /* -----  end of function wait_slave  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  Disruptor::deposit
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *   insert unlock ，触发 emit
     *   并且
     *  _sequence_ptr->atomic_increment_and_get(1);
     * ============================================
     */
    template <typename std::size_t Nc, size_t Nr>
    std::vector<int> deposit(std::array<std::array<SeqType, Nc>, Nr> &data)
    {
        std::vector<int> ret = insert(data);

        return ret;
    }
    template <typename std::size_t Nc>
    int deposit(const std::array<SeqType, Nc> &data)
    {
        int ret = insert(data);
        return ret;
    }
    int deposit(const std::shared_ptr<SeqType[]> data, size_t size)
    {
        int ret = insert(data, size);

        return ret;
    }

    /* -----  end of function Disruptor::deposit  ----- */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  Disruptor::wait_next
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *   master 往下一位移动
     *
     * ============================================
     */
    int wait_next();

    void quit();

    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    // 自己的 id 值
    SequecePtr _sequence_ptr = nullptr;
    // emit sub
    trigger_ptr _trigger = nullptr;

    // barrier list
    Barrier _barrier_list;

    // class name
    std::string _name = "";

    SequecePtr _wait_number = nullptr;

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

}; /* -----  end of class Disruptor  ----- */

/*
 * ================================
 *        Class:  ThreadDisruptor
 *  Description:
 * ================================
 */
class ThreadDisruptor : public Disruptor {
public:
    /* =============  LIFECYCLE     =================== */
    ThreadDisruptor(){}; /* constructor */
    ThreadDisruptor(const Mulberry<SeqType> mul) : Disruptor(mul){};
    ThreadDisruptor(const Mulberry<SeqType> mul, trigger_ptr tg)
        : Disruptor(mul, tg)
    {
    }
    ThreadDisruptor(const ThreadDisruptor &_other) : Disruptor(_other) {}

    /* =============  ACCESSORS     =================== */

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  push
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    void push(size_t row, size_t column, const SeqType &data)
    {
        SilkPermit::push(row, column, data);
    }
    template <typename std::size_t Nc>
    void push(std::size_t row, const std::array<SeqType, Nc> &data)
    {
        SilkPermit::push(row, data);
    } /* -----  end of function push  ----- */

    void thread_for(std::size_t row);
    void show()
    {
        for (auto b : _thread_row) {
            log::echo("id:", b.first, " row:", b.second);
        }
    };

    void full();

    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    // thread map
    std::map<std::thread::id, std::size_t> _thread_row;
}; /* -----  end of class ThreadDisruptor  ----- */

}  // namespace e2q
#endif /* ----- #ifndef DISRUPTOR_INC  ----- */
