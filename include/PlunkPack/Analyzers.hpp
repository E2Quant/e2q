/*
 * =====================================================================================
 *
 *       Filename:  Analyzers.hpp
 *
 *    Description:  Analyzers
 *
 *        Version:  1.0
 *        Created:  2022年08月02日 17时29分49秒
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

#ifndef ANALYZERS_INC
#define ANALYZERS_INC
#include "E2LScript/ExternClazz.hpp"
#include "PlunkPack/foreign.hpp"
#include "assembler/BaseType.hpp"
#include "libs/bprinter/table_printer.h"
namespace e2q {

/*
 * =====================================================================================
 *        Class:  Analyzers
 *  Description:
 * =====================================================================================
 */
class Analyzers {
public:
    /* ====================  LIFECYCLE =======================================
     */
    Analyzers(SeqType price, SeqType pre)
        : _current_price(price), _pre_price(pre) {

          }; /* constructor */

    SeqType result(e2::Int_e qid, e2::Int_e analy);
    /* ====================  ACCESSORS =======================================
     */

    /* ====================  MUTATORS =======================================
     */
    void run(OrderInfo info, SeqType atime);
    void ReturnAnn();
    /* ====================  OPERATORS =======================================
     */

protected:
    /* ====================  METHODS =======================================
     */

    /* ====================  DATA MEMBERS
     * ======================================= */

private:
    /* ====================  METHODS =======================================
     */

    void equity();
    /* ====================  DATA MEMBERS
     * ======================================= */

    SeqType _current_price;
    SeqType _pre_price;

    e2::Int_e _cumqty;
    e2::Int_e _avgpx;
    e2::Int_e _leavesqty;

    e2::Int_e _qty;
    e2::Int_e _price;

}; /* -----  end of class Analyzers  ----- */

/*
 * ================================
 *        Class:  AnalyBeam
 *  Description:
 * ================================
 */
struct AnalyBeam : public FuncBeamClass<FuncSignal> {
    void callback(std::shared_ptr<FuncSignal> beam)
    {
        if (beam == nullptr || beam->id != SigId::_feedId) {
            // log::bug("FuncSignal is null");

            return;
        }
        FIX_PTR_IS_NULL();

        e2q::bridge::ReceiveType<DisruptorStashSharePtr> fun =
            [](DisruptorStashSharePtr ptr) {
                if (ptr == nullptr) {
                    log::bug("DisruptorStashSharePtr is nullptr");
                    return;
                }
                auto thread_fun = [ptr]() {
                    ptr->id = 19;
                    ptr->data_ptr->name(
                        "AnalyBeam slave [ disruptor -> analy]");
                    bool filled = true;
                    // OrderInfo oinfo;
                    SeqType pre_price = 0;

                    auto call_fun = [ptr, &filled, &pre_price](
                                        e2q::SeqType start, e2q::SeqType end) {
                        e2::Int_e quantId = 0;
                        OrderInfo oinfo;
                        oinfo.symbol = -1;

                        if (filled && (FixPtr->_OrderIds.size() > 0)) {
                            for (auto it = FixPtr->_OrderIds.begin();
                                 it != FixPtr->_OrderIds.end(); ++it) {
                                // log::bug(it->first, " , ",
                                // it->second.symbol);
                                quantId = it->first;
                                for (auto k =
                                         FixPtr->_OrderIds[quantId].begin();
                                     k != FixPtr->_OrderIds[quantId].end();
                                     ++k) {
                                    oinfo = k->second;
                                    break;
                                }

                                break;
                            }
                            filled = false;
                        }
                        /**
                         * trading_protocols
                         * 最后一个值是 symbol_id,以区分多个股票
                         */
                        std::array<e2q::SeqType, trading_protocols> rec_data;

                        std::size_t rows = ptr->data_ptr->rows();
                        for (int m = start; m < end; m++) {
                            size_t row = m % rows;

                            int ret = ptr->data_ptr->fixed(&rec_data, row);

                            if (ret == -1) {
                                log::bug(ptr->data_ptr->name(),
                                         " row error:", row, " total:", rows,
                                         "start:", start, " end:", end);

                                continue;
                            }
                            if (oinfo.symbol == rec_data[Trading::t_stock]) {
                                /* log::info("stock:",
                                 * rec_data[Trading::t_stock], */
                                /*           " price:", */
                                /*           NUMBERVAL(rec_data[Trading::t_price]),
                                 */
                                /*           " avgpx:", oinfo.avgpx, */
                                /*           " price:", oinfo.price */

                                /* ); */
                                double price = rec_data[Trading::t_price];
                                // e2::Int_e atime = rec_data[Trading::t_time];
                                /* Analyzers analy(price, pre_price); */
                                /* analy.run(oinfo, atime); */
                                pre_price = price;
                            }
                        }
                    };
                    ptr->data_ptr->wait_for(call_fun);
                };
                THREAD_FUN(thread_fun);
            };  // -----  end lambda  -----
        beam->receive(fun);
    }
    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  shareptr
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    void shareptr(_Resource_ptr ptr)
    {
        _source_ptr = std::move(ptr);

    } /* -----  end of function shareptr  ----- */
}; /* -----  end of class AnalyBeam  ----- */

}  // namespace e2q
#endif /* ----- #ifndef ANALYZERS_INC  ----- */
