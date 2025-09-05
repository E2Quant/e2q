/*
 * =====================================================================================
 *
 *       Filename:  analyzer.cpp
 *
 *    Description:  analyzer
 *
 *        Version:  1.0
 *        Created:  2024年03月05日 11时09分32秒
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
#include <cstddef>
#include <string>

#include "E2L/E2LType.hpp"
#include "E2LScript/e2lLead.hpp"
#include "E2LScript/foreign.hpp"
#include "E2LScript/util_inline.hpp"
#include "assembler/BaseType.hpp"
#include "libs/DB/pg.hpp"

namespace e2l {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  analysis
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e analysis(e2::Int_e qid, e2::Int_e analy)
{
    std::vector<e2q::AnalyDict> AnalyDictList;

    e2::Int_e b = 0;
    for (auto dict : AnalyDictList) {
        if (dict.id == qid) {
            e2::Analy ana = e2q::EnumAnaly(analy);
            switch (ana) {
                case e2::Analy::cash:
                    return dict.cash;
                case e2::Analy::start:
                    return dict.start;
                case e2::Analy::end:
                    return dict.end;
                case e2::Analy::Duration:
                    return dict.Duration;
                case e2::Analy::ExposureTime:
                    return dict.ExposureTime;
                case e2::Analy::EquityFinal:
                    return dict.EquityFinal;
                case e2::Analy::EquityPeak:
                    return dict.EquityPeak;
                case e2::Analy::Return:
                    return dict.Return;
                case e2::Analy::BuyAndHold:
                    return dict.BuyAndHold;
                case e2::Analy::ReturnAnn:
                    return dict.ReturnAnn;
                case e2::Analy::VolatilityAnn:
                    return dict.VolatilityAnn;
                case e2::Analy::SharpeRatio:
                    return dict.SharpeRatio;
                case e2::Analy::SortinoRatio:
                    return dict.SortinoRatio;
                case e2::Analy::CalmarRatio:
                    return dict.CalmarRatio;
                case e2::Analy::MaxDrawdown:
                    return dict.MaxDrawdown;
                case e2::Analy::AvgDrawdown:
                    return dict.AvgDrawdown;
                case e2::Analy::MaxDrawdownDuration:
                    return dict.MaxDrawdownDuration;
                case e2::Analy::WinRate:
                    return dict.WinRate;
                case e2::Analy::BestTrade:
                    return dict.BestTrade;
                case e2::Analy::WorstTrade:
                    return dict.WorstTrade;
                case e2::Analy::AvgTrade:
                    return dict.AvgTrade;
                case e2::Analy::MaxTradeDuration:
                    return dict.AvgTradeDuration;
                case e2::Analy::ProfitFactor:
                    return dict.ProfitFactor;
                case e2::Analy::Expectancy:
                    return dict.Expectancy;
                case e2::Analy::SQN:
                    return dict.SQN;
                case e2::Analy::KellyCriterion:
                    return dict.KellyCriterion;

                default:
                    return 0;
            }
        }
    }
    return b;
} /* -----  end of function analysis  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  analytotal
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e analytotal(e2::Int_e analy)
{
    e2::Int_e b = 0;
    return b;
} /* -----  end of function analytotal  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  Analse
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void Analse(e2::Int_e id, const char *name)
{
    id = NUMBERVAL(id);
    std::thread::id _id = std::this_thread::get_id();

    e2q::e2lAnalse ana;
    ana.id = id;
    if (e2q::FixPtr->_quantId.count(_id) == 0) {
        log::bug("quantId");
        ana.quantId = 0;
    }
    else {
        ana.quantId = e2q::FixPtr->_quantId[_id].first;
    }
    ana.name = std::string(name);
    ana.etime = 0;
    ana.init = e2q::ticket_now;
    e2q::e2_analse.update(_id, ana);

} /* -----  end of function Analse  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  AnalseArgv
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void AnalseArgv(e2::Int_e id, e2::Int_e args)
{
    id = NUMBERVAL(id);
    int iargs = (int)NUMBERVAL(args);
    std::string vargs = std::to_string(iargs);

    std::thread::id _id = std::this_thread::get_id();
    e2q::e2_analse.addArgv(_id, id, vargs);

} /* -----  end of function AnalseArgv  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  AnalseValue
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void AnalseValue(e2::Int_e id, e2::Int_e val)
{
    id = NUMBERVAL(id);
    double dval = NUMBERVAL(val);
    std::string vals = std::to_string(dval);

    std::thread::id _id = std::this_thread::get_id();
    e2q::e2_analse.addValue(_id, id, vals);

} /* -----  end of function AnalseValue  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  AnalseDB
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void AnalseDB()
{
    std::thread::id _id = std::this_thread::get_id();
    e2q::e2_analse.Save(_id);

} /* -----  end of function AnalseDB  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  AnalseLog
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void AnalseLog(e2::Int_e key, e2::Int_e val, e2::Int_e type, e2::Int_e time)
{
    std::thread::id _id = std::this_thread::get_id();
    std::size_t quantid = 0;
    if (e2q::FixPtr->_quantId.count(_id) == 1) {
        quantid = e2q::FixPtr->_quantId[_id].first;
    }

    double _val = (double)NUMBERVAL(val);
    int _type = (int)NUMBERVAL(type);
    int _key = (int)NUMBERVAL(key);

    std::string table = "public.";
    std::size_t gidx = e2q::GlobalDBPtr->getId();
    e2q::Pgsql *gsql = e2q::GlobalDBPtr->ptr(gidx);
    if (gsql != nullptr) {
        gsql->public_table(table);
        gsql->insert_table("analselog");
        gsql->insert_field("quantid", quantid);
        gsql->insert_field("key", _key);

        gsql->insert_field("values", _val, 5);
        gsql->insert_field("type", _type);

        gsql->insert_field("ctime", time);
        InsertCommit(gsql);
    }

    e2q::GlobalDBPtr->release(gidx);

} /* -----  end of function AnalseLog  ----- */
}  // namespace e2l
