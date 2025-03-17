/*
 * =====================================================================================
 *
 *       Filename:  nbo.hpp
 *
 *    Description:  nbo
 *
 *        Version:  1.0
 *        Created:  2025/02/21 16时42分53秒
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

#ifndef NBO_INC
#define NBO_INC
#include <cstddef>
#include <cstdint>
namespace e2q {
std::uint16_t bswap_16(std::uint16_t value);
std::uint32_t bswap_32(std::uint32_t value);
std::uint64_t bswap_64(std::uint64_t value);

std::uint16_t parse_uint16(char* a);
std::uint32_t parse_uint32(char* a);
std::uint64_t parse_uint64(char* a);

std::uint64_t parse_ts(char* a);

template <typename T, std::size_t N = 0>
size_t parse_uint_t(const void* buffer, T& value)
{
    std::size_t len = sizeof(T);
    size_t next = len - 1 - N;

    for (size_t m = 0; m < len; m++) {
        if (m >= (len - N)) {
            ((uint8_t*)&value)[m] = 0;
        }
        else {
            ((uint8_t*)&value)[m] = ((const uint8_t*)buffer)[next];
            next--;
        }
    }

    return (len - N);
}

template <typename T, std::size_t N = 0>
size_t serialize_uint_t(void* buffer, T& value)
{
    std::size_t len = sizeof(T) - N;
    std::size_t next = len - 1;
    for (size_t m = 0; m < len; m++) {
        ((uint8_t*)buffer)[m] = ((uint8_t*)&value)[next];
        next--;
    }

    return len;
}

}  // namespace e2q
#endif /* ----- #ifndef NBO_INC  ----- */
