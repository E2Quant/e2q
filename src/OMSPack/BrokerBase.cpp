/*
 * =====================================================================================
 *
 *       Filename:  BrokerBase.cpp
 *
 *    Description:  BrokerBase
 *
 *        Version:  1.0
 *        Created:  2024年09月02日 15时35分23秒
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
#include "OMSPack/BrokerBase.hpp"

#include <string>

#include "E2LScript/ExternClazz.hpp"
#include "Toolkit/Norm.hpp"
#include "Toolkit/UtilTime.hpp"
#include "assembler/BaseType.hpp"
#include "utility/Log.hpp"
namespace e2q {

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BrokerBase::Debug
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BrokerBase::Debug()
{
    for (auto it : _traders) {
        std::string id = it.first.getTargetCompID().getValue();
        std::string cont = log::format("%.2f", it.second.total_cash);
        log::info("sid:", id, " total:", cont);

        for (auto ze : it.second.order_cash) {
            cont = log::format("report: %d, equity:%.2f, margin: %.2f",
                               ze.second.report, ze.second.equity,

                               ze.second.margin);
            log::info("ticket:", ze.first, " margin:", cont);
        }
    }
} /* -----  end of function BrokerBase::Debug  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BrokerBase::Equity
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  获取当前冻结的 equity
 * ============================================
 */
double BrokerBase::Equity(const FIX::SessionID& sid, std::size_t ticket,
                          const char status)
{
    double ret = 0;
    if (FinFabr->_sim == e2::SettleInstMode::_observer) {
        return 0;
    }
    if (_traders.count(sid) == 0) {
        return 0;
    }
    if (_traders.at(sid).order_cash.count(ticket) == 0) {
        return 0;
    }

    if (status == FIX::OrdStatus_NEW || status == FIX::OrdStatus_CANCELED) {
        ret = _traders.at(sid).order_cash.at(ticket).margin;
        if (ret == 0) {
            ret = _traders.at(sid).order_cash.at(ticket).equity;
        }
    }
    else {
        ret = _traders.at(sid).order_cash.at(ticket).equity;
    }
    return ret;
} /* -----  end of function BrokerBase::Equity  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BrokerBase::SettlInst
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  结算当前一笔 ticket
 *  如果是开仓成功，计算准确需要的费用
 *  如果是平仓成功，计算所得
 * ============================================
 */
void BrokerBase::SettlInst(OrderLots& lots)
{
    const FIX::SessionID& sid = lots.owner;
    std::size_t ticket = lots.ticket;
    double price = lots.lastExecutedPrice;
    long qty = lots.lastExecutedQuantity;
    bool isClose = lots.TradeTicket != 0;

    double margin = 0;

    if (FinFabr->_sim == e2::SettleInstMode::_observer) {
        return;
    }
    if (_traders.count(sid) == 0) {
        return;
    }

    double neet_equity = 0;
    char status =
        lots.isFilled ? FIX::OrdStatus_FILLED : FIX::OrdStatus_PARTIALLY_FILLED;

    BasicLock _lock(_BMute);

    if (isClose) {
        // 这边计算有点问题
        // 应该是 计算 收益的 百分比，而不是收盘的现价
        // 因为现价可能是 分红之后的价格
        // double order_price =
        //     _traders.at(sid).order_cash.at(lots.TradeTicket).adj_price;
        // // 实际收益
        // double earn = 1 + (lots.adjprice - order_price) / order_price;

        // neet_equity =
        //     _traders.at(sid).order_cash.at(lots.TradeTicket).equity * earn;

        neet_equity = lots.trade_amount;
        if (_traders.at(sid).order_cash.count(ticket) == 0) {
            TraderData_t dtt;
            dtt.adj_price = lots.adjprice;
            dtt.margin = 0;
            dtt.equity = neet_equity;
            dtt.qty = lots.quantity;
            dtt.report = false;
            _traders.at(sid).order_cash.insert({ticket, dtt});
        }
        else {
            _traders.at(sid).order_cash.at(ticket).equity = neet_equity;
        }
        if (status == FIX::OrdStatus_FILLED) {
            neet_equity = _traders.at(sid).order_cash.at(ticket).equity;
            _traders.at(sid).total_cash += neet_equity;

            // std::string balan = log::format(
            //     "total cash:%.3f, clostck:%ld, margin:%.3f",
            //     _traders.at(sid).total_cash, lots.TradeTicket, neet_equity);
            // log::info(balan);
        }
    }
    else {
        if (_traders.at(sid).order_cash.count(ticket) == 0) {
            return;
        }

        _traders.at(sid).order_cash.at(ticket).equity = lots.trade_amount;

        if (status == FIX::OrdStatus_FILLED) {
            _traders.at(sid).order_cash.at(ticket).adj_price = lots.adjprice;

            neet_equity = _traders.at(sid).order_cash.at(ticket).equity;
            margin = _traders.at(sid).order_cash.at(ticket).margin;

            if (FinFabr->_tif == e2::TimeInForce::tif_day) {
                if (neet_equity > margin) {
                    double _price = NUMBERVAL(price);
                    std::string cont = log::format(
                        " price: %.2f, margin: %.2f,neet equity:%.2f", _price,
                        margin, neet_equity);
                    log::bug(" settl error ticket:", ticket, cont,
                             " qty:", qty);
                    return;
                }
            }

            _traders.at(sid).order_cash.at(ticket).margin = neet_equity;

            _traders.at(sid).total_cash += (margin - neet_equity);
        }
    }

    if (lots.leavesQty == 0) {
        trade_report(lots);
    }
} /* -----  end of function BrokerBase::SettlInst  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BrokerBase::freeMargin
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  订单出问题释放冻结
 * ============================================
 */
void BrokerBase::freeMargin(const FIX::SessionID& sid, std::size_t ticket,
                            double margin)
{
    if (FinFabr->_sim == e2::SettleInstMode::_observer) {
        return;
    }
    if (_traders.count(sid) == 0 ||
        _traders.at(sid).order_cash.count(ticket) == 0) {
        return;
    }

    BasicLock _lock(_BMute);

    if (margin == 0) {
        double _margin = _traders.at(sid).order_cash.at(ticket).margin;
        _traders.at(sid).order_cash.at(ticket) = {0, 0};

        _traders.at(sid).total_cash += _margin;
        return;
    }

    if (_traders.at(sid).order_cash.at(ticket).margin >= margin) {
        _traders.at(sid).order_cash.at(ticket).margin -= margin;

        _traders.at(sid).total_cash += margin;
    }
    else {
        log::bug("free bug ticket:", ticket, " margin:", margin);
    }

} /* -----  end of function BrokerBase::freeMargin  ----- */
/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BrokerBase::Margin
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
bool BrokerBase::Margin(const FIX::SessionID& sid, std::size_t ticket,
                        double margin, long qty)
{
    bool ret = false;
    if (FinFabr->_sim == e2::SettleInstMode::_observer) {
        return ret;
    }
    if (_traders.count(sid) == 0 || margin <= 0) {
        return ret;
    }
    if (_traders.at(sid).total_cash < margin) {
        std::string fmt =
            log::format("total: %.2f, margin:%.2f,  ticket:%ld",
                        _traders.at(sid).total_cash, margin, ticket);
        log::bug(fmt);
        return ret;
    }

    BasicLock _lock(_BMute);

    if (_traders.at(sid).order_cash.count(ticket) == 1) {
        _traders.at(sid).order_cash.at(ticket).margin += margin;
    }
    else {
        TraderData_t dtt;
        dtt.adj_price = 0;
        dtt.margin = margin;
        dtt.equity = 0;
        dtt.qty = qty;
        dtt.report = false;
        _traders.at(sid).order_cash.insert({ticket, dtt});
    }

    _traders.at(sid).total_cash -= margin;

    return ret;
} /* -----  end of function BrokerBase::Margin  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BrokerBase::CheckMargin
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *  保证金计算 = Lots *  LastPrice * ( 1 + MarginRate )
 * ============================================
 */
double BrokerBase::CheckMargin(const FIX::SessionID& id, double price, long qty)
{
    /**
     * 如果不是观察模式就需要检查是否有足够的资产下单
     */
    if (FinFabr->_sim == e2::SettleInstMode::_observer) {
        return 0;
    }
    if (_traders.count(id) == 0) {
        log::bug("sid:", id.getTargetCompID().getValue());
        return 0;
    }

    double margin = FinFabr->equity(price, qty);

    if (_traders.at(id).total_cash < margin) {
        std::string erro =
            log::format("total_cash %.3f  margin: %.3f price:%.3f,  qty:%ld",
                        _traders.at(id).total_cash, margin, price, qty);
        log::bug(erro);
        margin = -1;
    }
    return margin;
} /* -----  end of function BrokerBase::CheckMargin  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BrokerBase::traders
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *   balance 的变动
 * ============================================
 */
void BrokerBase::traders(const FIX::SessionID& id, double cash)
{
    char* field = nullptr;
    char* val = nullptr;
    std::size_t idx = GlobalDBPtr->getId();
    Pgsql* gsql = GlobalDBPtr->ptr(idx);
    if (gsql == nullptr) {
        GlobalDBPtr->release(idx);
        log::bug("pgsql is null, idx:", idx);
        return;
    }
    int fix_id = 0;

    BasicLock _lock(_BMute);

    if (_traders.count(id) > 0) {
        _traders.at(id).total_cash += cash;
        fix_id = _traders.at(id).fix_id;

        gsql->update_table("account");
        std::string inc = " balance + " + std::to_string(cash);
        gsql->update_field("balance", inc);
        gsql->update_condition("id", fix_id);

        gsql->update_commit();
    }
    else {
        std::string compid = id.getTargetCompID().getValue();

        std::string sql = "SELECT id from fixsession WHERE targetcompid='" +
                          compid + "' LIMIT 1; ";

        bool r = gsql->select_sql(sql);

        if (r) {
            for (gsql->begin(); gsql->end(); gsql->next()) {
                int m = gsql->PGResult(&field, &val);
                if (m == -1) {
                    continue;
                }
                if (field != nullptr) {
                    fix_id = std::stoi(val);
                }
            }
        }
        if (fix_id == 0) {
            return;
        }
        TraderInfo ti;
        ti.fix_id = fix_id;
        ti.total_cash = cash;
        _traders.insert({id, ti});

        UtilTime ut;
        gsql->insert_table("account");
        gsql->insert_field("sessionid", fix_id);
        gsql->insert_field("balance", cash);
        gsql->insert_field("verid", FinFabr->_QuantVerId);
        gsql->insert_field("ctime", ut.time());
        gsql->insert_commit();
    }

    GlobalDBPtr->release(idx);

} /* -----  end of function BrokerBase::traders  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BrokerBase::trade_report
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BrokerBase::trade_report(OrderLots& lots)
{
    const FIX::SessionID& sid = lots.owner;
    std::size_t ticket = lots.ticket;

    double margin = 0;
    double balance = 0;
    double profit = 0;
    double credit = 0;
    bool isClose = lots.TradeTicket != 0;
    int side = e2::Side::os_Buy;

    // 记录单个 quantid 的 margin
    margin = _traders.at(sid).order_cash.at(ticket).equity;
    // log::info("margin:", _traders.at(sid).order_cash.at(ticket).margin);

    if (isClose) {
        side = e2::Side::os_Sell;
        margin = 0;

        _traders.at(sid).order_cash.at(lots.TradeTicket).report = true;
        profit = _traders.at(sid).order_cash.at(ticket).equity;
    }

    if (_traders.at(sid).order_cash.count(ticket) == 0) {
        return;
    }

    balance = _traders.at(sid).total_cash;
    const char fmt[] =
        "(SELECT id FROM fixsession WHERE targetcompid = '%s' LIMIT 1)";

    std::string sessionid =
        log::format(fmt, sid.getTargetCompID().getString().c_str());

    std::string ticket_to_id = log::format(
        "(select id from trades where ticket='%ld'  limit 1)", ticket);

    // 在数据库是记录当前所有 ticket 加起来的 balace
    for (auto it : _traders.at(sid).order_cash) {
        if (it.second.report || it.second.margin == 0) {
            continue;
        }
        credit += it.second.margin;
    }
    credit += balance;
    std::size_t idx = GlobalDBPtr->getId();
    Pgsql* pgsql = GlobalDBPtr->ptr(idx);

    if (pgsql == nullptr) {
        GlobalDBPtr->release(idx);
        log::bug("pg idx is nullptr, idx:", idx);
        return;
    }
    pgsql->insert_table("trade_report");
    pgsql->insert_field("ticket", ticket_to_id);
    pgsql->insert_field("sessionid", sessionid);
    pgsql->insert_field("balance", balance, 3);
    pgsql->insert_field("margin", margin, 3);
    pgsql->insert_field("profit", profit, 3);
    pgsql->insert_field("side", side);
    pgsql->insert_field("credit", credit, 3);
    pgsql->insert_field("ctime", lots.ctime);
    pgsql->insert_commit();

    pgsql->update_table("account");
    pgsql->update_field("balance", credit, 3);
    pgsql->update_field("margin", margin, 3);
    pgsql->update_field("profit", profit, 3);
    pgsql->update_condition("sessionid", sessionid);
    pgsql->update_commit();

    GlobalDBPtr->release(idx);

} /* -----  end of function BrokerBase::trade_report  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BrokerBase::AddExdrCash
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BrokerBase::AddExdrCash(SeqType ticket, double cash, std::size_t ctime)
{
    BasicLock _lock(_BMute);

    std::size_t idx = GlobalDBPtr->getId();
    Pgsql* pgsql = GlobalDBPtr->ptr(idx);
    double credit = 0;
    for (auto it : _traders) {
        if (it.second.order_cash.count(ticket) == 1) {
            double all_cash =
                FinFabr->exdr_cash(it.second.order_cash.at(ticket).qty, cash);

            _traders.at(it.first).total_cash += all_cash;

            if (pgsql != nullptr) {
                const char fmt[] =
                    "(SELECT id FROM fixsession WHERE targetcompid = '%s' "
                    "LIMIT 1)";

                std::string sessionid = log::format(
                    fmt, it.first.getTargetCompID().getString().c_str());

                std::string ticket_to_id = log::format(
                    "(select id from trades where ticket='%ld'  "
                    "limit 1)",
                    (std::size_t)ticket);
                credit = _traders.at(it.first).total_cash;

                for (auto itc : it.second.order_cash) {
                    if (itc.second.report || itc.second.margin == 0) {
                        continue;
                    }
                    credit += itc.second.margin;
                }
                pgsql->insert_table("trade_report");
                pgsql->insert_field("ticket", ticket_to_id);
                pgsql->insert_field("sessionid", sessionid);
                pgsql->insert_field("balance", _traders.at(it.first).total_cash,
                                    3);
                pgsql->insert_field("margin", 0);
                pgsql->insert_field("profit", all_cash, 3);
                pgsql->insert_field("side", 3);
                pgsql->insert_field("credit", credit, 3);
                pgsql->insert_field("ctime", ctime);
                pgsql->insert_commit();
            }

            break;
        }
    }
    GlobalDBPtr->release(idx);

} /* -----  end of function BrokerBase::AddExdrCash  ----- */

/*
 * ===  FUNCTION  =============================
 *
 *         Name:  BrokerBase::AddExdrQty
 *  ->  void *
 *  Parameters:
 *  - size_t  arg
 *  Description:
 *
 * ============================================
 */
void BrokerBase::AddExdrQty(SeqType cfi, SeqType ticket, double qty,
                            std::size_t ctime)
{
    std::size_t idx = GlobalDBPtr->getId();
    Pgsql* gsql = GlobalDBPtr->ptr(idx);
    if (gsql == nullptr) {
        GlobalDBPtr->release(idx);
        log::bug("error in ticket:", ticket, " qty:", qty);
        return;
    }

    BasicLock _lock(_BMute);

    if (FinFabr->_exdr_qty.count(ticket) == 0) {
        FinFabr->_exdr_qty.insert({ticket, qty});
    }
    else {
        FinFabr->_exdr_qty[ticket] += qty;
    }

    gsql->update_table("trades");

    std::string incqty = " openqty * (1.0 + " + std::to_string(qty) + ")";
    gsql->update_field("openqty", incqty);

    gsql->update_condition("ticket", ticket);
    gsql->update_condition("stat", 2);

    gsql->update_commit();

    GlobalDBPtr->release(idx);

} /* -----  end of function BrokerBase::AddExdrQty  ----- */
}  // namespace e2q
