/*
 * =====================================================================================
 *
 *       Filename:  Disruptor.cpp
 *
 *    Description:  Disruptor
 *
 *        Version:  1.0
 *        Created:  2023年09月06日 14时13分40秒
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

#include "MessagePack/Disruptor.hpp"

#include <unistd.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <memory>
#include <thread>
#include <utility>
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Sequence::Sequence
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
Sequence::Sequence()
{
    _sequence.store(0, std::memory_order_release);
} /* -----  end of function Sequence::Sequence  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Sequence::aquire
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
SeqType Sequence::aquire()
{
    return _sequence.load(std::memory_order_acquire);
} /* -----  end of function Sequence::aquire  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  store
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Sequence::store(SeqType value)
{
    _sequence.store(value, std::memory_order_release);
} /* -----  end of function store  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  atomic_increment_and_get
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
SeqType Sequence::atomic_increment_and_get(SeqType inc)
{
    return _sequence.fetch_add(inc, std::memory_order_release) + inc;
} /* -----  end of function atomic_increment_and_get  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Sequence::alert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool Sequence::alert()
{
    return _alert.load(std::memory_order_acquire) == E2Q_RUNING;

} /* -----  end of function Sequence::alert  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Sequence::status
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
SeqType Sequence::status()
{
    return _alert.load(std::memory_order_acquire);
} /* -----  end of function Sequence::status  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Sequence::check_alert
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Sequence::alert(SeqType value)
{
    _alert.store(value, std::memory_order_release);

} /* -----  end of function Sequence::check_alert  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Barrier::get_min
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
SeqType Barrier::get_min()
{
    SeqType min_pos = -1;
    for (auto itr : _limit_seq) {
        auto itr_pos = itr->aquire();
        if (min_pos == -1) {
            min_pos = itr_pos;
            continue;
        }
        if (itr_pos < min_pos) min_pos = itr_pos;
    }
    return _last_min = min_pos;

} /* -----  end of function Barrier::get_min  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Barrier
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
SeqType Barrier::last()
{
    return _last_min;
} /* -----  end of function Barrier  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Barrier::wait_for
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
SeqType Barrier::wait_for(SeqType pos) const
{
    // log::info("_limit_seq size:", _limit_seq.size());
    if (_last_min > pos) return _last_min;
    SeqType itr_pos = 0;
    SeqType min_pos = -1;
    for (auto itr : _limit_seq) {
        itr_pos = itr->aquire();
        if (min_pos == -1) {
            min_pos = itr_pos;
            continue;
        }
        // spin for a bit
        for (int i = 0; itr_pos < pos && i < 10000; ++i) {
            itr_pos = itr->aquire();
            if (itr->alert()) break;
        }
        // yield for a while, queue slowing down
        for (int y = 0; itr_pos < pos && y < 10000; ++y) {
            // usleep(0);
            itr_pos = itr->aquire();
            if (itr->alert()) break;
        }

        // queue stalled, don't peg the CPU but don't wait
        // too long either...
        while (itr_pos < pos) {
            // usleep(10 * 1000);
            itr_pos = itr->aquire();
            if (itr->alert()) break;
        }

        if (itr->alert()) {
            //   itr->alert();
            if (itr_pos > pos)
                return itr_pos - 1;  // process everything up to itr_pos
            throw eof();
        }

        if (itr_pos < min_pos) min_pos = itr_pos;
    }
    assert(min_pos != -1);
    return _last_min = min_pos;

} /* -----  end of function Barrier::wait_for  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Barrier::follows
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Barrier::follows(const SequecePtr seq)
{
    _limit_seq.push_back(std::move(seq));

} /* -----  end of function Barrier::follows  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Barrier::quit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool Barrier::quit()
{
    bool ok = 1;
    for (auto itr : _limit_seq) {
        auto itr_pos = itr->alert();
        ok = ok & itr_pos;
    }
    return ok;
} /* -----  end of function Barrier::quit  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Mutex::lock
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Mutex::lock()
{
    if (_trigger != nullptr) {
        _trigger->start();
    }
} /* -----  end of function Mutex::lock  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Mutex::unlock
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Mutex::unlock()
{
    //   unlocks++;
    if (_trigger != nullptr) {
        SeqType stu = _sequence_ptr->status();

        _trigger->turn(stu);
        _trigger->emit();

        _sequence_ptr->atomic_increment_and_get(1);
    }
} /* -----  end of function Mutex::unlock  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Mutex::sub
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int Mutex::sub()
{
    int r = 0;
    if (_trigger != nullptr) {
        r = _trigger->subscriber();
    }
    return r;

} /* -----  end of function Mutex::sub  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  emit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Mutex::release(SeqType t)
{
    if (_trigger != nullptr) {
        _trigger->turn(t);
        _trigger->emit();
    }
} /* -----  end of function emit  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Mutex::status
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
SeqType Mutex::status()
{
    if (_trigger == nullptr) {
        return E2Q_EXIST;
    }
    return _trigger->status();
} /* -----  end of function Mutex::status  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Mutex::trigger
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Mutex::trigger(const trigger_ptr _other)
{
    _trigger = std::move(_other);
} /* -----  end of function Mutex::trigger  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Mutex::sequence
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Mutex::sequence(SequecePtr _other)
{
    _sequence_ptr = std::move(_other);
} /* -----  end of function Mutex::sequence  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Disruptor::name
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Disruptor::name(std::string val)
{
    _name = std::move(val);
} /* -----  end of function Disruptor::name  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Disruptor::isRead
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  是主还是从
 * ============================================
 */
bool Disruptor::isRead()
{
    return _mulberry.rw == _ENUM::r;
} /* -----  end of function Disruptor::isRead  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Disruptor::name
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
const std::string Disruptor::name()
{
    return _name;
} /* -----  end of function Disruptor::name  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Disruptor::follows
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   跟随者
 * ============================================
 */
void Disruptor::follows(const SequecePtr seq)
{
    _barrier_list.follows(seq);
} /* -----  end of function Disruptor::follows  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Disruptor::distance
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::size_t Disruptor::distance()
{
    SeqType dis = _sequence_ptr->aquire() - _barrier_list.get_min();

    return dis;
} /* -----  end of function Disruptor::isOk  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Disruptor::me
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
const SequecePtr Disruptor::me()
{
    return _sequence_ptr;
} /* -----  end of function Disruptor::me  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Disruptor::quit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Disruptor::quit()
{
    do {
        sleep(1);
        SilkPMutex.release(E2Q_EXIST);

    } while (distance() != 0); /* -----  end do-while  ----- */
    _sequence_ptr->alert(E2Q_EXIST);

#ifdef DEBUG
    SeqType me_seq = 0;
    SeqType follow_min = 0;
    me_seq = _sequence_ptr->aquire();

    follow_min = _barrier_list.get_min();
    log::info("name:", _name, " sleep 1, me_seq:", me_seq,
              " follow:", follow_min, " pid:", getpid());
#endif

} /* -----  end of function Disruptor::quit  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Disruptor::wait_for
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Disruptor::wait_for(wait_for_t fun)
{
    SeqType me_seq = 0;
    SeqType follow_min = 0;
    int r = 0;
    UtilTime ut;
    do {
        me_seq = _sequence_ptr->aquire();
        follow_min = _barrier_list.get_min();

        if (follow_min == me_seq) {
            r = SilkPMutex.sub();

            if (r == -1) {
                break;
            }
            follow_min = _barrier_list.get_min();
        }

        if (follow_min > me_seq) {
            fun(me_seq, follow_min);
        }

        _sequence_ptr->store(follow_min);

    } while (_barrier_list.quit());

#ifdef DEBUG

    log::echo(_name, ", me_seq:", me_seq, " follow:", follow_min,
              " pid:", getpid());
#endif

} /* -----  end of function Disruptor::wait_for  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Disruptor::wait_slave
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Disruptor::wait_slave(std::size_t offer_sleep)
{
    SeqType inc = 1;
    SeqType sleep_time = 3;
    SeqType me = 0;
    SeqType follow = 0;

    SeqType now_stats = 0;

    while (true) {
        now_stats = SilkPMutex.status();
        if (now_stats == E2Q_EXIST) {
            break;
        }
        me = _sequence_ptr->aquire();
        follow = _barrier_list.get_min();

        if (me == follow && now_stats == E2Q_RUNING) {
            SeqType ret = _wait_number->atomic_increment_and_get(inc);
            if (ret > sleep_time) {
                SilkPMutex.release(E2Q_STOP);
                _wait_number->store(0);
            }
        }

        TSleep(offer_sleep);
    }

} /* -----  end of function Disruptor::wait_slave  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Disruptor::wait_next
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   master 往下一位移动
 * ============================================
 */
int Disruptor::wait_next()
{
    size_t dis_val = distance();
    std::size_t clear_cur = _sequence_ptr->aquire();

    std::size_t half = rows() - 1;
    std::size_t offset = 2;

    if (dis_val > offset && dis_val != 0) {
        /**
         * 防止某些线程不读数据，就一直卡住
         */

        SilkPMutex.release(E2Q_RUNING);
    }

    // 保留 half 个数据
    int ret = 0;
    if (clear_cur >= half) {
        ret = cursor();
    }

    return ret;

} /* -----  end of function Disruptor::wait_next  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Disruptor::from
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   追随 master
 * ============================================
 */
void Disruptor::from(const Disruptor &_other)
{
    if (this != &_other) {
        SilkPermit::operator=(_other);
        _sequence_ptr = std::make_shared<Sequence>();
        _wait_number = std::make_shared<Sequence>();

        _barrier_list.follows(_other._sequence_ptr);
        SilkPMutex.trigger(_other._trigger);
        SilkPMutex.sequence(_sequence_ptr);
    }
}

void Disruptor::from(const std::shared_ptr<Disruptor> _other)
{
    if (this != _other.get()) {
        SilkPermit::operator=(_other);
        _sequence_ptr = std::make_shared<Sequence>();
        _wait_number = std::make_shared<Sequence>();

        _barrier_list.follows(_other->_sequence_ptr);
        SilkPMutex.trigger(_other->_trigger);
        SilkPMutex.sequence(_sequence_ptr);
    }
}
/* -----  end of function Disruptor::from  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name: ThreadDisruptor::thread_for
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void ThreadDisruptor::thread_for(std::size_t b)
{
    _thread_row.emplace(std::make_pair(std::this_thread::get_id(), b));
} /* -----  end of function ThreadDisruptor::thread_for  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  ThreadDisruptor::full
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void ThreadDisruptor::full()
{
    std::array<e2q::SeqType, 3> tt{(e2q::SeqType)0, (e2q::SeqType)0,
                                   (e2q::SeqType)0};
    for (std::size_t m = 0; m < rows(); m++) {
        tt[1] = m;
        insert(tt);
    }
} /* -----  end of function ThreadDisruptor::full  ----- */

}  // namespace e2q
