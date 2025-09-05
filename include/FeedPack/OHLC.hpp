/*
 * =====================================================================================
 *
 *       Filename:  OHLC.hpp
 *
 *    Description: OHLC
 *
 *        Version:  1.0
 *        Created:  2022年07月28日 11时45分58秒
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

/*
 * =====================================================================================
 *        Class:  OHLC
 *  Description:
 * =====================================================================================
 */

#ifndef OHLC_INC
#define OHLC_INC
#include <bits/stdint-uintn.h>

#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "E2L/E2LType.hpp"
#include "FeedPack/Container.hpp"
#include "FeedPack/foreign.hpp"
namespace e2q {

/***
 * Default ohlc type
 *              time_index, _open, _high, _low, _close, _vol
 */
#define OHLC_TYPE_UNIT _ohlc_t, _ohlc_t, _ohlc_t, _ohlc_t, _ohlc_t, _ohlc_t

/**
 * extend BAR index
 */
#define DEF_OHLC_T "ohlc_t"
#define DEF_OPEN "open"
#define DEF_HIGH "high"
#define DEF_LOW "low"
#define DEF_CLOSE "close"
#define DEF_VOLUME "volume"

/*
 * =====================================================================================
 *        Class:  OHLCThread
 *  Description:
 * =====================================================================================
 */

class OHLCThread : public Loom {
public:
    /* ====================  LIFECYCLE =======================================
     */
    OHLCThread() {}; /* constructor */
    OHLCThread(Mulberry<SeqType> mul) : Loom(mul) {}

    OHLCThread(OHLCThread& _other)
    {
        if (*this != _other) {
            released();
        }
        *this = _other;
    }

    /* ====================  ACCESSORS =======================================
     */
    void push(const std::array<_ohlc_t, ohlc_column>&);
    void push(const _ohlc_t*);

    /**
     * index timestamp
     *
     * index Microsecond
     */
    // void index(_OHLCThread_t m);

    /**
     * day = "2022-09-10 11:23:54"
     * fmt = "%Y-%m-%d %H:%M:%S"
     */
    // void index(const char *day, const char *fmt);

    /* void Microsecond(uint64_t m); */
    /* void Nanosecond(uint64_t n); */
    /* void Picosecond(uint64_t p); */
    /* ====================  MUTATORS =======================================
     */
    /***
     *  bar period
     */
    const _ohlc_t GetOpen(size_t pos);
    const _ohlc_t GetHigh(size_t pos);
    const _ohlc_t GetLow(size_t pos);
    const _ohlc_t GetClose(size_t pos);
    const _ohlc_t GetVolume(size_t pos);

    /* ====================  OPERATORS =======================================
     */
    const OHLCThread& operator[](Scope scope)
    {
        Loom::operator[](scope);

        return *this;
    }

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
    trigger_ptr _trigger = std::make_shared<Trigger>();

    std::size_t ohlc_m = 0;

}; /* -----  end of class OHLCThread  ----- */

MsgStash(OHLCThread);

/**
 * 可能存在接收不完整的情况，到时候再检查吧
 *
 */

struct OHLCBeam : public e2q::FuncBeamClass<FuncSignal> {
    void callback(std::shared_ptr<FuncSignal> beam)
    {
        if (beam == nullptr || beam->id != SigId::_feedId) {
            // log::bug("FuncSignal is null");

            return;
        }
        /**
         * tfun => Stash _in 的fun
         *
         */
        size_t type = typeid(ContainerStash).hash_code();

        ContainerStashSharePtr _cnt_ptr =
            _source_ptr->fetch<ContainerStash>(type);
        if (_cnt_ptr == nullptr) {
            log::bug("OHLCBeam ContainerStash ptr is nullptr!");
            return;
        }
        _cnt_ptr->data_ptr->shareptr(_source_ptr);
        _cnt_ptr->data_ptr->Cell(_frame, _tradetime);
        int off = _off;
        e2q::bridge::ReceiveType<DisruptorStashSharePtr> tfun =
            [_cnt_ptr, off](DisruptorStashSharePtr ptr) {
                if (ptr == nullptr) {
                    log::bug("DisruptorStashSharePtr is nullptr");
                    return;
                }
                auto thread_fun = [_cnt_ptr, ptr, off]() {
                    ptr->data_ptr->name(
                        "OHLCBeam slave [disruptor -> ohlc(container stash)]");
                    ptr->id += 3;

                    /**
                     * call_fun 是在 Disruptor 的 wait_for 中使用，是 master
                     * 用的
                     */
                    auto call_fun = [_cnt_ptr, ptr, off](e2q::SeqType start,
                                                         e2q::SeqType end) {
                        /**
                         * trading_protocols
                         * 最后一个值是 symbol_id,以区分多个股票
                         */

                        std::array<e2q::SeqType, trading_protocols> rec_data;
                        if (_cnt_ptr == nullptr) {
                            log::echo("ohlc_pt is nullptr");
                            return;
                        }

                        std::size_t rows = ptr->data_ptr->rows();
                        int cnt = -1;

                        for (int m = start; m < end; m++) {
                            size_t row = m % rows;
                            int ret = ptr->data_ptr->fixed(&rec_data, row);

                            if (ret == -1) {
                                log::bug(ptr->data_ptr->name(),
                                         " row error:", row, " m:", m,
                                         " start:", start, " end:", end);

                                continue;
                            }

                            /**
                             *在这儿 通过 OHLC 的类运算，把数据写入 某一块的
                             *Disruptor 中去，读出的使用者，在 resource
                             *通过固定的 id
                             *读出这个内存块,有必要的话，这内存块在 OHLC
                             *上面作为策略运算
                             */

                            /*
                             * check offers
                             */
                            // cnt == cfi code == index code
                            cnt = _cnt_ptr->data_ptr->push(rec_data);
                            if (cnt == -1) {
                                // log::bug("cnt == -1");
                                continue;
                            }
                            if (cnt == 0) {
                                // cnt == 0 is offers == index
                                _cnt_ptr->data_ptr->emit();
                            }
                            else {
                                if (off == e2::Offers::OF_Tick ||
                                    GlobalMainArguments.number_for_bin_read >=
                                        0) {
                                    // 历史记录，不需要有指数的
                                    _cnt_ptr->data_ptr->emit();
                                }
                            }
                        }
                    };  // -----  end lambda  -----
                    ptr->data_ptr->wait_for(call_fun);
                    _cnt_ptr->data_ptr->quit();
                };  // -----  end lambda  -----

                THREAD_FUN(thread_fun);
            };
        beam->receive(tfun);
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
    void market(std::vector<e2::TimeFrames>& frame, e2::Offers o,
                std::vector<TradeTime>& tt)
    {
        // _symId = sym;
        _frame = frame;
        _off = o;
        _tradetime = tt;
    }

private:
    // std::vector<size_t> _symId;
    std::vector<e2::TimeFrames> _frame;
    std::vector<TradeTime> _tradetime;
    e2::Offers _off;
}; /* ----------  end of struct OHLCBeam  ---------- */

typedef struct OHLCBeam OHLCBeam;

}  // namespace e2q
#endif /* ----- #ifndef OHLC_INC  ----- */

