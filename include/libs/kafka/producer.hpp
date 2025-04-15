/*
 * =====================================================================================
 *
 *       Filename:  producer.hpp
 *
 *    Description:  producer
 *
 *        Version:  1.0
 *        Created:  2024年07月12日 14时43分15秒
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

#ifndef PRODUCER_INC
#define PRODUCER_INC
#include <atomic>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <queue>
#include <string>
#include <thread>

#include "Toolkit/Norm.hpp"
#include "assembler/BaseType.hpp"

#ifdef _WIN32
#include "../win32/wingetopt.h"
#elif _AIX
#include <unistd.h>
#else
#include <getopt.h>
#endif
#include <stdlib.h>

/*
 * Typically include path in a real application would be
 * #include <librdkafka/rdkafkacpp.h>
 */
#include "Toolkit/pack.hpp"
#include "librdkafka/rdkafkacpp.h"
namespace e2q {

static volatile sig_atomic_t pro_run = 1;
inline bool pro_exit_eof = false;

inline void pro_sigterm(int sig) { pro_run = 0; }

/* Use of this partitioner is pretty pointless since no key is provided
 * in the produce() call. */
class MyHashPartitionerCb : public RdKafka::PartitionerCb {
public:
    int32_t partitioner_cb(const RdKafka::Topic *topic, const std::string *key,
                           int32_t partition_cnt, void *msg_opaque)
    {
        return djb_hash(key->c_str(), key->size()) % partition_cnt;
    }

private:
    static inline unsigned int djb_hash(const char *str, size_t len)
    {
        unsigned int hash = 5381;
        for (size_t i = 0; i < len; i++) hash = ((hash << 5) + hash) + str[i];
        return hash;
    }
};

class ExampleEventCb : public RdKafka::EventCb {
public:
    void event_cb(RdKafka::Event &event)
    {
        switch (event.type()) {
            case RdKafka::Event::EVENT_ERROR:
                if (event.fatal()) {
                    std::cerr << "FATAL ";
                    pro_run = 0;
                }
                std::cerr << "ERROR (" << RdKafka::err2str(event.err())
                          << "): " << event.str() << std::endl;
                break;

            case RdKafka::Event::EVENT_STATS:
                std::cerr << "\"STATS\": " << event.str() << std::endl;
                break;

            case RdKafka::Event::EVENT_LOG:
                fprintf(stderr, "LOG-%i-%s: %s\n", event.severity(),
                        event.fac().c_str(), event.str().c_str());
                break;

            default:
                std::cerr << "EVENT " << event.type() << " ("
                          << RdKafka::err2str(event.err())
                          << "): " << event.str() << std::endl;
                break;
        }
    }
};

/*
 * ================================
 *        Class:  Producer
 *  Description:
 * ================================
 */
class Producer {
public:
    /* =============  LIFECYCLE     =================== */
    Producer() {}; /* constructor */
    ~Producer()
    {
        RELEASE(producer);
        RELEASE(conf);
        RELEASE(tconf);

        /*
         * Wait for RdKafka to decommission.
         * This is not strictly needed (when check outq_len() above), but
         * allows RdKafka to clean up all its resources before the application
         * exits so that memory profilers such as valgrind wont complain about
         * memory leaks.
         */
        RdKafka::wait_destroyed(5000);
    }
    /* =============  ACCESSORS     =================== */
    int init(std::string brokers, std::string topic_str);
    void daemon();
    /* =============  MUTATORS      =================== */
    void data(std::pair<std::thread::id, std::string>);
    void data(std::pair<std::thread::id, std::string>, std::string);
    void data(std::string, std::string);
    void data(const char *data, std::size_t size);
    void data(const char *data, std::size_t size, RdKafka::Headers *headers);
    void data(const char *data, std::size_t size, std::string,
              RdKafka::Headers *headers);

    void exist();
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    // std::condition_variable cv;
    // std::mutex guard;

    RdKafka::Conf *conf = nullptr;
    RdKafka::Conf *tconf = nullptr;

    std::atomic_bool _active{true};
    std::queue<std::pair<std::thread::id, std::string>> _arg;

    // std::atomic_bool _init{false};
    std::string log_topic = "";
    RdKafka::Producer *producer = nullptr;
    using EMute = BasicLock::mutex_type;
    mutable EMute _kMute;
}; /* -----  end of class Producer  ----- */

}  // namespace e2q
#endif /* ----- #ifndef Producer_INC  ----- */
