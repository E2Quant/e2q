/*
 * =====================================================================================
 *
 *       Filename:  Order.hpp
 *
 *    Description:  Order
 *
 *        Version:  1.0
 *        Created:  2024年01月10日 14时19分13秒
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

#ifndef ORDER_INC
#define ORDER_INC
#include <cstddef>
#include <cstdint>
#include <memory>
#include <ostream>
#include <string>

#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "OMSPack/foreign.hpp"
#include "assembler/BaseType.hpp"
#include "utility/Log.hpp"

#if __cplusplus >= 201703L

/* Now remove the trow */
#define throw(...)
#include <quickfix/SessionID.h>
#undef throw /* reset */
#endif
namespace e2q {

typedef struct LeItem PendType;
/*
 * ================================
 *        Class:  OrderPending
 *  Description:
 * ================================
 */
class OrderPending : public PendType {
public:
    /* =============  LIFECYCLE     =================== */
    OrderPending() {};
    OrderPending(std::size_t ticket, double price, long quantity,
                 e2::OrdType type)
        : _ticket(ticket), _price(price), _quantity(quantity), _type(type)

    {
        // 平仓的时候才会减少的
        _openQuantity = _quantity;
        // 当前的状态还有多少 qty
        _leavesQty = _quantity;
        _executedQuantity = 0;
        _avgExecutedPrice = 0;
        _lastExecutedPrice = price;
        _lastExecutedQuantity = 0;
        _trade_amount = 0;
    } /* constructor */

    /* =============  ACCESSORS     =================== */
    bool volatility() { return isFilled(); }
    /* =============  MUTATORS      =================== */
    std::size_t getTicket() const { return _ticket; }
    double getLastExecutedPrice() const { return _lastExecutedPrice; }
    long getLastExecutedQuantity() const { return _lastExecutedQuantity; }
    long getOpenQuantity() const { return _openQuantity; }
    long getExecutedQuantity() const { return _executedQuantity; }
    double getAvgExecutedPrice() const { return _avgExecutedPrice; }
    long getLeavesQty() const { return _leavesQty; }
    e2::OrdType getType() const { return _type; }

    bool isFilled() const
    {
        /* log::info("ticket:", _ticket, " quantity:", _quantity, */
        /*           " execqty:", _executedQuantity, " openqty:",
         * _openQuantity); */
        return _quantity == _executedQuantity || _openQuantity == 0;
    }
    bool isClosed() const { return _openQuantity == 0; }
    void Closeed()
    {
        _leavesQty = 0;
        _quantity = _openQuantity = _executedQuantity;
    }
    double getPrice() const { return _price; }
    long getQuantity() const { return _quantity; }
    void Bot() { _bot = true; }
    bool isBot() const { return _bot; }

    void Adj(double apx) { _adjprice = apx; }
    double Adj() { return _adjprice; }
    double amount() { return _trade_amount; }
    void Margin(double margin) { _margin = margin; }
    /**
     * reject 的时候使用，delete order
     */
    void disable()
    {
        log::bug("disable ticket:", _ticket);
        _quantity = _executedQuantity = _openQuantity = 0;
    }

    /**
     * 计算有多少 qty 可以撮合，在 checkorder fix application 这儿用的
     */
    long CloseOrder(long lots)
    {
        _isCloseOrder = true;
        // 分红加上来的在 match bot 的时候，也得加一下
        // long all_qty = _quantity;
        if (FinFabr->_exdr_qty.count(_ticket) == 1 &&
            FinFabr->_exdr_qty.at(_ticket) > 0) {
            _executedQuantity = _quantity =
                _quantity * (1.0 + FinFabr->_exdr_qty.at(_ticket));

            FinFabr->_exdr_qty[_ticket] = 0;
        }
        if (lots > _quantity) {
            return _quantity;
        }

        return lots;
    }

    /**
     * 撮合成功之后减去 lots 的 qty
     */
    void Lots(long lots)
    {
        if (lots > _openQuantity || lots == 0) {
            log::bug("error, lots:", lots, " in ticket:", _ticket);
            _openQuantity = 0;
            return;
        }
        _openQuantity -= lots;
    }
    bool isCloseOrder() { return _isCloseOrder; }

    long execute(double price, long quantity)
    {
        /**
         * ioc
         */
        long effect = 0;

        if (_long_short == 0 && _bot == false) {
            // 如果 是平仓就不进这儿
            effect = FinFabr->effectQty(price, _margin);

            if (effect < quantity) {
                quantity = effect;
            }
        }

        if (quantity > _leavesQty) {
            quantity = _leavesQty;
        }

        double true_price = price;
#ifdef NUMBER_DECI
        true_price = NUMBERVAL(price);
#endif

        // 当前一笔 成交金额，为此订单的成交总金额
        double trade_amount = FinFabr->effect(true_price, (double)quantity);

        double exec_amount =
            FinFabr->effect(_avgExecutedPrice, _executedQuantity);

        _avgExecutedPrice = ((trade_amount + exec_amount) /
                             (double)(quantity + _executedQuantity)) /
                            FinFabr->_lot_and_share;

        _leavesQty -= quantity;
        _executedQuantity += quantity;
        _lastExecutedPrice = price;
        _lastExecutedQuantity = quantity;

        _trade_amount += trade_amount;

        // 开仓的时候计算
        if (_bot == false && _long_short == 0) {
            // log::echo(" margin :", _margin, " ticket:", _ticket);
            _margin -= trade_amount;
            if (_leavesQty > 0 && _margin <= 0) {
                Closeed();
            }
        }

        return quantity;
    }
    void shorted() { _long_short = 1; }
    void cancel() { _lastExecutedQuantity = 0; }
    /* =============  OPERATORS     =================== */
    bool operator<(const OrderPending& rhs) const
    {
        return _price < rhs.getPrice() ? true : false;
    }
    bool operator>(const OrderPending& rhs) const
    {
        return _price > rhs.getPrice() ? true : false;
    }

    bool operator==(const OrderPending& rhs) const
    {
        return _price == rhs.getPrice() ? true : false;
    }

    /* OrderPending& operator+=(const OrderPending& rhs) */
    /* { */
    /*     _ticket = rhs.getTicket(); */
    /*     _price = rhs.getPrice(); */
    /*     _quantity = rhs.getQuantity(); */

    /*     return *this; */
    /* }; */
    /* OrderPending& operator-=(OrderPending& closeOrder) */
    /* { */
    /*     //        OrderPending leave; */
    /*     long quantity = closeOrder.getQuantity(); */

    /*     execute(_price, quantity); */
    /*     closeOrder.execute(_price, _openQuantity); */

    /*     return *this; */
    /* }; */

    e2::Int_e value() const { return _price; }
    bool check() const { return _bot; }
    friend std::ostream& operator<<(std::ostream& ost, const OrderPending& m)
    {
        ost << " price:" << m.getPrice() << " ticket:" << m.getTicket()
            << " qty:" << m.getLeavesQty() << " fille:"
            << " bot:" << m.isBot();

        return ost;
    }
    OrderPending* next = nullptr;

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    std::size_t _ticket = 0;
    double _price = 0;
    double _adjprice = 0;
    long _quantity = 0;

    long _openQuantity = 0;
    long _leavesQty = 0;
    long _executedQuantity = 0;
    double _avgExecutedPrice = 0;
    double _lastExecutedPrice = 0;
    long _lastExecutedQuantity = 0;
    bool _bot = false;

    bool _isCloseOrder = false;
    // 0 = long 1 = short
    int8_t _long_short = 0;
    e2::OrdType _type;
    // 当前一笔交易的金额
    double _margin = 0;
    // 累计成交金额
    double _trade_amount;

}; /* -----  end of class OrderPending  ----- */

struct OrderLots {
    std::uint64_t ticket;
    std::string clOrdId;
    std::string symbol;
    FIX::SessionID owner;
    e2::Side side;
    e2::OrdType type;

    long quantity;

    double price;
    double adjprice;

    long leavesQty;
    long openQuantity;
    long executedQuantity;
    double avgExecutedPrice;
    long lastExecutedQuantity;
    double lastExecutedPrice;
    double trade_amount;

    bool isFilled;
    bool isCancel;
    e2::Int_e quantId;
    std::size_t ctime;
    std::size_t otime;

    e2::Int_e TradeTicket = 0;

}; /* ----------  end of struct OrderLots  ---------- */

/*
 * ================================
 *        Class:  OrderItem
 *  Description:
 * ================================
 */
class OrderItem {
public:
    /* =============  LIFECYCLE     =================== */

    OrderItem(std::size_t ticket, const std::string& clOrdId,
              const std::string& symbol, const FIX::SessionID& owner,
              e2::Side side, e2::OrdType type, double price, long quantity,
              e2::Int_e qid, std::size_t ctime, std::size_t otime)
        : _clOrdId(clOrdId),
          _symbol(symbol),
          _owner(owner),
          _side(side),
          _type(type),
          _quantId(qid),
          _time(ctime),
          _otime(otime)
    {
        _pending = MALLOC(OrderPending, ticket, price, quantity, type);
    }
    ~OrderItem()
    {
        // log::echo("release pending");
        RELEASE(_pending);
    }
    /* constructor */

    /* =============  ACCESSORS     =================== */

    /* =============  MUTATORS      =================== */

    const std::string& getClOrdID() const { return _clOrdId; }
    const std::string& getSymbol() const { return _symbol; }
    const FIX::SessionID& getOwner() const { return _owner; }
    e2::Side getSide() const { return _side; }
    e2::OrdType getType() const { return _type; }
    void TradeTicket(e2::Int_e t)
    {
        _TradeTicket = t;
        _pending->shorted();
    }
    e2::Int_e getCloseTicket() { return _TradeTicket; }
    e2::Int_e getTime() { return _time; }
    e2::Int_e getOtime() { return _otime; }
    void hasMargin(double margin) { _pending->Margin(margin); }

    OrderLots Lots()
    {
        OrderLots lots;
        lots.symbol = _symbol;
        lots.ticket = _pending->getTicket();
        lots.openQuantity = _pending->getOpenQuantity();
        lots.executedQuantity = _pending->getExecutedQuantity();
        lots.avgExecutedPrice = _pending->getAvgExecutedPrice();
        lots.lastExecutedQuantity = _pending->getLastExecutedQuantity();
        lots.lastExecutedPrice = NUMBERVAL(_pending->getLastExecutedPrice());
        lots.isFilled = _pending->isFilled();
        lots.quantity = _pending->getQuantity();
        lots.leavesQty = _pending->getLeavesQty();
        lots.quantId = _quantId;
        lots.price = NUMBERVAL(_pending->getPrice());
        lots.ctime = _time;
        lots.otime = _otime;

        lots.adjprice = NUMBERVAL(_pending->Adj());
        // log::info("adj:", lots.adjprice);
        lots.trade_amount = _pending->amount();
        lots.isCancel = false;
        if (_bot) {
            lots.clOrdId = "";
        }
        else {
            lots.clOrdId = getClOrdID();
        }
        lots.symbol = getSymbol();
        lots.owner = getOwner();
        lots.side = getSide();
        lots.type = getType();
        lots.TradeTicket = _TradeTicket;

        return lots;
    }
    void Bot()
    {
        _bot = true;
        _pending->Bot();
    }
    bool isBot() { return _bot; }
    OrderPending* Pending() { return _pending; }
    /* =============  OPERATORS     =================== */
    friend std::ostream& operator<<(std::ostream& ost, const OrderItem& m)
    {
        ost << "dd";
        /* OrderPending* p = m.next; */
        /* if (p != nullptr) { */
        /*     ost << "next:" << (*p); */
        /* } */
        return ost;
    }

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */
    OrderPending* _pending = nullptr;
    std::string _clOrdId;
    std::string _symbol;
    FIX::SessionID _owner;
    e2::Side _side;
    e2::OrdType _type;
    e2::Int_e _quantId = 0;
    std::size_t _time;   // market time
    std::size_t _otime;  // order send ticket time
    e2::Int_e _TradeTicket = 0;

    bool _bot = false;

}; /* -----  end of class OrderItem  ----- */

}  // namespace e2q
#endif /* ----- #ifndef ORDER_INC  ----- */
