/*
 * =====================================================================================
 *
 *       Filename:  nbo.cpp
 *
 *    Description:  nbo
 *
 *        Version:  1.0
 *        Created:  2025/02/21 16时44分59秒
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
#include "libs/kafka/protocol/nbo.hpp"

#include <cstddef>
namespace e2q {
uint16_t bswap_16(uint16_t value)
{
    return static_cast<uint16_t>(((value) & 0xff) << 8 | ((value) >> 8));
}

uint32_t bswap_32(uint32_t value)
{
    return (
        (static_cast<uint32_t>(

             bswap_16(static_cast<uint16_t>((value) & 0xffff))

                 )
         << 16) |
        static_cast<uint32_t>(bswap_16(static_cast<uint16_t>((value) >> 16))));
}

uint64_t bswap_64(uint64_t value)
{
    return (
        (static_cast<uint64_t>(
             bswap_32(static_cast<uint32_t>((value) & 0xffffffff)))
         << 32) |
        static_cast<uint64_t>(bswap_32(static_cast<uint32_t>((value) >> 32))));
}

uint16_t parse_uint16(char* a)
{
    return bswap_16(*(reinterpret_cast<uint16_t*>(a)));
    // return ntohs(*(reinterpret_cast<uint16_t *>(a)));
}

uint32_t parse_uint32(char* a)
{
    return bswap_32(*(reinterpret_cast<uint32_t*>(a)));
    // return ntohl(*(reinterpret_cast<uint32_t *>(a)));
}

uint64_t parse_uint64(char* a)
{
    return bswap_64(*(reinterpret_cast<uint64_t*>(a)));
    // return ntohl(*(reinterpret_cast<uint64_t *>(a)));
}

uint64_t parse_ts(char* a)
{
    uint64_t m =
        (static_cast<uint64_t>(bswap_16(*(reinterpret_cast<uint16_t*>(a)))))
        << 32;
    uint64_t n =
        static_cast<uint64_t>(bswap_32(*(reinterpret_cast<uint32_t*>(a + 2))));

    return (m | n);
}

}  // namespace e2q
