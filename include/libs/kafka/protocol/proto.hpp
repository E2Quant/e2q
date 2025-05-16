/*
 * =====================================================================================
 *
 *       Filename:  proto.hpp
 *
 *    Description:  proto
 *
 *        Version:  1.0
 *        Created:  2025/02/22 10时44分19秒
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

#ifndef PROTO_INC
#define PROTO_INC

#include <cstddef>
#include <cstdint>
#include <map>
#include <vector>

#include "Toolkit/Norm.hpp"

namespace e2q {

enum e2l_pro_t {
    INIT = 'I',
    XDXR = 'X',
    SUSPEND = 'S',
    TICK = 'T',
    CUSTOM = 'C',
    EXIT = 'E',
    LOG = 'L'
}; /* ----------  end of enum e2l_pro_t  ---------- */

typedef enum e2l_pro_t E2l_pro_t;

enum aligned_t {
    UNDER = 'U',  // 进行中
    PULL = 'P'    // 完成
}; /* ----------  end of enum aligned_t  ---------- */

typedef enum aligned_t Aligned_t;

struct BaseMessage {
    char MsgType;
    char Aligned;
}; /* ----------  end of struct BaseMessage  ---------- */

typedef struct AlignedMessage AlignedMessage;

#define E2QSTOCK_LENGTH 10

// index or symbol
enum InitType {
    INDEX = 'i',
    TRADE = 't'
}; /* ----------  end of enum InitType  ---------- */

typedef enum InitType InitType;
struct SystemInitMessage : public BaseMessage {
    char Stock[E2QSTOCK_LENGTH] = {0};
    std::uint32_t CfiCode = 0;
    char Itype = InitType::INDEX;
    std::uint32_t OfferTime = 0;
}; /* ----------  end of struct SystemInitMessage  ---------- */

typedef struct SystemInitMessage SystemInitMessage;

/**
 * 分红 配股
 */

struct StockAXdxrMessage : public BaseMessage {
    std::uint32_t CfiCode = 0;
    std::uint16_t year = 0;
    std::uint16_t month = 0;
    std::uint16_t day = 0;
    std::uint16_t category = 0;
    std::uint32_t fenhong = 0;
    std::uint32_t songzhuangu = 0;
    std::uint32_t outstanding = 0;
    std::uint32_t outstandend = 0;
    std::uint32_t mrketCaping = 0;
    std::uint16_t uint = 0;  // 10 送，还是 100 送
}; /* ----------  end of struct StockAXdxrMessage  ---------- */

typedef struct StockAXdxrMessage StockAXdxrMessage;

// if qty == 0   // 涨跌停 不撮合交易
struct MarketTickMessage : public BaseMessage {
    std::uint32_t CfiCode = 0;
    std::uint64_t unix_time = 0;
    std::uint16_t frame = 0;
    char side = 'B';          // BID OR ASK  change e2::Side
    std::uint64_t price = 0;  // last price
    std::uint64_t qty = 0;
    std::uint32_t number = 0;
}; /* ----------  end of struct MarketTickMessage  ---------- */

typedef struct MarketTickMessage MarketTickMessage;

/**
 * 定义数据
 *
 */

enum CmType {
    UINT16 = '6',
    UINT32 = '2',
    UINT64 = '4'
}; /* ----------  end of enum CmType  ---------- */

typedef enum CmType CmType;

struct CustomMessage : public BaseMessage {
    std::uint32_t CfiCode = 0;
    std::uint16_t index = 0;  // 可以用作 dict 的索引
    std::uint16_t size = 0;
    char type;  // CmType
}; /* ----------  end of struct CustomMessage  ---------- */

typedef struct CustomMessage CustomMessage;

#define GetMsgData(array_data, index, value_uint)                    \
    ({                                                               \
        do {                                                         \
            for (std::size_t m = 0; m < cmsg.size; m++) {            \
                idx += parse_uint_t(ptr + idx, value_uint);          \
                array_data.push_back(index, m, (SeqType)value_uint); \
                if (idx >= (std::size_t)sz) {                        \
                    break;                                           \
                }                                                    \
            }                                                        \
        } while (0);                                                 \
    })

struct __CustomMsgStore {
    void init(std::uint32_t cfi, std::uint16_t idx, std::size_t len)
    {
        _cfi = cfi;
        _index = idx;
        if (_datas.count(cfi) == 0) {
            BasicLock _lock(_CMute);
            std::vector<SeqType> v(len);
            std::map<std::uint16_t, std::vector<SeqType>> vi;
            vi.insert({idx, v});
            _datas.insert({cfi, vi});
        }

        if (_datas[cfi].count(idx) == 0) {
            BasicLock _lock(_CMute);
            std::vector<SeqType> v(len);
            _datas[cfi].insert({idx, v});
        }
    }
    void push_back(std::uint16_t idx, std::size_t pos, SeqType data)
    {
        std::size_t _pos = pos % _datas[_cfi][idx].size();
        _datas[_cfi][idx][_pos] = data;
    }

    std::size_t size(std::uint32_t cfi, std::uint16_t idx)
    {
        if (_datas.count(cfi) == 0) {
            return 0;
        }
        if (_datas[cfi].count(idx) == 0) {
            return 0;
        }
        return _datas[cfi][idx].size();
    }

    SeqType get(std::uint32_t cfi, std::uint16_t idx, std::size_t pos)
    {
        if (_datas.count(cfi) == 0) {
            return 0;
        }
        if (_datas[cfi].count(idx) == 0) {
            return 0;
        }
        if (pos >= _datas[cfi][idx].size()) {
            return 0;
        }
        return _datas[cfi][idx][pos];
    }

private:
    std::map<std::uint32_t, std::map<std::uint16_t, std::vector<SeqType>>>
        _datas;
    std::uint32_t _cfi = 0;
    std::uint16_t _index = 0;
    using CMute = BasicLock::mutex_type;
    mutable CMute _CMute;
}; /* ----------  end of struct __CustomMsgStore  ---------- */

typedef struct __CustomMsgStore CustomMsgStore;

/**
 * log proto
 */
enum LogType_t {
    BASE = 'B',
    LINE = 'L',
    PRO = 'P',
    TIME = 'T'
}; /* ----------  end of enum LogType_t  ---------- */

typedef enum LogType_t LogType_t;

enum NumberType_t {
    NEGATIVE = 'N',  // -1121
    POSITIVE = 'P'   // 2323
}; /* ----------  end of enum NumberType_t  ---------- */

typedef enum NumberType_t NumberType_t;

#define DYNAMIC_ALPHA 256

struct E2LScriptLogMessage {
    char MsgType;
    char logt;
    char numt;
    std::uint64_t value;
    std::uint16_t deci = 0;
    std::uint32_t loc = 0;
    std::uint64_t ticket_now = 0;
    std::uint32_t pid = 0;
    std::uint16_t vname_len = 0;
    std::uint16_t path_len = 0;
    char alpha[DYNAMIC_ALPHA] = {0};
}; /* ----------  end of struct E2LScriptLogMessage  ---------- */

typedef struct E2LScriptLogMessage E2LScriptLogMessage;

}  // namespace e2q
#endif /* ----- #ifndef PROTO_INC  ----- */
