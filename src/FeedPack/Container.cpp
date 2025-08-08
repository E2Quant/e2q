/*
 * =====================================================================================
 *
 *       Filename:  Container.cpp
 *
 *    Description:  Container
 *
 *        Version:  1.0
 *        Created:  2022年11月19日 17时34分05秒
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
#include "FeedPack/Container.hpp"

#include <unistd.h>

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <vector>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "E2LScript/e2lLead.hpp"
#include "Toolkit/Norm.hpp"

namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::Cell
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Container::Cell(std::vector<e2::TimeFrames>& tf,
                     std::vector<TradeTime>& tt)
{
    _time_frames = tf;
    _trade_time = tt;

} /* -----  end of function Container::Cell  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::InitCell
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Container::InitCell()
{
    std::vector<size_t> symId = FixPtr->_symbols;

    if (_time_frames.size() == 0) {
        log::bug("_time_frames.size() == 0");
        return;
    }

    if (_source_ptr == nullptr) {
        log::bug("source ptr is nullptr");
        return;
    }
    if (_cells.size() > 0) {
        log::echo("cells size:", _cells.size());
        return;
    }
    for (auto t : _time_frames) {
        CellShape cell;
        cell.frame = t;
        cell.idx = 0;
        cell.data = _source_ptr->MemPtr<SilkPermit<SeqType>>(ohlc_column);
        _cells[0].push_back(cell);
    }
    for (auto id : symId) {
        if (id == 0) {
            continue;
        }

        if (_cells.count(id) == 0) {
            for (auto t : _time_frames) {
                CellShape cell;
                cell.frame = t;
                cell.idx = 0;
                cell.data =
                    _source_ptr->MemPtr<SilkPermit<SeqType>>(ohlc_column);
                _cells[id].push_back(cell);
            }
        }
    }
#ifdef DEBUG
    log::info("init cell and symId size:", symId.size());
#endif
} /* -----  end of function Container::InitCell  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::push
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  return stock cfi code
 * ============================================
 */
int Container::push(std::array<e2q::SeqType, trading_protocols>& data)
{
    UtilTime ut;

#define day_second 86400

#define NowTime(t, gmt)                                         \
    ({                                                          \
        std::size_t __ret = 0;                                  \
        do {                                                    \
            __ret = (t + gmt) - ((t + gmt) % day_second) - gmt; \
        } while (0);                                            \
        __ret;                                                  \
    })
    std::size_t row = 0;
    bool pause = false;

    SeqType stock = data[Trading::t_stock];

    SeqType frame = data[Trading::t_frame];

    if (FixPtr->_current_tf == e2::TimeFrames::PERIOD_CURRENT) {
        CheckDefFrame(frame);
    }

    auto search = _cells.find(stock);
    std::array<SeqType, ohlc_column> ohlc;
    int ret = 0;
    std::size_t minute = 60;

    SeqType trad_time = data[Trading::t_time];

    // 毫秒 和秒的问题
    std::size_t timestamp = trad_time / 1000;

    // double Decimal = (millisecond - (double)timestamp) * 1000.0;
    // log::echo("start day", ut.toDate(timestamp));
    _now = trad_time;
    std::size_t intervalTime = 0;
    std::size_t startTime = 0;
    if (search == _cells.end()) {
        log::bug("not found stock:", stock, " cell size:", _cells.size());
        for (auto it : _cells) {
            log::info("code:", it.first);
        }
        return -1;
    }
    std::size_t time_flag = 0;
    std::size_t gmtTime = 0;
    if (FixPtr->_gmt > 0) {
        gmtTime = ut.offset_gmt();
    }

    if (FixPtr->_onOpen) {
        // 强制前置时间
        timestamp -= frame * minute;
    }

    for (std::size_t m = 0; m < _cells.at(stock).size(); m++) {
        if (_cells.at(stock).at(m).frame < frame) {
            // 不支持生成小于 源数据 的周期
            continue;
        }

        time_flag = _cells.at(stock).at(m).frame;

        switch (time_flag) {
            case e2::TimeFrames::PERIOD_CURRENT:
                // 0
                startTime = timestamp;
                break;
            case e2::TimeFrames::PERIOD_M1:
                // 1

            case e2::TimeFrames::PERIOD_M: {
                // 5
                intervalTime = time_flag * minute;
                startTime = timestamp - (timestamp % intervalTime);
                break;
            }
            case e2::TimeFrames::PERIOD_M15:
                // 15
            case e2::TimeFrames::PERIOD_M30:
                // 30

            case e2::TimeFrames::PERIOD_H1:
                // 60

            case e2::TimeFrames::PERIOD_H4:
                // 240
                {
                    std::size_t time_day_flag = NowTime(timestamp, gmtTime);
                    std::size_t deviation_time =
                        deviation(timestamp, time_flag);
                    startTime = time_day_flag + (deviation_time * minute);
                    // log::echo("start:", ut.toDate(startTime),
                    //           " time:", ut.toDate(timestamp),
                    //           " day_flag:", ut.toDate(time_day_flag),
                    //           " devia:", deviation_time,
                    //           " time_flag:", time_flag);
                    break;
                }

            case e2::TimeFrames::PERIOD_D1: {
                // 1440
                startTime = NowTime(timestamp, gmtTime);

                break;
            }
            case e2::TimeFrames::PERIOD_W1: {
                // 10080

                startTime = (std::size_t)ut.first_of_the_week(timestamp);
                if (FixPtr->_gmt) {
                    startTime -= ut.offset_gmt();
                }
                break;
            }
            case e2::TimeFrames::PERIOD_MN1: {
                // 43200
                startTime = (std::size_t)ut.first_of_the_month(timestamp);
                if (FixPtr->_gmt) {
                    startTime -= ut.offset_gmt();
                }

                break;
            }
            default:

                break;
        }
        /* -----  end switch  ----- */

        startTime = startTime * 1000;

        ohlc = {0};
        ret = _cells.at(stock).at(m).data->read(&ohlc);
        pause = false;

        if (stock > 0) {
            row = _cells.at(stock).at(m).data->hash_row();
            if (ohlc[OHLC_T::close_t] == 0 && row > 0) {
                pause = true;
            }
            if (pause && data[Trading::t_price] == 0) {
                return stock;
            }
        }

        if (ohlc[OHLC_T::ohlc_t] == 0 ||
            (std::size_t)ohlc[OHLC_T::ohlc_t] != startTime || ret == -1) {
            ohlc[OHLC_T::ohlc_t] = startTime;
            ohlc[OHLC_T::open_t] = data[Trading::t_price];
            ohlc[OHLC_T::high_t] = data[Trading::t_price];

            ohlc[OHLC_T::low_t] = data[Trading::t_price];
            ohlc[OHLC_T::close_t] = data[Trading::t_price];
            ohlc[OHLC_T::volume_t] = data[Trading::t_qty];
            ohlc[OHLC_T::adjclose_t] = data[Trading::t_adjprice];

            if (pause) {
                _cells.at(stock).at(m).data->push(0, ohlc);
            }
            else {
                _cells.at(stock).at(m).data->insert(ohlc);
            }
        }
        else {
            if (data[Trading::t_price] > ohlc[OHLC_T::high_t]) {
                ohlc[OHLC_T::high_t] = data[Trading::t_price];
            }

            if (ohlc[OHLC_T::low_t] > data[Trading::t_price]) {
                ohlc[OHLC_T::low_t] = data[Trading::t_price];
            }

            ohlc[OHLC_T::close_t] = data[Trading::t_price];
            ohlc[OHLC_T::adjclose_t] = data[Trading::t_adjprice];

            if (FixPtr->_volume_append) {
                ohlc[OHLC_T::volume_t] += data[Trading::t_qty];
            }

            _cells.at(stock).at(m).data->push(0, ohlc);
        }

        _cells.at(stock).at(m).idx++;
    }

    return stock;
} /* -----  end of function Container::push  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::CheckDefFrame
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Container::CheckDefFrame(SeqType frame)
{
    bool exist_tf = true;
    FixPtr->_current_tf = EnumValid(frame);
    if (std::find(std::begin(e2q::FixPtr->_tf), std::end(e2q::FixPtr->_tf),
                  FixPtr->_current_tf) == std::end(e2q::FixPtr->_tf)) {
        e2q::FixPtr->_tf.push_back(FixPtr->_current_tf);
    }
    exist_tf = true;
    for (auto it : _cells[0]) {
        if (it.frame == FixPtr->_current_tf) {
            exist_tf = false;
        }
    }

    if (exist_tf) {
        CellShape cell;
        cell.frame = FixPtr->_current_tf;
        cell.idx = 0;
        cell.data = _source_ptr->MemPtr<SilkPermit<SeqType>>(ohlc_column);
        _cells[0].push_back(cell);
    }

    std::vector<size_t> symId = FixPtr->_symbols;

    for (auto id : symId) {
        if (id == 0) {
            continue;
        }

        if (_cells.count(id) == 1) {
            exist_tf = true;
            for (auto it : _cells[id]) {
                if (it.frame == FixPtr->_current_tf) {
                    exist_tf = false;
                }
            }

            if (exist_tf) {
                CellShape cell;
                cell.frame = FixPtr->_current_tf;
                cell.idx = 0;
                cell.data =
                    _source_ptr->MemPtr<SilkPermit<SeqType>>(ohlc_column);
                _cells[id].push_back(cell);
            }
        }
    }
} /* -----  end of function Container::CheckDefFrame  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::deviation
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::size_t Container::deviation(std::size_t timestamp, size_t timeFlag)
{
    std::vector<TradeTime> trade_time = FixPtr->_tradetime;
    std::size_t ret = 0;
    if (trade_time.size() == 0) {
        log::bug(" trade time is empty!");
        return ret;
    }
    UtilTime ut;
    const char hh[] = "%H";
    const char mm[] = "%M";
    std::string hhr = ut.stamptostr(timestamp, hh);
    std::size_t hour = atol(hhr.c_str());
    std::string mmr = ut.stamptostr(timestamp, mm);
    std::size_t min = atol(mmr.c_str());

    std::size_t now_total_time = hour * 60 + min;

    //  log::echo("tick hour:", hour, " min:", min, " now_total:",
    //  now_total_time);

    size_t closeTimeflag = 0, openTimeflag = 0;
    size_t n = 0;
    int valuby = 0;
    int num = 0;
    std::size_t start = 0, stop = 0;

    for (auto it : trade_time) {
        closeTimeflag = it.close_hour * 60 + it.close_min;
        openTimeflag = it.open_hour * 60 + it.open_min;

        if (now_total_time >= openTimeflag && now_total_time <= closeTimeflag) {
            // log::echo("n:", n, " now_total_time:", now_total_time,
            //           " open:", openTimeflag, " close:", closeTimeflag);
            if (n > 0) {
                start = (now_total_time / timeFlag) * timeFlag;
                stop = (now_total_time / timeFlag + 1) * timeFlag;
                if (start <= openTimeflag && stop >= closeTimeflag) {
                    return start;
                }
            }

            valuby = now_total_time - openTimeflag;
            num = valuby / timeFlag;
            if (num == 0) {
                ret = openTimeflag;
            }
            else {
                ret = openTimeflag + (num * timeFlag);
            }

            return ret;
        }
        n++;
    }
    return ret;
} /* -----  end of function Container::deviation  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::aquire
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
SeqType Container::aquire()
{
    return _sequence.load(std::memory_order_acquire);
} /* -----  end of function Container::aquire  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::wait_for
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Container::wait_for()
{
    SeqType num = _listener.fetch_add(inc, std::memory_order_release);
    _trigger->start();
    _trigger->subscriber();
    num -= 1;
    _listener.store(num, std::memory_order_release);

} /* -----  end of function Container::wait_for  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::emit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Container::emit()
{
    _sequence.fetch_add(inc, std::memory_order_release);
    _trigger->turn(E2Q_RUNING);
    _trigger->emit();
} /* -----  end of function Container::emit  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::quit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Container::quit()
{
    SeqType n = 1;
    //  dump();
    while (n > 0) {
        _trigger->turn(E2Q_EXIST);
        _trigger->emit();
        // log::bug("sleep 1 ");

        sleep(1);

        n = _listener.load(std::memory_order_acquire);
    };

} /* -----  end of function Container::quit  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::runing
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool Container::runing()
{
    return _trigger->status() == E2Q_EXIST;
} /* -----  end of function Container::runing  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::rows
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int Container::rows(std::size_t id, std::size_t timeframe)
{
    int ret = -1;

    if (_cells.count(id) == 0) {
        return ret;
    }

    for (auto cell : _cells.at(id)) {
        if (cell.frame == timeframe) {
            ret = cell.data->rows();

            break;
        }
    }
    return ret;
} /* -----  end of function Container::rows  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::writed
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int Container::writed(std::size_t id, std::size_t timeframe)
{
    int ret = -1;

    if (_cells.count(id) == 0) {
        log::info("code not found:", id);
        return ret;
    }

    for (auto cell : _cells.at(id)) {
        if (cell.frame == timeframe) {
            ret = cell.data->writed();

            break;
        }
    }
    return ret;

} /* -----  end of function Container::writed  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::idx
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
std::size_t Container::idx(std::size_t stock, std::size_t timeframe)
{
    if (_cells.count(stock) == 0) {
        log::bug("but stock:", stock);
        for (auto it : _cells) {
            log::info("cells system cfi code:", it.first);
        }
        return 0;
    }

    for (auto cell : _cells.at(stock)) {
        if (cell.frame == timeframe) {
            return cell.idx;
        }
    }
    return 0;

} /* -----  end of function Container::idx  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::read
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  以后再优化
 * ============================================
 */
int Container::read(std::array<SeqType, ohlc_column>& ohlc, std::size_t stock,
                    std::size_t timeframe, std::size_t shift)
{
    int ret = -1;
    std::size_t m = 0;

    if (_cells.count(stock) == 0) {
        log::bug("but stock:", stock);
        return ret;
    }
    int rows = 0;
    for (auto cell : _cells.at(stock)) {
        if (cell.frame == timeframe) {
            rows = _cells.at(stock).at(m).data->writed();
            if (rows >= (int)shift) {
                ret = _cells.at(stock).at(m).data->read(&ohlc, shift);
            }

            break;
        }
        m++;
    }
    return ret;

} /* -----  end of function Container::read  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::tail
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
int Container::tail(std::array<SeqType, ohlc_column>& ohlc)
{
    int ret = -1;

    if (_cells.size() == 0) {
        return ret;
    }
    for (auto cell : _cells.at(0)) {
        ret = _cells.at(0).at(0).data->read(&ohlc);
        break;
    }
    return ret;
} /* -----  end of function Container::tail  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::dump
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Container::dump()
{
    size_t stock = 0;
    std::array<std::array<e2q::SeqType, ohlc_column>, 5> bar;

    for (auto it : _cells) {
        stock = it.first;
        log::echo("cfi:", stock);

        for (auto cs : it.second) {
            log::info("idx:", cs.idx, " frame:", cs.frame);
            cs.data->read(&bar);
            logs(bar);
        }
    }
} /* -----  end of function Container::dump  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Container::logs
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Container::logs(std::array<std::array<e2q::SeqType, ohlc_column>, 5> bar)
{
    UtilTime ut;
    const char* fmt = "%Y-%m-%d %H:%M";

    bprinter::TablePrinter tp(&std::cout);
    tp.AddColumn("time", 20);
    tp.AddColumn("open", 10);
    tp.AddColumn("high", 10);
    tp.AddColumn("low", 10);
    tp.AddColumn("close", 10);
    tp.AddColumn("volume", 10);

    tp.PrintHeader();

    for (auto bnum : bar) {
        tp << ut.millitostr(bnum[OHLC_T::ohlc_t], fmt) << bnum[OHLC_T::open_t]
           << bnum[OHLC_T::high_t] << bnum[OHLC_T::low_t]
           << bnum[OHLC_T::close_t] << bnum[OHLC_T::volume_t];
    }

    tp.PrintFooter();

} /* -----  end of function Container::logs  ----- */
}  // namespace e2q
