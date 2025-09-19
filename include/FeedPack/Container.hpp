/*
 * =====================================================================================
 *
 *       Filename:  Container.hpp
 *
 *    Description:  Container
 *
 *        Version:  1.0
 *        Created:  2022年08月02日 17时43分26秒
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

#ifndef CONTAINER_INC
#define CONTAINER_INC

#include <atomic>
#include <cstddef>
#include <memory>
#include <vector>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "FeedPack/foreign.hpp"
#include "Toolkit/Norm.hpp"
#include "assembler/BaseType.hpp"
namespace e2q {

/*
 * =====================================================================================
 *        Class:  Container
 *  Description:  生成不同的周期值
 * =====================================================================================
 */

/*
 * =====================================================================================
 *        Class:  CellShape
 *  Description:
 *  周期是多个 data ,以后再优化
 * =====================================================================================
 */

struct __CellShape {
    size_t idx;            // data idx
    e2::TimeFrames frame;  //
    std::shared_ptr<SilkPermit<SeqType>> data;
}; /* ----------  end of struct CellShape  ---------- */

typedef struct __CellShape CellShape;

class Container : public MessageShare::StashPack<e2q::SeqType> {
public:
    /* ====================  LIFECYCLE =======================================
     */
    Container()
    {
        _listener.store(0, std::memory_order_release);
    }; /* constructor */

    /* ====================  ACCESSORS =======================================
     */

    /* ====================  MUTATORS =======================================
     */
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
    /**
     * init cell
     */
    void Cell(std::vector<e2::TimeFrames> &, std::vector<TradeTime> &);

    void InitCell();

    /**
     * get data
     */
    SeqType getData(std::size_t id, e2::TimeFrames);

    int push(std::array<e2q::SeqType, trading_protocols> &);

    SeqType aquire();
    std::size_t now() { return _now; }
    void wait_for();
    void emit();
    void quit();
    bool runing();
    int rows(std::size_t id, std::size_t timeframe);
    int writed(std::size_t id, std::size_t timeframe);
    int tail(std::array<SeqType, ohlc_column> &);
    int read(std::array<SeqType, ohlc_column> &, std::size_t, std::size_t,
             std::size_t);
    std::size_t idx(std::size_t, std::size_t);
    template <typename std::size_t Nr = 10>
    int reads(std::array<std::array<SeqType, ohlc_column>, Nr> &ohlcs,
              std::size_t stock, std::size_t timeframe)

    {
        int ret = -1;
        std::size_t m = 0;

        if (_cells.count(stock) == 0) {
            elog::bug("but stock:", stock);
            return ret;
        }
        for (auto cell : _cells.at(stock)) {
            if (cell.frame == timeframe) {
                ret = _cells.at(stock).at(m).data->read(&ohlcs);

                break;
            }
            m++;
        }
        return ret;
    };
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
    void CheckDefFrame(SeqType);
    std::size_t deviation(std::size_t timestamp, size_t timeFlag);

    void dump();
    void logs(std::array<std::array<SeqType, ohlc_column>, 5> trad_data);
    /* ====================  DATA MEMBERS
     * ======================================= */
    trigger_ptr _trigger = std::make_shared<Trigger>();

    atomic_seqtype _sequence;
    atomic_seqtype _listener;
    SeqType inc = 1;

    _Resource_ptr _source_ptr{nullptr};
    /**
     * MarketInfo symbol id
     */
    // symbol id
    std::map<size_t, std::vector<CellShape>> _cells;

    std::size_t _now = 0;

    std::vector<e2::TimeFrames> _time_frames;
    std::vector<TradeTime> _trade_time;
}; /* -----  end of class Container  ----- */
MsgStash(Container);

/**
 * Stash memory
 */
extern std::shared_ptr<ContainerStash> e2l_cnt;

}  // namespace e2q
#endif /* ----- #ifndef CONTAINER_INC  ----- */

