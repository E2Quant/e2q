/*
 * =====================================================================================
 *
 *       Filename:  IDGenerator.hpp
 *
 *    Description:  IDGenerator
 *
 *        Version:  1.0
 *        Created:  2024年01月10日 14时37分03秒
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

#ifndef IDGENERATOR_INC
#define IDGENERATOR_INC

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
// define something for Windows (32-bit and 64-bit, this part is common)
#ifdef _WIN64
// define something for Windows (64-bit only)
#else
// define something for Windows (32-bit only)
#endif
#elif __APPLE__

#include <CoreFoundation/CoreFoundation.h>
#if TARGET_IPHONE_SIMULATOR
// iOS, tvOS, or watchOS Simulator
#elif TARGET_OS_MACCATALYST
// Mac's Catalyst (ports iOS API into Mac, like UIKit).
#elif TARGET_OS_IPHONE
// iOS, tvOS, or watchOS device
#elif TARGET_OS_MAC
// Other kinds of Apple platforms
#else
#error "Unknown Apple platform"
#endif
#elif __ANDROID__
// Below __linux__ check should be enough to handle Android,
// but something may be unique to Android.
#elif __linux__
#include <uuid/uuid.h>

// linux
#elif __unix__  // all unices not caught above
#include <uuid/uuid.h>

// Unix
#elif defined(_POSIX_VERSION)
// POSIX
#else
#error "Unknown compiler"
#endif

#include <string>
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  UUidGen
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::string UUidGen(); /* -----  end of function UUidGen  ----- */
}  // namespace e2q
#endif /* ----- #ifndef IDGENERATOR_INC  ----- */
