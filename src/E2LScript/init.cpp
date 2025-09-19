/*
 * =====================================================================================
 *
 *       Filename:  init.cpp
 *
 *    Description:  init
 *
 *        Version:  1.0
 *        Created:  2024年01月27日 16时02分23秒
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
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "E2LScript/e2lLead.hpp"
#include "E2LScript/foreign.hpp"
#include "Toolkit/GlobalConfig.hpp"
#include "assembler/BaseType.hpp"
namespace e2l {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  isInit
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  InitOk.I_Proc
 * ============================================
 */
e2::Int_e isInit()
{
    e2::Int_e ret = e2::InitOk::I_OK;
    if (e2q::FixPtr != nullptr) {
        if (e2q::FixPtr->_ok == e2::InitOk::I_Proc) {
            ret = e2::InitOk::I_Proc;
            if (e2q::FixPtr->_fix_symbols.size() > 0) {
                e2q::FixPtr->_ok = e2::InitOk::I_OK;
            }
        }
    }

    if (e2q::FinFabr != nullptr) {
        if (e2q::FinFabr->_ok == e2::InitOk::I_Proc) {
            ret = e2::InitOk::I_Proc;
            if (e2q::FinFabr->_fix_symbols.size() > 0) {
                e2q::FinFabr->_ok = e2::InitOk::I_OK;
            }
        }
    }

    return VALNUMBER(ret);
} /* -----  end of function isInit  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  fix
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void fix(const char *cfg)
{
    if (cfg == nullptr) {
        llog::bug("cfg is null");
        return;
    }
    int ret = access(cfg, R_OK);
    if (ret == -1) {
        printf("file: %s not exist \n", cfg);
        return;
    }
    if (e2q::FixPtr != nullptr) {
        e2q::FixPtr->_fix_cfg = std::string(cfg);
    }
    else if (e2q::FinFabr != nullptr) {
        e2q::FinFabr->_fix_cfg = std::string(cfg);
    }
    else {
        llog::bug("FixPtr or FinFabr is nullptr!");
    }

} /* -----  end of function fix  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  mkkf
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void mkkf(const char *source)
{
    if (source == nullptr) {
        llog::bug("source or topic not  nullptr");
        return;
    }

    if (e2q::FinFabr != nullptr) {
        e2q::FinFabr->_source = std::string(source);

        e2q::FinFabr->_csv_kafka = MKType::mk_kafka;
    }
    else if (e2q::FixPtr != nullptr) {
        e2q::FixPtr->_source = std::string(source);
    }

} /* -----  end of function mkkf  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  topic_tick
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void topic_tick(const char *topic)
{
    if (topic == nullptr || e2q::FinFabr == nullptr) {
        llog::bug("source or topic not  nullptr");
        return;
    }
    e2q::FinFabr->_topic = std::string(topic);
} /* -----  end of function topic_tick  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  topic_log
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void topic_log(const char *topic)
{
    if (topic == nullptr) {
        llog::bug("source or topic not  nullptr");
        return;
    }
#ifdef KAFKALOG

    std::string host = "";
    if (e2q::FinFabr != nullptr) {
        host = e2q::FinFabr->_source;
    }
    else if (e2q::FixPtr != nullptr) {
        host = e2q::FixPtr->_source;
    }
    if (host.length() == 0) {
        llog::bug("source or topic not  nullptr");
        return;
    }

    std::string str_topic = std::string(topic);
    auto log_fun = [](std::string host, std::string topic) {
        e2q::log.init(host, topic);
    };  // -----  end lambda  -----

    std::thread log_thread(log_fun, host, str_topic);
    log_thread.detach();
#endif

} /* -----  end of function topic_log  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  mkcsv
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void mkcsv(const char *source, const char *symbol)
{
    if (source == nullptr || symbol == nullptr) {
        llog::bug("path , symbol, code not  nullptr");
        return;
    }

    e2q::FinFabr->_source = std::string(source);
    e2q::FinFabr->_csv_kafka = MKType::mk_csv;

} /* -----  end of function mkcsv  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  mktype
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   csv 的时候，需要的, 到时候再调
 * ============================================
 */
void mktype(e2::Int_e tb)
{
    tb = NUMBERVAL(tb);
    e2q::FinFabr->_tick_bar = (e2::MKType)tb;
} /* -----  end of function mktype  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  tframe
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void tframe(e2::Int_e t)
{
    t = NUMBERVAL(t);
    e2::TimeFrames tf = e2q::EnumValid(t);

    if (std::find(std::begin(e2q::FixPtr->_tf), std::end(e2q::FixPtr->_tf),
                  tf) == std::end(e2q::FixPtr->_tf)) {
        e2q::FixPtr->_tf.push_back(tf);
    }

} /* -----  end of function tframe  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  deftframe
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e deftframe()
{
    return VALNUMBER(e2q::FixPtr->_current_tf);
} /* -----  end of function deftframe  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  index
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  报价是不是以  index 为准
 * ============================================
 */
e2::Int_e index()
{
    return e2q::FixPtr->_offers;
} /* -----  end of function index  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  offers
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  报价方式
 * ============================================
 */
void offers(e2::Int_e o)
{
    switch (o) {
        case e2::Offers::OF_Index:
            e2q::FixPtr->_offers = e2::Offers::OF_Index;
            break;
        case e2::Offers::OF_Tick:
            e2q::FixPtr->_offers = e2::Offers::OF_Tick;
            break;
        default:
            e2q::FixPtr->_offers = e2::Offers::OF_Index;
            break;
    }

} /* -----  end of function offers  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  OfferTime
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   min time >= 100ms
 * ============================================
 */
void OfferTime(e2::Int_e t)
{
    t = NUMBERVAL(t);

    if (t < 50) {
        t = 50;
    }

    e2q::FinFabr->_offer_time = t;

} /* -----  end of function OfferTime  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TradeTime
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void TradeTime(e2::Int_e open_hour, e2::Int_e open_min, e2::Int_e close_hour,
               e2::Int_e close_min)
{
    e2q::TradeTime tt;
    tt.open_hour = NUMBERVAL(open_hour);
    tt.open_min = NUMBERVAL(open_min);
    tt.close_hour = NUMBERVAL(close_hour);
    tt.close_min = NUMBERVAL(close_min);

    e2q::FinFabr->_tradetime.push_back(tt);

} /* -----  end of function TradeTime  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  gmtime
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void gmtime()
{
    e2q::FixPtr->_gmt = 1;
} /* -----  end of function gmtime  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  commission
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void commission(e2::Int_e cms, const char *ccy)
{
    cms = NUMBERVAL(cms);
    e2q::FinFabr->_commission = cms;
    e2q::FinFabr->_ccy = std::string(ccy);

} /* -----  end of function commission  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  QuantId
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  当前策略的 id
 * ============================================
 */
void QuantId(e2::Int_e id)
{
    std::thread::id _id = std::this_thread::get_id();
    if (e2q::FixPtr->_quantId.count(_id) == 0) {
        e2q::FixPtr->_quantId.insert({_id, {id, 0}});
    }
    else {
        e2q::FixPtr->_quantId[_id].first = id;
    }
} /* -----  end of function QuantId  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  GenerateQuantId
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void GenerateQuantId() {} /* -----  end of function GenerateQuantId  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  CurrentQuantId
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e CurrentQuantId()
{
    std::thread::id _id = std::this_thread::get_id();
    if (e2q::FixPtr->_quantId.count(_id) == 0) {
        return 0;
    }

    e2::Int_e ret = e2q::FixPtr->_quantId[_id].first;
    return ret;
} /* -----  end of function CurrentQuantId  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  TradeMode
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void TradeMode(e2::SymbolTradeMode m)
{
    e2::SymbolTradeMode tm = (e2::SymbolTradeMode)NUMBERVAL(m);
    switch (tm) {
        case e2::SymbolTradeMode::M_Disabled:
            e2q::FinFabr->_trade_mode = e2::SymbolTradeMode::M_Disabled;
            break;
        case e2::SymbolTradeMode::M_LongOnly:
            e2q::FinFabr->_trade_mode = e2::SymbolTradeMode::M_LongOnly;
            break;
        case e2::SymbolTradeMode::M_ShortOnly:
            e2q::FinFabr->_trade_mode = e2::SymbolTradeMode::M_ShortOnly;
            break;
        case e2::SymbolTradeMode::M_CloseOnly:
            e2q::FinFabr->_trade_mode = e2::SymbolTradeMode::M_CloseOnly;
            break;
        default:
            e2q::FinFabr->_trade_mode = e2::SymbolTradeMode::M_Full;
            break;
    }
} /* -----  end of function TradeMode  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  QuantVersion
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void QuantVersion(e2::Int_e major, e2::Int_e minor, e2::Int_e patch)
{
    char *field = nullptr;
    char *val = nullptr;
    int maj = (int)NUMBERVAL(abs(major));
    int min = (int)NUMBERVAL(abs(minor));
    int pat = (int)NUMBERVAL(abs(patch));
    std::string qversion = "";
    if (e2q::FinFabr->_QuantVerId > 0 || e2q::GlobalDBPtr == nullptr) {
        return;
    }
    char *v = nullptr;
    std::size_t len = snprintf(NULL, 0, "%d.%d.%d", maj, min, pat) + 1;
    v = MALLOC(char, len);
    snprintf(v, len, "%d.%d.%d", maj, min, pat);

    const char *fmt =
        "SELECT id  FROM trade_info WHERE version = '%s' LIMIT 1;";
    std::string sql = llog::format(fmt, v);

    std::size_t idx = e2q::GlobalDBPtr->getId();
    e2q::Pgsql *gsql = e2q::GlobalDBPtr->ptr(idx);
    if (gsql == nullptr) {
        e2q::GlobalDBPtr->release(idx);
        RELEASE(v);
        return;
    }
    gsql->update_table("trade_info");
    gsql->update_field("active", 0);
    UpdateCommit(gsql);

    bool r = SelectSQL(gsql, sql);
    if (r && gsql->tuple_size() > 0) {
        gsql->OneHead(&field, &val);
        if (val != nullptr) {
            e2q::FinFabr->_QuantVerId = stoi(val);
        }
    }
    else {
        e2q::UtilTime ut;
        gsql->insert_table("trade_info");
        gsql->insert_field("version", v);
        gsql->insert_field("desz", v);
        gsql->insert_field("ctime", ut.time());
        gsql->insert_return("id");
        r = InsertCommit(gsql);

        if (r) {
            gsql->OneHead(&field, &val);
            if (val != nullptr) {
                e2q::FinFabr->_QuantVerId = stoi(val);
            }
        }
    }

    gsql->update_table("trade_info");
    gsql->update_field("active", 1);
    gsql->update_condition("id", e2q::FinFabr->_QuantVerId);
    UpdateCommit(gsql);

    e2q::GlobalDBPtr->release(idx);

    RELEASE(v);

} /* -----  end of function QuantVersion  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  VersionId
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e VersionId()
{
    if (e2q::FinFabr != nullptr) {
        return VALNUMBER(e2q::FinFabr->_QuantVerId);
    }

    if (e2q::FixPtr != nullptr) {
        return VALNUMBER(e2q::FixPtr->_QuantVerId);
    }

    return 0;
} /* -----  end of function VersionId  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  LotAndShare
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void LotAndShare(e2::Int_e e)
{
    double lots = (double)NUMBERVAL(e);

    e2q::FinFabr->_lot_and_share = lots;
} /* -----  end of function LotAndShare  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  CurrentLS
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e CurrentLS()
{
    e2::Int_e ret = 0;
    if (e2q::FinFabr != nullptr) {
        ret = VALNUMBER(e2q::FinFabr->_lot_and_share);
    }

    if (e2q::FixPtr != nullptr) {
        ret = VALNUMBER(e2q::FixPtr->_lot_and_share);
    }
    return ret;
} /* -----  end of function CurrentLS  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  whois
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
e2::Int_e whois()
{
    e2::Int_e ret = VALNUMBER(e2::OMSRisk::I_BROKER);
    if (e2q::FixPtr != nullptr) {
        ret = VALNUMBER(e2::OMSRisk::I_EA);
    }
    else {
        std::thread::id _id = std::this_thread::get_id();
        if (_id == e2q::global_id_class[0]) {
            ret = VALNUMBER(e2::OMSRisk::I_OMS);
        }
    }

    return ret;
} /* -----  end of function whois  ----- */
}  // namespace e2l
