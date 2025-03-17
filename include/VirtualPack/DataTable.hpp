/*
 * =====================================================================================
 *
 *       Filename:  DataTable.hpp
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  2022年09月20日 17时50分37秒
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

#ifndef DATATABLE_INC
#define DATATABLE_INC
#include <sstream>
#include <string>
namespace e2q {

/*
 * =====================================================================================
 *        Class:  DataTable
 *  Description:
 * =====================================================================================
 */
class DataTable {
public:
    /* ====================  LIFECYCLE =======================================
     */
    DataTable(); /* constructor */

    /* ====================  ACCESSORS =======================================
     */

    /* ====================  MUTATORS =======================================
     */
    std::string right(const std::string s, const int w);
    std::string left(const std::string s, const int w);
    std::string center(const std::string s, const int w);
    std::string prd(const double x, const int decDigits, const int width);
    std::string prd(const double x, const int decDigits);
    /* ====================  OPERATORS
     * =======================================
     */
    void print();

protected:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */

}; /* -----  end of class DataTable  ----- */

}  // namespace e2q
#endif /* ----- #ifndef DATATABLE_INC  ----- */
