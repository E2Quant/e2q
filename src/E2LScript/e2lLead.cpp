/*
 * =====================================================================================
 *
 *       Filename:  e2lLead.cpp
 *
 *    Description:  e2lLead
 *
 *        Version:  1.0
 *        Created:  2023年12月21日 17时49分24秒
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

#include "E2LScript/e2lLead.hpp"

#include "E2L/account.hpp"
#include "E2L/bot.hpp"
#include "E2L/broker.hpp"
#include "E2L/date_time.hpp"
#include "E2L/init.hpp"
#include "E2L/marketinfo.hpp"
#include "E2L/system.hpp"
#include "E2L/trade.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "Toolkit/GlobalConfig.hpp"

namespace e2q {

/**
 * e2l function
 */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OMSE2LFun
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  oms or risk
 * ============================================
 */
void OMSE2LFun()
{
    E2LFunList();

} /* -----  end of function InitE2LFun  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  EAE2LFun
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 * ea
 * ============================================
 */
void EAE2LFun() { E2LFunList(); } /* -----  end of function EAE2LFun  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  E2LFunList
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void E2LFunList()
{
    AddPlugFun();

    /**
     * account start
     */

    AddFun(AccountBalance, 0, "FAccountBalance", E2L_RETURN, "();");
    AddFun(AccountMargin, 0, "FAccountMargin", E2L_RETURN, "();");
    AddFun(AccountProfit, 0, "FAccountProfit", E2L_RETURN, "();");
    AddFun(AccountEquity, 0, "FAccountEquity", E2L_RETURN, "();");
    AddFun(ThreadPosition, 2, "FThreadPosition", E2L_NORETURN,
           "(thread_number, postion);");
    /**
     * account end
     */

    /**
     * init start
     */
    AddFun(isInit, 0, "FIsInit", E2L_RETURN, "();");

    AddFun(fix, 1, "FFix", E2L_NORETURN, "(path);");

    AddFun(mktype, 1, "FMkType", E2L_NORETURN, "(tick_bar);");

    AddFun(mkcsv, 2, "FMkCsv", E2L_NORETURN, "(source, symbol);");

    AddFun(mkkf, 1, "FMkkf", E2L_NORETURN, "(source);");
    AddFun(topic_tick, 1, "FTopicTick", E2L_NORETURN, "(topic);");
    AddFun(topic_log, 1, "FTopicLog", E2L_NORETURN, "(log);");

    AddFun(tframe, 1, "FTFrame", E2L_NORETURN, "(frame);");
    AddFun(deftframe, 0, "FDefTFrame", E2L_RETURN, "();");

    AddFun(index, 0, "FIndex", E2L_RETURN, "();");
    AddFun(offers, 1, "FOffers", E2L_NORETURN, "(istick);");
    AddFun(OfferTime, 1, "FOfferTime", E2L_NORETURN, "(time);");
    AddFun(commission, 2, "FCommission", E2L_NORETURN, "(cms, ccy);");

    AddFun(QuantId, 1, "FQuantId", E2L_NORETURN, "(qid);");
    AddFun(GenerateQuantId, 0, "FGenerateQuantId", E2L_NORETURN, "();");
    AddFun(CurrentQuantId, 0, "FCurrentQuantId", E2L_RETURN, "();");

    AddFun(QuantVersion, 3, "FQuantVersion", E2L_NORETURN,
           "(major,minor,patch);");

    AddFun(TradeTime, 4, "FTradeTime", E2L_NORETURN,
           "(open_hour,open_minute,close_hour,close_minute);");
    AddFun(TradeMode, 1, "FTradeMode", E2L_NORETURN,
           "(USymbolTradeMode::M_CloseOnly);");

    AddFun(gmtime, 0, "FGmt", E2L_NORETURN, "();");

    AddFun(LotAndShare, 1, "FLotAndShare", E2L_NORETURN, "(lot);");
    AddFun(CurrentLS, 0, "FCurrentLS", E2L_RETURN, "();");
    /**
     * init end
     */

    /**
     * trade start
     */
    AddFun(OrderTicket, 0, "FOrderTicket", E2L_RETURN, "();");
    AddFun(OrderSelect, 3, "FOrderSelect", E2L_RETURN, "(index,select,pool);");
    AddFun(OrderComment, 3, "FOrderComment", E2L_NORETURN,
           "(ticket,side, cmt);");

    AddFun(OrderClose, 4, "FOrderClose", E2L_RETURN,
           "(ticket,lost,stoppx,slippage);");

    AddFun(OrderSend, 6, "FOrderSend", E2L_RETURN,
           "(symbol,side,qty,price,slippage,ordtype);");
    AddFun(OrdersTotal, 0, "FOrdersTotal", E2L_RETURN, "();");
    AddFun(OrdersHistoryTotal, 0, "FOrdersHistoryTotal", E2L_RETURN, "();");

    AddFun(OrderLots, 1, "FOrderLots", E2L_RETURN, "(ticket)");
    AddFun(OrderOpenPrice, 2, "FOrderOpenPrice", E2L_RETURN, "(ticket, bool);");

    /**
     * trade end
     */

    /**
     * analyzer start
     */
    AddFun(analysis, 2, "FAnalysis", E2L_RETURN, "(qid,analy );");

    AddFun(analytotal, 1, "FAnalyTotal", E2L_RETURN, "(analy);");

    AddFun(Analse, 2, "FAnalse", E2L_NORETURN, "(id, name);");
    AddFun(AnalseArgv, 2, "FAnalseArgv", E2L_NORETURN, "(id, args);");
    AddFun(AnalseValue, 2, "FAnalseValue", E2L_NORETURN, "(id,value);");
    AddFun(AnalseDB, 0, "FAnalseDB", E2L_NORETURN, "();");
    AddFun(AnalseLog, 4, "FAnalseLog", E2L_NORETURN, "(key, value, idx,time);");
    /**
     * analyzer end
     */

    /**
     * broker start
     */
    AddFun(setCash, 1, "FSetCash", E2L_NORETURN, "(cash);");
    AddFun(getCash, 0, "FGetCash", E2L_RETURN, "();");
    AddFun(SettlInst, 1, "FSettlInst", E2L_NORETURN, "(settlemode);");
    AddFun(TIForce, 1, "FTimeInForce", E2L_NORETURN, "(timeinforce);");
    AddFun(MarginRate, 1, "FMarginRate", E2L_NORETURN, "(rate);");
    AddFun(MatchEventInit, 1, "FMatchEventInit", E2L_NORETURN, "(match);");
    AddFun(EnableExdividend, 1, "FExdividendEnable", E2L_NORETURN, "(bool);");
    AddFun(ExDivPrice, 1, "FExDivPrice", E2L_RETURN, "(cfi_code);");
    AddFun(ExDividendSize, 1, "FExDividendSize", E2L_RETURN, "(cfi_code);");
    AddFun(ExDividendDate, 2, "FExDividendDate", E2L_RETURN, "(cfi_code,idx);");
    AddFun(ExDividendCash, 2, "FExDividendCash", E2L_RETURN, "(cfi_code,idx);");
    AddFun(ExDividendShare, 2, "FExDividendShare", E2L_RETURN,
           "(cfi_code,idx);");
    AddFun(BrokerBook, 1, "FBrokerBook", E2L_NORETURN, "(booktype);");

    /**
     * broker end
     */

    /**
     * general start
     */
    AddFun(log, 4, "log", E2L_NORETURN, "(value);");
    AddFun(PrintLine, 4, "FPrintLine", E2L_NORETURN, "(value);");

    AddFun(PrintDeci, 5, "FPrintDeci", E2L_NORETURN, "(value,deci);");
    AddFun(PrintTime, 4, "FPrintTime", E2L_NORETURN, "(value);");
    AddFun(StoreId, 2, "FStoreId", E2L_RETURN, "();");
    AddFun(fetch, 1, "FFetch", E2L_RETURN, "(id);");
    AddFun(store, 2, "FStore", E2L_NORETURN, "(id, value);");

    AddFun(TicketSize, 0, "FTicketSize", E2L_RETURN, "();");
    /**
     * general end
     */
    /**
     * math start
     */
    AddFun(Last, 1, "FLast", E2L_RETURN, "(id);");

    AddFun(Array, 5, "FArray", E2L_NORETURN, "(id, size);");
    AddFun(ArrayFixed, 4, "FArrayFixed", E2L_NORETURN, "(id);");
    AddFun(ArrayShare, 4, "FArrayShare", E2L_NORETURN, "(id);");
    AddFun(ArrayAdd, 5, "FArrayAdd", E2L_RETURN, "(id, value);");
    AddFun(ArrayUpdate, 6, "FArrayUpdate", E2L_RETURN, "(id, index, value);");
    AddFun(ArrayGet, 5, "FArrayGet", E2L_RETURN, "(id,index);");
    AddFun(ArrayFill, 5, "FArrayFill", E2L_NORETURN, "(id, value);");
    AddFun(ArrayLast, 4, "FArrayLast", E2L_RETURN, "(id);");
    AddFun(ArrayLength, 4, "FArrayLength", E2L_RETURN, "(id);");
    AddFun(ArraySize, 4, "FArraySize", E2L_RETURN, "(id);");
    AddFun(ArrayMax, 4, "FArrayMax", E2L_RETURN, "(id);");
    AddFun(ArrayMin, 4, "FArrayMin", E2L_RETURN, "(id);");
    AddFun(Sum, 2, "FArraySum", E2L_RETURN, "(id,idx);");

    AddFun(Max, 2, "FMax", E2L_RETURN, "(val1, val2);");
    AddFun(Min, 2, "FMin", E2L_RETURN, "(val1, val2);");
    AddFun(Maxs, 3, "FMaxs", E2L_RETURN, "(val1, val2, val3);");
    AddFun(Mins, 3, "FMins", E2L_RETURN, "(val1, val2, val3);");

    AddFun(between, 3, "FBetween", E2L_RETURN, "(val, start, end);");

    AddFun(Sqrt, 1, "FSqrt", E2L_RETURN, "(value);");
    AddFun(Stdev, 1, "FStdev", E2L_RETURN, "(id);");

    AddFun(Abs, 1, "FAbs", E2L_RETURN, "(value);");
    AddFun(Ceil, 1, "FCeil", E2L_RETURN, "(value);");
    AddFun(Floor, 1, "FFloor", E2L_RETURN, "(value);");

    /**
     * math end
     */

    /**
     * market info
     */

    AddFun(SymbolCFICode, 1, "FCFICode", E2L_RETURN, "(idx);");

    AddFun(SymbolsTotal, 0, "FSymbols", E2L_RETURN, "();");

    AddFun(SymbolSelect, 1, "FSymbol", E2L_NORETURN, "(symbol);");
    AddFun(Settlement, 1, "FSettlement", E2L_NORETURN, "(value);");

    AddFun(iOpen, 3, "FOpen", E2L_RETURN, "(cfi_id, timeframe, shift);");
    AddFun(iHigh, 3, "FHigh", E2L_RETURN, "(cfi_id, timeframe, shift);");
    AddFun(iLow, 3, "FLow", E2L_RETURN, "(cfi_id, timeframe, shift);");
    AddFun(iClose, 3, "FClose", E2L_RETURN, "(cfi_id, timeframe, shift);");
    AddFun(iVolume, 3, "FVolume", E2L_RETURN, "(cfi_id, timeframe, shift);");
    AddFun(iTime, 3, "FTime", E2L_RETURN, "(cfi_id, timeframe, shift);");

    AddFun(iAdjClose, 3, "FAdjClose", E2L_RETURN,
           "(cfi_id, timeframe, shift);");

    AddFun(BarVolumeAppend, 0, "FVolumeAppend", E2L_NORETURN, "();");
    AddFun(BarOnOpen, 0, "FOnOpen", E2L_NORETURN, "();");
    AddFun(BarSize, 2, "FBarSize", E2L_RETURN, "(cfi_id, timeframe);");
    AddFun(Bar, 3, "FBar", E2L_RETURN, "(cfi_id, timeframe, shift);");
    AddFun(BarSeries, 1, "FBarSeries", E2L_RETURN, "(bartype);");

    AddFun(isExdiDate, 1, "FExdiDate", E2L_RETURN, "(cfi_id);");
    AddFun(ExdiCash, 1, "FExdiCash", E2L_RETURN, "(cfi_id);");
    AddFun(ExdiShare, 1, "FExdiShare", E2L_RETURN, "(cfi_id);");

    AddFun(CustomDataSize, 2, "FCustomDataSize", E2L_RETURN, "(cfi_id, idx);");
    AddFun(CustomDataNumber, 2, "FCustomDataNumber", E2L_RETURN,
           "(cfi_id, idx);");
    AddFun(CustomDataGet, 3, "FCustomDataGet", E2L_RETURN,
           "(cfi_id, idx, pos);");
    /**
     * market info end
     */

    /**
     * bot
     */

    AddFun(Giveaway, 0, "FGiveaway", E2L_NORETURN, "();");

    /**
     * bot end
     */

    /**
     * date time
     */

    AddFun(TimeCurrent, 0, "FTimeCurrent", E2L_RETURN, "();");
    AddFun(TimeLocal, 0, "FTimeLocal", E2L_RETURN, "();");
    AddFun(Clock, 0, "FClock", E2L_RETURN, "();");
    AddFun(year, 1, "FYear", E2L_RETURN, "(now);");
    AddFun(month, 1, "FMonth", E2L_RETURN, "(now);");
    AddFun(day, 1, "FDay", E2L_RETURN, "(now);");
    AddFun(week, 1, "FWeek", E2L_RETURN, "(now);");
    AddFun(year_week, 1, "FYearWeek", E2L_RETURN, "(now);");
    AddFun(hours, 1, "FHour", E2L_RETURN, "(now);");
    AddFun(minutes, 1, "FMinute", E2L_RETURN, "(now);");
    AddFun(second, 1, "FSecond", E2L_RETURN, "(now);");
    AddFun(millisecond, 1, "Fmillisecond", E2L_RETURN, "(now);");
    /**
     * date time end
     */

    /**
     * system
     */
    AddFun(ethread, 1, "FThread", E2L_NORETURN, "( number);");
    AddFun(process_id, 0, "FProcessId", E2L_RETURN, "();");
    AddFun(process_runs, 0, "FProcessRuns", E2L_RETURN, "();");
    AddFun(ethread_id, 0, "FThreadId", E2L_NORETURN, "();");
    AddFun(TypeOf, 1, "FTypeOf", E2L_RETURN, "(fun);");

    AddFun(ApiList, 0, "FApiList", E2L_NORETURN, "();");
    AddFun(SymbolUnion, 0, "FSymbolUnion", E2L_NORETURN, "();");
} /* -----  end of function E2LFunList  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  EnumValid
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::TimeFrames EnumValid(e2::Int_e v)
{
    e2::TimeFrames __tf = e2::TimeFrames::PERIOD_M1;
    switch (v) {
        case e2::TimeFrames::PERIOD_CURRENT:
            __tf = e2::TimeFrames::PERIOD_CURRENT;
            break;
        case e2::TimeFrames::PERIOD_M1:
            __tf = e2::TimeFrames::PERIOD_M1;
            break;
        case e2::TimeFrames::PERIOD_M:
            __tf = e2::TimeFrames::PERIOD_M;
            break;
        case e2::TimeFrames::PERIOD_M15:
            __tf = e2::TimeFrames::PERIOD_M15;
            break;
        case e2::TimeFrames::PERIOD_M30:
            __tf = e2::TimeFrames::PERIOD_M30;
            break;
        case e2::TimeFrames::PERIOD_H1:
            __tf = e2::TimeFrames::PERIOD_H1;
            break;
        case e2::TimeFrames::PERIOD_H4:
            __tf = e2::TimeFrames::PERIOD_H4;
            break;
        case e2::TimeFrames::PERIOD_D1:
            __tf = e2::TimeFrames::PERIOD_D1;
            break;
        case e2::TimeFrames::PERIOD_W1:
            __tf = e2::TimeFrames::PERIOD_W1;
            break;
        case e2::TimeFrames::PERIOD_MN1:
            __tf = e2::TimeFrames::PERIOD_MN1;
            break;
        default:
            __tf = e2::TimeFrames::PERIOD_CURRENT;
            break;
    }
    return __tf;
} /* -----  end of function EnumValid  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  EnumAnaly
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  constexpr inline auto enum_range = [](auto front, auto back) {
 return std::views::iota(std::to_underlying(front), std::to_underlying(back) +
 1) | std::views::transform([](auto e) { return decltype(front)(e); });
 };

https://stackoverflow.com/questions/69762598/what-are-commonly-used-ways-to-iterate-over-an-enum-class-in-c
 * ============================================
 */
e2::Analy EnumAnaly(e2::Int_e v)
{
    e2::Analy ana = e2::Analy::id;
    for (e2::Analy a : e2::AnalyList) {
        if ((e2::Int_e)a == v) {
            return a;
        }
    }

    return ana;
} /* -----  end of function EnumAnaly  ----- */

}  // namespace e2q
