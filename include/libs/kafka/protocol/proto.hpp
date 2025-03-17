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
#include <cmath>
#include <cstddef>
#include <cstdint>
namespace e2q {

#define BUFF_SIZE 8192
#define TimeStampSize 4

#ifdef NUMBER_DECI
inline double_t _deci = NUMBER_DECI;
#else
/**
 * 精度
 */
static std::size_t _scale = 3;
inline double_t _deci = std::pow(10, _scale);
#endif

template <unsigned nbits>
struct uint {};

template <>
struct uint<2> {
    using type = uint16_t;
};
template <>
struct uint<4> {
    using type = uint16_t;
};
template <>
struct uint<8> {
    using type = uint32_t;
};

#define fldsiz(name, field) (sizeof(((struct name*)0)->field))

enum e2l_pro_t {
    INIT = 'I',
    XDXR = 'X',
    SUSPEND = 'S',
    TICK = 'T',
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

struct SystemInitMessage : public BaseMessage {
    char Stock[9] = {0};
    std::uint32_t CfiCode = 0;
    char Itype = 'i';
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

struct MarketTickMessage : public BaseMessage {
    std::uint32_t CfiCode = 0;
    std::uint64_t unix_time = 0;
    std::uint16_t frame = 0;
    char side = 'B';
    std::uint64_t price = 0;
    std::uint64_t qty = 0;
    std::uint32_t number = 0;
}; /* ----------  end of struct MarketTickMessage  ---------- */

typedef struct MarketTickMessage MarketTickMessage;

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
    char alpha[256] = {0};
}; /* ----------  end of struct E2LScriptLogMessage  ---------- */

typedef struct E2LScriptLogMessage E2LScriptLogMessage;

}  // namespace e2q
#endif /* ----- #ifndef PROTO_INC  ----- */
