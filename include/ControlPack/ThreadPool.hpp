/*
 * =====================================================================================
 *
 *       Filename:  ThreadPool.hpp
 *
 *    Description:  ThreadPool
 *
 *        Version:  1.0
 *        Created:  2024年07月09日 10时54分02秒
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

#ifndef THREADPOOL_INC
#define THREADPOOL_INC
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include "E2LScript/ExternClazz.hpp"
#include "Toolkit/Norm.hpp"
#include "Toolkit/pack.hpp"
namespace e2q {

/*
 * ================================
 *        Class:  ThreadPool
 *  Description:
 * ================================
 */
template <typename T>
class ThreadPool {
public:
    /* =============  LIFECYCLE     =================== */
    ThreadPool(std::size_t num_threads) : _num_threads(num_threads)
    {
        if (FixPtr->_cash._thread_pos.size() > 0) {
            for (auto it : FixPtr->_cash._thread_pos) {
                // std::cout << "thread_pos == " << it.first
                //           << "  post:" << it.second._postion <<
                //           std::endl;
                _thread_queue.push(it.first);
            }
        }
        else {
            for (std::size_t m = 0; m < _num_threads; m++) {
                _thread_queue.push(m);
            }
        }

    }; /* constructor */
    ~ThreadPool()
    {
        if (_active) {
            _active = false;
            cv.notify_all();
            for (auto& th : _pool) {
                th.join();
            }
        }
    }
    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */
    void init()
    {
        for (std::size_t i = 0; i < _num_threads; i++) {
            _pool.emplace_back(&ThreadPool::run, this, i);
        }
    }
    void exits()
    {
        _active = false;
        cv.notify_all();
        for (auto& th : _pool) {
            th.join();
        }
    }
    void job_init(func_type<T, std::thread::id> f) { _job_init = std::move(f); }
    void jobs(func_type<T, T, std::size_t, std::thread::id> f)
    {
        _job_fun = std::move(f);
    }
    void emit(T t)
    {
        // std::unique_lock lock(guard);
        // std::cout << "emit" << std::endl;
        SeqType m = 0;
        for (auto it = _arg_jobs.begin(); it != _arg_jobs.end(); ++it) {
            it->second.push({t, m});
        }
        cv.notify_all();
    }
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    void run(std::size_t idx)
    {
        if (_job_fun == nullptr || _job_init == nullptr) {
            log::bug("job_fun or job_init is nullptr");
            return;
        }

        std::thread::id _id = std::this_thread::get_id();
        std::queue<std::pair<T, T>> q;
        {
            std::unique_lock lock(guard);
            _arg_jobs.insert({_id, q});

            std::size_t thread_num = _thread_queue.front();
            _thread_queue.pop();

            _thread_number.insert({_id, thread_num});

            _job_init(thread_num, _id);
        };

        /**
         * while 设置为 _active 的话，有可能没有运行完就退出来
         * 如果设置为 true 的话，就会完全运行完成才退出来
         */
        while (_active) {
            thread_local std::packaged_task<void()> job;
            {
                if (!_active && _arg_jobs[_id].empty()) {
                    log::echo("exit");
                    break;
                }
                /**
                 * e2l 那儿不需要再加锁了
                 * 或者以后再优化吧
                 */
                std::unique_lock lock(guard);

                cv.wait(lock,
                        [&] { return !_arg_jobs[_id].empty() || !_active; });

                if (!_active) {
                    break;
                }
                if (_arg_jobs.count(_id) == 0) {
                    log::bug("exit bug");
                    break;
                }
                std::pair<T, T> arg = _arg_jobs[_id].front();
                _arg_jobs[_id].pop();

                std::size_t tnum = _thread_number.at(_id);
                std::packaged_task<void()> task(
                    std::bind(_job_fun, arg.first, tnum, _num_threads, _id));
                job.swap(task);
            }
            job();
        }
    }
    /* =============  DATA MEMBERS  =================== */
    func_type<T, T, std::size_t, std::thread::id> _job_fun{nullptr};
    func_type<T, std::thread::id> _job_init{nullptr};

    std::size_t _num_threads = 0;
    std::map<std::thread::id, std::queue<std::pair<T, T>>> _arg_jobs;

    std::vector<std::thread> _pool;
    std::atomic_bool _active{true};

    std::queue<std::size_t> _thread_queue;
    std::map<std::thread::id, std::size_t> _thread_number;

    std::condition_variable cv;
    std::mutex guard;
}; /* -----  end of class ThreadPool  ----- */

}  // namespace e2q
#endif /* ----- #ifndef THREADPOOL_INC  ----- */
