/*
 * =====================================================================================
 *
 *       Filename:  producer.cpp
 *
 *    Description:  producer
 *
 *        Version:  1.0
 *        Created:  2024年07月12日 14时43分54秒
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
#include "libs/kafka/producer.hpp"

#include <cstddef>
#include <cstdio>
#include <sstream>
#include <string>
#include <thread>

#include "Toolkit/Norm.hpp"
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Producer::init
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Producer::init(std::string brokers, std::string topic_str)
{
    if (brokers.length() == 0 || topic_str.length() == 0) {
        _active = false;
        log::bug("brokers:", brokers, " topic:", topic_str);
        return;
    }

    std::string errstr;
    std::string mode;
    std::string debug;

    MyHashPartitionerCb hash_partitioner;

    /*
     * Create configuration objects
     */
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    RdKafka::Conf *tconf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

    /*
     * Set configuration properties
     */
    conf->set("metadata.broker.list", brokers, errstr);

    ExampleEventCb ex_event_cb;
    conf->set("event_cb", &ex_event_cb, errstr);

    signal(SIGINT, pro_sigterm);
    signal(SIGTERM, pro_sigterm);

    /*
     * Producer mode
     */

    conf->set("default_topic_conf", tconf, errstr);

    /*
     * Create producer using accumulated global configuration.
     */
    producer = RdKafka::Producer::create(conf, errstr);
    if (!producer) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }

    log_topic = topic_str;
    while (_active || producer->outq_len() > 0) {
        // std::cerr << "Waiting for " << producer->outq_len() << std::endl;
        producer->poll(1000);
    }

    delete producer;

    delete conf;
    delete tconf;

    /*
     * Wait for RdKafka to decommission.
     * This is not strictly needed (when check outq_len() above), but
     * allows RdKafka to clean up all its resources before the application
     * exits so that memory profilers such as valgrind wont complain about
     * memory leaks.
     */
    RdKafka::wait_destroyed(5000);

} /* -----  end of function Producer::init  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Producer::data
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Producer::data(std::pair<std::thread::id, std::string> arg)
{
    data(arg, log_topic);
}
void Producer::data(std::pair<std::thread::id, std::string> msg,
                    std::string topic_str)
{
    std::thread::id _id;
    std::stringstream ssId;
    std::string line;
    RdKafka::Headers *headers = RdKafka::Headers::create();
    /*
     * Produce message
     */
    line = msg.second;
    _id = msg.first;

    ssId.str("");
    ssId.clear();

    ssId << _id;

    headers->add("thread_id", ssId.str());

    data(line.c_str(), line.size(), topic_str, headers);
}

void Producer::data(std::string line, std::string topic_str)
{
    data(line.c_str(), line.size(), topic_str, nullptr);
}
void Producer::data(const char *line, std::size_t size)
{
    data(line, size, log_topic, NULL);
}
void Producer::data(const char *line, std::size_t size,
                    RdKafka::Headers *headers)
{
    data(line, size, log_topic, headers);
}
void Producer::data(const char *line, std::size_t size, std::string topic_str,
                    RdKafka::Headers *headers)
{
    int32_t partition = RdKafka::Topic::PARTITION_UA;

    if (producer == nullptr || topic_str.length() == 0 || line == nullptr) {
        log::bug(" topic:", topic_str);
        return;
    }

    BasicLock _lock(_kMute);
    //    log::info(line + "  " + topic_str);
    RdKafka::ErrorCode resp = producer->produce(
        topic_str, partition, RdKafka::Producer::RK_MSG_COPY /* Copy payload
                                                              */
        ,
        /* Value */
        const_cast<char *>(line), size,
        /* Key */
        NULL, 0,
        /* Timestamp (defaults to now) */
        0,
        /* Message headers, if any */
        headers,
        /* Per-message opaque value passed to
         * delivery report */
        NULL);

    /**
     * 有时候需要这样来刷新数据
     */

    producer->flush(100);
    if (resp != RdKafka::ERR_NO_ERROR) {
        log::bug(" Produce failed: ", RdKafka::err2str(resp));
        delete headers;
    }

    // producer->poll(0);
}

/* -----  end of function Producer::data  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Producer::exist
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Producer::exist()
{
    _active = false;

} /* -----  end of function Producer::exist  ----- */
}  // namespace e2q
