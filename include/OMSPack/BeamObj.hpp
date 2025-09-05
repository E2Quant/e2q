/*
 * =====================================================================================
 *
 *       Filename:  BeamObj.hpp
 *
 *    Description:  BeamObj
 *
 *        Version:  1.0
 *        Created:  2024年02月27日 09时41分37秒
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

#ifndef BEAMOBJ_INC
#define BEAMOBJ_INC
#include <array>
#include <cstdint>
#include <memory>
#include <thread>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "OMSPack/FixApplication.hpp"
#include "OMSPack/foreign.hpp"
#include "SessionGlobal.hpp"
#include "Toolkit/Norm.hpp"
#include "Toolkit/UtilTime.hpp"
namespace e2q {
struct FixBeam : public FuncBeamClass<FuncSignal> {
    void callback(std::shared_ptr<FuncSignal> beam)
    {
        if (beam == nullptr || beam->id != SigId::_strategy_id) {
            return;
        }
        bridge::ReceiveType<DisruptorStashSharePtr> rfun =
            [this](DisruptorStashSharePtr ptr) {
                if (ptr == nullptr) {
                    log::echo("ptr is nullptr");
                    return;
                }

                auto thread_fun = [this](DisruptorStashSharePtr ptr) {
                    // log::echo("fix ok ptr ");

                    ptr->data_ptr->name(
                        "FixBeam slave [disruptor -> fixaccount]");
                    ptr->id++;

                    auto call_fun = [this, &ptr](SeqType start, SeqType end) {
                        std::array<SeqType, trading_protocols> rec_data;
                        for (SeqType m = start; m < end; m++) {
                            size_t row = m % ptr->data_ptr->rows();
                            int ret = ptr->data_ptr->fixed(&rec_data, row);
                            if (ret == -1) {
                                log::bug(ptr->data_ptr->name(),
                                         " row error:", row, " m:", m,
                                         " start:", start, " end:", end);
                                continue;
                            }
                            SeqType symbol = rec_data[Trading::t_stock];
                            SeqType price = rec_data[Trading::t_price];

                            if (price == 0) {
                                /**
                                 * 停牌
                                 */
                                continue;
                            }

                            std::size_t exsit = FinFabr->_stock.count(symbol);
                            if (exsit == 0) {
                                FinFabr->_stock.insert({symbol, rec_data});
                            }
                            else {
                                FinFabr->_stock[symbol] = rec_data;
                            }

                            this->CallFun(rec_data);
                        }
                    };  // -----  end lambda  -----
                    ptr->data_ptr->wait_for(call_fun);
                };  // -----  end lambda  -----
                THREAD_FUN(thread_fun, ptr);
            };  // -----  end lambda  -----
        beam->receive(rfun);
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

    /*
     * ===  FUNCTION  =============================
     *
     *         Name:  CallFun
     *  ->  void *
     *  Parameters:
     *  - size_t  arg
     *  Description:
     *
     * ============================================
     */
    void CallFun(std::array<SeqType, trading_protocols>& rec_data)
    {
        e2q::FixGuard _fix;
        SeqType stock = rec_data[Trading::t_stock];

        for (auto it = SessionSymList.begin(); it != SessionSymList.end();
             it++) {
            if (it->second.empty()) {
                // 应该是  OnlyEA::LOCKFOREA, 还没有来得及订阅
                //  log::info("sid:", it->first.getTargetCompID().getValue(),
                //           " not has symbol:", stock);
                continue;
            }

            if (stock == 0) {
                // 如果没有申请 symbole 就只发送 index 的 symbole
                // OnlyEA::FORANLYONE 否则会出错
                // && 先不做这儿的处理
                // FinFabr->_fix_symbol_only_for_ea == OnlyEA::FORANLYONE
                _fix.MarketMessage(it->first, rec_data);
                continue;
            }

            // session select symbols

            for (auto id : it->second) {
                if (id == 0) {
                    continue;
                }
                if (FinFabr->_fix_symbols.count(id) == 0) {
                    log::bug("bug stock:", id);
                    continue;
                }

                if (id == (std::size_t)stock) {
                    _fix.MarketMessage(it->first, rec_data);

                    break;
                }
            }
        }

    } /* -----  end of function CallFun  ----- */

private:
    std::shared_ptr<OrderMatcher> _orderMatcher = nullptr;
}; /* ----------  end of struct FixBeam  ---------- */

typedef struct FixBeam FixBeam;
}  // namespace e2q
#endif /* ----- #ifndef BEAMOBJ_INC  ----- */
