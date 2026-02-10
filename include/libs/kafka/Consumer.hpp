/*
 * =====================================================================================
 *
 *       Filename:  Kafka.hpp
 *
 *    Description:  Kafak.hpp
 *
 *        Version:  1.0
 *        Created:  2024年02月04日 10时59分58秒
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

#ifndef KAFKA_INC
#define KAFKA_INC

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

#include "OMSPack/FixGuard.hpp"
#include "Toolkit/Norm.hpp"
#include "Toolkit/pack.hpp"
#include "assembler/BaseType.hpp"
#include "librdkafka/rdkafkacpp.h"
#include "libs/bprinter/table_printer.h"
#include "libs/kafka/protocol/proto.hpp"

#if __cplusplus >= 201703L

/* Now remove the trow */
#define throw(...)

#include <quickfix/FieldTypes.h>
#include <quickfix/FixFields.h>
#include <quickfix/Session.h>
#include <quickfix/fix44/MassQuote.h>
#include <quickfix/fix44/OrderStatusRequest.h>
#include <quickfix/fix44/Quote.h>
#include <quickfix/fix44/QuoteStatusReport.h>

#undef throw /* reset */
#endif

namespace e2q {

/**
 * https://github.com/confluentinc/librdkafka/blob/master/examples/rdkafka_example.cpp
 */
inline volatile sig_atomic_t _kafka_run = 1;
inline bool exit_eof = false;

inline void sigterm(int sig) { _kafka_run = 0; }

class KfEventCb : public RdKafka::EventCb {
public:
    void event_cb(RdKafka::Event& event)
    {
        switch (event.type()) {
            case RdKafka::Event::EVENT_ERROR:
                if (event.fatal()) {
                    _kafka_run = 0;
                }
                elog::bug("FATAL ERROR (", RdKafka::err2str(event.err()),
                          "): ", event.str(), " bokers:", _bokers,
                          " topic:", _topic);
                break;

            case RdKafka::Event::EVENT_STATS:
                elog::bug("\"STATS\": ", event.str());
                break;

            case RdKafka::Event::EVENT_LOG:
                elog::bug("LOG-", event.severity(), "-", event.fac().c_str(),
                          "-", event.str().c_str(), " bokers:", _bokers,
                          " topic:", _topic);
                break;

            default:
                elog::bug("EVENT ", event.type(), " (",
                          RdKafka::err2str(event.err()), "): ", event.str());
                break;
        }
    }

    std::string _bokers;
    std::string _topic;
};

/*
 * ================================
 *        Class:  KfConsumeCb
 *  Description:
 * ================================
 */
class KfConsumeCb : public RdKafka::ConsumeCb, FixGuard {
public:
    /* =============  LIFECYCLE     =================== */
    KfConsumeCb() {}; /* constructor */

    /* =============  ACCESSORS     =================== */
    void consume_cb(RdKafka::Message& msg, void* opaque)
    {
        msg_consume(&msg, opaque);
    }
    void msg_consume(RdKafka::Message* message, void* opaque)
    {
        int64_t now_offset = 0;
        // elog::info("Read msg at offset ", message->offset());

        switch (message->err()) {
            case RdKafka::ERR__TIMED_OUT:
                break;

            case RdKafka::ERR_NO_ERROR: {
                /* Real message */

                if (message->key()) {
                    elog::info("Key: ", *message->key());
                }
                Header(message->headers());

                now_offset = message->offset();
                if (_lastoffset >= now_offset) {
                    elog::bug("repeat offset:", now_offset);
                }
                _lastoffset = now_offset;

                int sz = static_cast<int>(message->len()) - 1;

                const char* p = static_cast<const char*>(message->payload());

                Events(p, sz, now_offset);
                break;
            }

            case RdKafka::ERR__PARTITION_EOF:
                /* Last message */
                if (exit_eof) {
                    ExitOrder();
                }
                break;

            case RdKafka::ERR__UNKNOWN_TOPIC:
            case RdKafka::ERR__UNKNOWN_PARTITION:
                elog::bug("Consume failed: ", message->errstr());
                ExitOrder();
                break;

            default:
                /* Errors */
                elog::bug("Consume failed: ", message->errstr());
                ExitOrder();
        }
    }
    /* =============  MUTATORS      =================== */
    void handle(TradType t) { _TunCall = std::move(t); }
    void dealcall(deal_match_type t) { _DealCall = std::move(t); }
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    void SymbolInit(const char* p, int sz);
    void SymbolExrd(const char* p, int sz);
    void MarketIng(const char* p, int sz);
    void CustomMsg(const char* p, int sz, int64_t);
    void StopOrder();
    void ExitOrder();

    void Header(const RdKafka::Headers* headers);

    void Events(const char* p, int sz, int64_t now_offset);

    void Quote(const FIX::SessionID&, MarketDelOrIPOMessage mdoi);
    void QuoteStatusReport(const FIX::SessionID&, ExRD, std::size_t,
                           std::vector<long>);
    void logs(std::array<SeqType, trading_protocols>, int64_t);

    void TicketMsg(const char* ptr, int sz, int64_t);

    void DealMatchMsg(const char* ptr, int sz, int64_t);

    /* =============  DATA MEMBERS  =================== */
    TradType _TunCall = nullptr;
    deal_match_type _DealCall = nullptr;

    std::array<SeqType, trading_protocols> _call_data{0};

    uint64_t _lastTime = 0;

    int64_t _lastoffset;

}; /* -----  end of class KfConsumeCb  ----- */

/*
 * ================================
 *        Class:  KafkaFeed
 *  Description:
 * ================================
 */
class KafkaFeed {
public:
    /* =============  LIFECYCLE     =================== */
    KafkaFeed(std::string& bokers, std::string& topic)
        : _bokers(bokers), _topic(topic) {

          }; /* constructor */

    /* =============  ACCESSORS     =================== */
    void handle(TradType tt);
    void dealCall(deal_match_type tt) { _DealCall = std::move(tt); };
    /* =============  MUTATORS      =================== */

    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    std::string _bokers;
    std::string _topic;
    deal_match_type _DealCall = nullptr;

    //    std::vector<std::string> _symobls;
}; /* -----  end of class KafkaFeed  ----- */

}  // namespace e2q
#endif /* ----- #ifndef KAFKA_INC  ----- */
