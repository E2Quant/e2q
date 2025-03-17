/*
 * =====================================================================================
 *
 *       Filename:  snowflake.hpp
 *
 *    Description:  snowflake
 *
 *        Version:  1.0
 *        Created:  2024年01月05日 15时01分05秒
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

#ifndef SNOWFLAKE_INC
#define SNOWFLAKE_INC
#include <chrono>
#include <cstdint>
#include <mutex>
namespace e2q {

class snowflake_nonlock {
public:
    void lock() {}
    void unlock() {}
};
/**
 * Copyright @ https://github.com/sniper00/snowflake-cpp
 */
template <std::int64_t Twepoch, typename Lock = snowflake_nonlock>
class snowflake {
    using lock_type = Lock;
    static constexpr std::int64_t TWEPOCH = Twepoch;
    static constexpr std::int64_t WORKER_ID_BITS = 5L;
    static constexpr std::int64_t DATACENTER_ID_BITS = 5L;
    static constexpr std::int64_t MAX_WORKER_ID = (1 << WORKER_ID_BITS) - 1;
    static constexpr std::int64_t MAX_DATACENTER_ID =
        (1 << DATACENTER_ID_BITS) - 1;
    static constexpr std::int64_t SEQUENCE_BITS = 12L;
    static constexpr std::int64_t WORKER_ID_SHIFT = SEQUENCE_BITS;
    static constexpr std::int64_t DATACENTER_ID_SHIFT =
        SEQUENCE_BITS + WORKER_ID_BITS;
    static constexpr std::int64_t TIMESTAMP_LEFT_SHIFT =
        SEQUENCE_BITS + WORKER_ID_BITS + DATACENTER_ID_BITS;
    static constexpr std::int64_t SEQUENCE_MASK = (1 << SEQUENCE_BITS) - 1;

    using time_point = std::chrono::time_point<std::chrono::steady_clock>;

    time_point start_time_point_ = std::chrono::steady_clock::now();
    std::int64_t start_millsecond_ =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();

    std::int64_t last_timestamp_ = -1;
    std::int64_t workerid_ = 0;
    std::int64_t datacenterid_ = 0;
    std::int64_t sequence_ = 0;
    lock_type lock_;

public:
    snowflake() = default;

    snowflake(const snowflake&) = delete;

    snowflake& operator=(const snowflake&) = delete;

    void init(std::int64_t workerid, std::int64_t datacenterid)
    {
        if (workerid > MAX_WORKER_ID || workerid < 0) {
            throw std::runtime_error(
                "worker Id can't be greater than 31 or less than 0");
        }

        if (datacenterid > MAX_DATACENTER_ID || datacenterid < 0) {
            throw std::runtime_error(
                "datacenter Id can't be greater than 31 or less than 0");
        }

        workerid_ = workerid;
        datacenterid_ = datacenterid;
    }

    std::int64_t nextid()
    {
        std::lock_guard<lock_type> lock(lock_);
        // std::chrono::steady_clock  cannot decrease as physical time moves
        // forward
        auto timestamp = millsecond();
        if (last_timestamp_ == timestamp) {
            sequence_ = (sequence_ + 1) & SEQUENCE_MASK;
            if (sequence_ == 0) {
                timestamp = wait_next_millis(last_timestamp_);
            }
        }
        else {
            sequence_ = 0;
        }

        last_timestamp_ = timestamp;

        return ((timestamp - TWEPOCH) << TIMESTAMP_LEFT_SHIFT) |
               (datacenterid_ << DATACENTER_ID_SHIFT) |
               (workerid_ << WORKER_ID_SHIFT) | sequence_;
    }

private:
    std::int64_t millsecond() const noexcept
    {
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start_time_point_);
        return start_millsecond_ + diff.count();
    }

    std::int64_t wait_next_millis(std::int64_t last) const noexcept
    {
        auto timestamp = millsecond();
        while (timestamp <= last) {
            timestamp = millsecond();
        }
        return timestamp;
    }
};
using snowflake_t = snowflake<INT64_MAX>;
using snowflake_lock_t = snowflake<INT64_MAX, std::mutex>;

}  // namespace e2q
#endif /* ----- #ifndef SNOWFLAKE_INC  ----- */
