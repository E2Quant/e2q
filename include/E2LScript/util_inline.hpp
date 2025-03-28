/*
 * =====================================================================================
 *
 *       Filename:  util_inline.hpp
 *
 *    Description:  util_inline
 *
 *        Version:  1.0
 *        Created:  2024年07月31日 09时56分39秒
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

#ifndef UTIL_INLINE_INC
#define UTIL_INLINE_INC
#include <stdio.h>
#include <unistd.h>

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "E2L/E2LType.hpp"
#include "E2L/account.hpp"
#include "E2L/analyzer.hpp"
#include "E2L/bot.hpp"
#include "E2L/broker.hpp"
#include "E2L/date_time.hpp"
#include "E2L/general.hpp"
#include "E2L/indicator.hpp"
#include "E2L/init.hpp"
#include "E2L/marketinfo.hpp"
#include "E2L/math.hpp"
#include "E2L/predefined.hpp"
#include "E2L/system.hpp"
#include "E2L/timeseries.hpp"
#include "E2L/trade.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "FeedPack/Container.hpp"
#include "MessagePack/RingLoop.hpp"
#include "assembler/BaseNode.hpp"
#include "assembler/BaseType.hpp"
#include "libs/kafka/producer.hpp"
#include "libs/kafka/protocol/nbo.hpp"
#include "libs/kafka/protocol/proto.hpp"
#include "utility/Log.hpp"

namespace e2q {

struct __AutoInc_t {
    void init(std::thread::id _id)
    {
        if (_autoinc.count(_id) == 0) {
            BasicLock _lock(_EMute);
            std::shared_ptr<e2q::AutoIncrement> ai =
                std::make_shared<e2q::AutoIncrement>();
            _autoinc.insert({_id, ai});
        }
    }

    void AutoInit(std::thread::id _id, std::size_t num)
    {
        if (_autoinc.count(_id) == 1) {
            _autoinc.at(_id)->init();
            _autoinc.at(_id)->_number = num;
        }
    }

    e2::Int_e Id(std::thread::id _id) { return _autoinc.at(_id)->Id(); }
    e2::Int_e StoreId(std::thread::id _id)
    {
        return _autoinc.at(_id)->StoreId();
    }

    std::size_t number(std::thread::id _id)
    {
        if (_autoinc.count(_id) == 1) {
            return _autoinc.at(_id)->_number;
        }
        return 0;
    }

private:
    std::map<std::thread::id, std::shared_ptr<AutoIncrement>> _autoinc;
    using EMute = BasicLock::mutex_type;
    mutable EMute _EMute;

}; /* ----------  end of struct __AutoInc_t  ---------- */

typedef struct __AutoInc_t AutoInc_t;

inline AutoInc_t e2l_thread_map;

#define AutoInc(_id)                          \
    ({                                        \
        do {                                  \
            _id = std::this_thread::get_id(); \
            e2q::e2l_thread_map.init(_id);    \
        } while (0);                          \
    })

/**
 *  e2l variable store
 *   thread_id: { store id, store value }
 */

struct __Silk_t {
    void init(std::thread::id _id)
    {
        BasicLock _lock(_EMute);
        std::map<e2::Int_e, e2::Int_e> p;
        _silk.insert({_id, p});
    }
    bool exist(std::thread::id _id) { return _silk.count(_id) == 0; }
    bool check(std::thread::id _id, e2::Int_e id)
    {
        return _silk.at(_id).count(id) == 0;
    }
    void insert(std::thread::id _id, e2::Int_e id, e2::Int_e val)
    {
        BasicLock _lock(_EMute);
        _silk[_id].insert({id, val});
    }
    void update(std::thread::id _id, e2::Int_e id, e2::Int_e val)
    {
        BasicLock _lock(_EMute);
        _silk[_id][id] = val;
    }
    e2::Int_e get(std::thread::id _id, e2::Int_e id) { return _silk[_id][id]; }

private:
    std::map<std::thread::id, std::map<e2::Int_e, e2::Int_e>> _silk;
    using EMute = BasicLock::mutex_type;
    mutable EMute _EMute;

}; /* ----------  end of struct __Silk_t  ---------- */

typedef struct __Silk_t Silk_t;
inline Silk_t e2l_silk;

#define E2LSILK(_bool, _id, id)                 \
    ({                                          \
        do {                                    \
            _id = std::this_thread::get_id();   \
            if (e2q::e2l_silk.exist(_id)) {     \
                e2q::e2l_silk.init(_id);        \
            }                                   \
            if (e2q::e2l_silk.check(_id, id)) { \
                _bool = e2::Bool::B_FALSE;      \
            }                                   \
            else {                              \
                _bool = e2::Bool::B_TRUE;       \
            }                                   \
        } while (0);                            \
    })

struct __BarOHLC_t {
    void init(std::thread::id _id)
    {
        BasicLock _lock(_EMute);
        std::array<e2q::SeqType, ohlc_column> bar{0};

        if (_bar_ohlc.count(_id) == 0) {
            std::size_t idx = 0;
            _bar_ohlc.insert({_id, {idx, bar}});
        }
    }

    void clear(std::thread::id _id)
    {
        std::fill(_bar_ohlc.at(_id).second.begin(),
                  _bar_ohlc.at(_id).second.end(), 0);
    }

    std::size_t size(std::thread::id _id)
    {
        return _bar_ohlc.at(_id).second.size();
    }

    e2::Int_e value(std::thread::id _id, e2::BarType bt)
    {
        return _bar_ohlc.at(_id).second[bt];
    }

    void update(std::thread::id _id, size_t idx,
                std::array<e2q::SeqType, ohlc_column> bar)
    {
        BasicLock _lock(_EMute);
        clear(_id);
        _bar_ohlc.at(_id).second = bar;
        _bar_ohlc.at(_id).first = idx;
    }

private:
    std::map<std::thread::id,
             std::pair<std::size_t, std::array<e2q::SeqType, ohlc_column>>>
        _bar_ohlc;

    using EMute = BasicLock::mutex_type;
    mutable EMute _EMute;

}; /* ----------  end of struct __BarOHLC_t  ---------- */

typedef struct __BarOHLC_t BarOHLC_t;

inline BarOHLC_t e2l_bar_ohlc;

#define E2LBAR(id)                      \
    ({                                  \
        do {                            \
            e2q::e2l_bar_ohlc.init(id); \
        } while (0);                    \
    })

struct __OrderStruct {
    e2::Int_e id;  // tick or pos
    e2::SelectFlag select;
    e2::SelectFlag pool;
}; /* ----------  end of struct __OrderStruct  ---------- */

typedef struct __OrderStruct OrderStruct;

struct __Select_t {
    void insert(std::thread::id _id, e2::Int_e index, e2::SelectFlag select,
                e2::SelectFlag pool)
    {
        BasicLock _lock(_EMute);

        if (_os.count(_id) == 0) {
            e2q::OrderStruct os;
            os.id = index;
            os.select = select;
            os.pool = pool;
            _os.insert({_id, os});
        }
        else {
            _os.at(_id).id = index;
            _os.at(_id).select = select;
            _os.at(_id).pool = pool;
        }
    }

    bool check(std::thread::id _id) { return _os.count(_id) == 0; }
    e2q::OrderStruct get(std::thread::id _id) { return _os.at(_id); }

    void release(std::thread::id _id)
    {
        BasicLock _lock(_EMute);
        _os.at(_id).id = -1;
    }

private:
    std::map<std::thread::id, OrderStruct> _os;

    using EMute = BasicLock::mutex_type;
    mutable EMute _EMute;

}; /* ----------  end of struct __Select_t  ---------- */

typedef struct __Select_t Select_t;

inline Select_t e2_os;

struct __e2lAnalse {
    e2::Int_e id = 0;       // indicator id
    e2::Int_e quantId = 0;  // quant id
    std::string name = "";  // indicator name
    std::size_t etime = 0;  // index time
    std::string argv;       // indicator args
    std::string values;     // indicator value
    std::size_t init = 0;   // init
    std::string toString()
    {
        /**
         *  id,etime,name,argv,values
         */
        const char *fmt = "02%d,%d,%d,%s,%s,%s,%d";

        std::string ret = log::format(fmt, id, etime, quantId, name.c_str(),
                                      argv.c_str(), values.c_str(), getpid());
        return ret;
    }
}; /* ----------  end of struct __e2lAnalse  ---------- */

typedef struct __e2lAnalse e2lAnalse;

struct __Analse_t {
    std::size_t size(std::thread::id _id) { return 0; }
    bool check(std::thread::id _id, e2::Int_e id)
    {
        return _analse.count(_id) > 0 && _analse.at(_id).count(id) == 1;
    }

    void init(std::thread::id _id)
    {
        BasicLock _lock(_EMute);
        std::map<e2::Int_e, e2lAnalse> val;
        _analse.insert({_id, val});
    }

    void update(std::thread::id _id, e2lAnalse ana)
    {
        BasicLock _lock(_EMute);

        if (_analse.count(_id) == 0) {
            std::map<e2::Int_e, e2lAnalse> val;
            val.insert({ana.id, ana});
            _analse.insert({_id, val});
        }
        else {
            if (_analse[_id].count(ana.id) == 0) {
                _analse[_id].insert({ana.id, ana});
            }
            else {
                _analse[_id][ana.id] = ana;
            }
        }
    }

    void addArgv(std::thread::id _id, e2::Int_e id, std::string args)
    {
        BasicLock _lock(_EMute);
        e2lAnalse ana;
        if (_analse.count(_id) == 0 || _analse[_id].count(id) == 0) {
            log::bug(" e2_analse is empty:");
        }
        else {
            if (_analse.at(_id).at(id).init == e2q::ticket_now) {
                if (_analse[_id][id].argv.length() == 0) {
                    _analse[_id][id].argv = args;
                }
                else {
                    _analse[_id][id].argv += "/" + args;
                }
            }
        }
    }
    void addValue(std::thread::id _id, e2::Int_e id, std::string args)
    {
        BasicLock _lock(_EMute);
        e2lAnalse ana;
        if (_analse.count(_id) == 0 || _analse[_id].count(id) == 0) {
            log::bug(" e2_analse is empty:");
        }
        else {
            if (_analse.at(_id).at(id).etime == e2q::ticket_now) {
                if (_analse[_id][id].values.length() == 0) {
                    _analse[_id][id].values = args;
                }
                else {
                    _analse[_id][id].values += "/" + args;
                }
            }
        }
    }

    void Save()
    {
        e2q::UtilTime ut;
        std::size_t now = ut.time();
        std::thread::id _id = std::this_thread::get_id();

        std::size_t idx = e2q::GlobalDBPtr->getId();

        e2q::Pgsql *gsql = e2q::GlobalDBPtr->ptr(idx);
        if (gsql == nullptr || _analse.count(_id) == 0) {
            e2q::GlobalDBPtr->release(idx);
            return;
        }
        for (auto ana : _analse.at(_id)) {
            auto it = ana.second;
            gsql->insert_table("analse");
            gsql->insert_field("aid", it.id);
            gsql->insert_field("quantid", it.quantId);
            gsql->insert_field("name", it.name.c_str());
            gsql->insert_field("argv", it.argv.c_str());
            gsql->insert_query(
                "verid",
                "( SELECT id FROM trade_info WHERE active = 1 ORDER "
                " BY ctime LIMIT 1) ");
            gsql->insert_field("ctime", now);
            gsql->insert_commit();
        }
        e2q::GlobalDBPtr->release(idx);
    }

private:
    std::map<std::thread::id, std::map<e2::Int_e, e2lAnalse>> _analse;
    using EMute = BasicLock::mutex_type;
    mutable EMute _EMute;

}; /* ----------  end of struct __Analse_t  ---------- */

typedef struct __Analse_t Analse_t;
inline Analse_t e2_analse;

struct __ProcessShare_t : public RingLoop {
    void add_proce(e2::Int_e id)
    {
        BasicLock _lock(_RLoopMutex);
        std::thread::id _id = std::this_thread::get_id();

        if (!check(id)) {
            _share_array.insert({id, _id});
        }
        else {
            _share_array[id] = _id;
        }
    }

    e2::Int_e length(e2::Int_e id)
    {
        std::thread::id _id;
        if (check(id)) {
            _id = _share_array.at(id);
        }
        else {
            _id = std::this_thread::get_id();
        }

        return RingLoop::length(_id, id);
    }

    e2::Int_e size(e2::Int_e id)
    {
        std::thread::id _id;
        if (check(id)) {
            _id = _share_array.at(id);
        }
        else {
            _id = std::this_thread::get_id();
        }

        return RingLoop::size(_id, id);
    }

    e2::Int_e get(e2::Int_e id, std::size_t idx)
    {
        std::thread::id _id;
        if (check(id)) {
            _id = _share_array.at(id);
        }
        else {
            _id = std::this_thread::get_id();
        }

        return RingLoop::get(_id, id, idx);
    }

private:
    bool check(e2::Int_e id) { return _share_array.count(id) == 1; }

    // 跨进程的
    // store id > thread_id
    std::map<e2::Int_e, std::thread::id> _share_array;

}; /* ----------  end of struct __ProcessShare_t  ---------- */

typedef struct __ProcessShare_t ProcessShare_t;
inline ProcessShare_t e2_share_array;

/**
 * exdi
 *
 */
// inline std::size_t exdiv_symbol;

typedef ExRD exdi_type;
// inline

struct __ExdiType {
    void add(e2::Int_e sym, exdi_type et)
    {
        BasicLock _lock(_EMute);
        if (_exdiv_map.count(sym) == 0) {
            std::vector<exdi_type> etlist;
            etlist.push_back(et);
            _exdiv_map.insert({sym, etlist});
        }
        else {
            _exdiv_map.at(sym).push_back(et);
        }
    }

    e2::Int_e cash(e2::Int_e sym)
    {
        if (_exdiv_map.count(sym) == 0) {
            return 0;
        }
        else {
            exdi_type et = _exdiv_map.at(sym).back();
            return et._cash;
        }
    }

    e2::Int_e share(e2::Int_e sym)
    {
        if (_exdiv_map.count(sym) == 0) {
            return 0;
        }
        else {
            exdi_type et = _exdiv_map.at(sym).back();
            return et._share;
        }
    }

    e2::Bool date(e2::Int_e sym)
    {
        e2::Bool _bool = e2::Bool::B_FALSE;
        const char fmt[] = "%Y%m%d";
        UtilTime ut;
        std::string ymd = "";

        if (_exdiv_map.count(sym) > 0) {
            e2::Int_e eymd = _exdiv_map.at(sym).back()._ymd;

            ymd = ut.stamptostr(ticket_now, fmt);
            e2::Int_e iymd = atoll(ymd.c_str());

            if (eymd == iymd) {
                _bool = e2::Bool::B_TRUE;
            }
        }

        return _bool;
    }
    void addPrice(e2::Int_e cfi, e2::Int_e price)
    {
        BasicLock _lock(_EMute);

        if (_exdi_price.count(cfi) == 0) {
            _exdi_price.insert({cfi, price});
        }
        else {
            _exdi_price[cfi] = price;
        }
    }
    e2::Int_e getPrice(e2::Int_e cfi)
    {
        e2::Int_e ret = 0;
        if (_exdi_price.count(cfi) == 1) {
            ret = _exdi_price.at(cfi);
        }

        return ret;
    }

private:
    //  symid, price
    std::map<e2::Int_e, e2::Int_e> _exdi_price;
    std::map<e2::Int_e, std::vector<exdi_type>> _exdiv_map;

    //  std::size_t _idx = 0;
    using EMute = BasicLock::mutex_type;
    mutable EMute _EMute;
}; /* ----------  end of struct __ExdiType  ---------- */

typedef struct __ExdiType ExdiType;

inline ExdiType ExdiSymList;

struct LogProtoPtr_t : public Producer {
    char *log(std::thread::id tid)
    {
        BasicLock _lock(_EMute);
        std::hash<std::thread::id> hasher;
        std::size_t idh = hasher(tid);
        char *ptr = nullptr;
        if (_data.count(idh) == 0) {
            ptr = (char *)calloc(elm_size, sizeof(char *));
            _data.insert({idh, ptr});
        }
        else {
            ptr = _data.at(idh);
        }
        return ptr;
    }
    void exist()
    {
        for (auto it : _data) {
            free(it.second);
            it.second = nullptr;
        }
        Producer::exist();
    }

private:
    //    e2q::Producer _elog;
    std::size_t elm_size = fldsiz(E2LScriptLogMessage, MsgType) +
                           fldsiz(E2LScriptLogMessage, logt) +
                           fldsiz(E2LScriptLogMessage, value) +
                           fldsiz(E2LScriptLogMessage, loc) +
                           fldsiz(E2LScriptLogMessage, ticket_now) +
                           fldsiz(E2LScriptLogMessage, pid) +
                           fldsiz(E2LScriptLogMessage, vname_len) +
                           fldsiz(E2LScriptLogMessage, path_len) +
                           fldsiz(E2LScriptLogMessage, alpha);
    std::map<std::size_t, char *> _data;
    using EMute = BasicLock::mutex_type;
    mutable EMute _EMute;
}; /* ----------  end of struct LogProtoPtr_t  ---------- */

typedef struct LogProtoPtr_t LogProtoPtr_t;

struct LogProto_t {
    void data(char *ptr)
    {
        idx = 0;
        _ptr = ptr;
        *(_ptr + idx) = e2l_pro_t::LOG;
        idx++;
    };
    void logtype(LogType_t lt)
    {
        *(_ptr + idx) = lt;
        idx++;
    }
    void numtype(NumberType_t nt)
    {
        *(_ptr + idx) = nt;
        idx++;
    }
    void value(std::uint64_t val)
    {
        idx += serialize_uint_t((_ptr + idx), val);
    }
    void deci(std::uint16_t d) { idx += serialize_uint_t((_ptr + idx), d); }
    void loc(std::uint32_t l)
    {
        idx += serialize_uint_t((_ptr + idx), l);

        std::uint64_t now = (std::uint64_t)e2q::ticket_now;
        idx += serialize_uint_t((_ptr + idx), now);
        std::uint32_t pid = (std::uint32_t)getpid();
        idx += serialize_uint_t((_ptr + idx), pid);
    }
    void vname(const char *vname)
    {
        int n = snprintf(NULL, 0, "%s", vname);
        if (n <= 1) {
            printf("error vname:%s\n", vname);
            return;
        }
        std::uint16_t vname_size = (std::uint16_t)n;
        if (vname_size > max_vname) {
            vname_size = max_vname;
        }
        idx += serialize_uint_t((_ptr + idx), vname_size);
        memcpy((_ptr + idx), vname, vname_size);
        idx += (std::size_t)vname_size;
    }
    void path(const char *p)
    {
        int n = snprintf(NULL, 0, "%s", p);
        if (n <= 1) {
            printf("error path:%s\n", p);

            return;
        }
        std::uint16_t path_size = (std::uint16_t)n;
        if (path_size > max_path) {
            path_size = max_path;
        }
        // std::cout << "path size:" << path_size << std::endl;
        idx += serialize_uint_t((_ptr + idx), path_size);
        memcpy((_ptr + idx), p, path_size);
        idx += (std::size_t)path_size;
    }
    std::size_t size() { return idx; }

    RdKafka::Headers *header(std::thread::id _id)
    {
        std::stringstream ssId;
        RdKafka::Headers *headers = RdKafka::Headers::create();
        /*
         * Produce message
         */

        ssId.str("");
        ssId.clear();

        ssId << _id;

        headers->add("thread_id", ssId.str());
        return headers;
    }

private:
    std::size_t idx = 0;
    char *_ptr;
    std::uint16_t max_vname = 50;
    std::uint16_t max_path = 205;
}; /* ----------  end of struct LogProto_t  ---------- */

typedef struct LogProto_t LogProto_t;

#ifdef KAFKALOG
inline LogProtoPtr_t elog;
#define E2LOG(dptr, dsize, header)                        \
    ({                                                    \
        do {                                              \
            if (e2q::FinFabr != nullptr &&                \
                e2q::FinFabr->_source.length() > 0) {     \
                e2q::elog.data(dptr, dsize, header);      \
            }                                             \
            else if (e2q::FixPtr != nullptr &&            \
                     e2q::FixPtr->_source.length() > 0) { \
                e2q::elog.data(dptr, dsize, header);      \
            }                                             \
            else {                                        \
                std::hash<std::thread::id> hasher;        \
                std::size_t idh = hasher(tid);            \
                fprintf(stderr, "%s, %ld\n", dptr, idh);  \
            }                                             \
        } while (0);                                      \
    })

#else

#define E2LOG(arg, tid)                                     \
    ({                                                      \
        do {                                                \
            std::hash<std::thread::id> hasher;              \
            std::size_t idh = hasher(tid);                  \
            fprintf(stderr, "%s, %ld\n", arg.c_str(), idh); \
        } while (0);                                        \
    })
#endif
}  // namespace e2q
#endif /* ----- #ifndef UTIL_INLINE_INC  ----- */
