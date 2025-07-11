/*
 * =====================================================================================
 *
 *       Filename:  FixApplication.hpp
 *
 *    Description:  FixApplication
 *
 *        Version:  1.0
 *        Created:  2024年01月10日 14时40分18秒
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

#ifndef FIXAPPLICATION_INC
#define FIXAPPLICATION_INC

#include <exception>
#include <memory>
#include <string>
#include <thread>

#include "E2L/E2LType.hpp"
#include "OMSPack/FixGuard.hpp"
#include "OMSPack/OrderBook/Order.hpp"
#include "OMSPack/OrderBook/OrderMatcher.hpp"
#include "OMSPack/Quote/FeedData.hpp"
#include "OMSPack/SessionGlobal.hpp"
#include "assembler/BaseType.hpp"

#if __cplusplus >= 201703L

/* Now remove the trow */
#define throw(...)
#include <quickfix/Application.h>
#include <quickfix/FixFields.h>
#include <quickfix/FixValues.h>
#include <quickfix/Group.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/SessionID.h>
#include <quickfix/SocketAcceptor.h>
#include <quickfix/fix44/BidRequest.h>
#include <quickfix/fix44/ExecutionReport.h>
#include <quickfix/fix44/MarketDataSnapshotFullRefresh.h>
#include <quickfix/fix44/MessageCracker.h>
#include <quickfix/fix44/NewOrderSingle.h>
#include <quickfix/fix44/OrderCancelReject.h>
#include <quickfix/fix44/OrderCancelReplaceRequest.h>
#include <quickfix/fix44/OrderCancelRequest.h>
#include <quickfix/fix44/Quote.h>
#include <quickfix/fix44/QuoteCancel.h>
#include <quickfix/fix44/QuoteRequest.h>
#include <quickfix/fix44/QuoteResponse.h>
#include <quickfix/fix44/QuoteStatusReport.h>
#undef throw /* reset */
#endif

namespace e2q {

/*
 * ================================
 *        Class:  Application
 *  Description:
 * ================================
 */
class FixApplication : public FIX::Application,
                       public FIX::MessageCracker,
                       public FixGuard {
public:
    /* =============  LIFECYCLE     =================== */
    FixApplication() {}

    FixApplication(std::shared_ptr<ScriptThread> pro)
        : _program(pro) {

          }; /* constructor */
    ~FixApplication() {}
    /* =============  ACCESSORS     =================== */
    void onCreate(const FIX::SessionID&) override {}

    // Notification of a session successfully logging on
    void onLogon(const FIX::SessionID&) override;
    // Notification of a session logging off or disconnecting
    void onLogout(const FIX::SessionID&) override;
    // Notification of admin message being sent to target
    void toAdmin(FIX::Message&, const FIX::SessionID&) override;
    // Notification of app message being sent to target
    void toApp(FIX::Message&, const FIX::SessionID&) override
        /*std::throw(DoNotSend) */;
    // Notification of admin message being received from target
    void fromAdmin(const FIX::Message&,
                   const FIX::SessionID&) override /* throw(
FieldNotFound, IncorrectDataFormat, IncorrectTagValue, RejectLogon)*/
        ;
    // Notification of app message being received from target
    void fromApp(const FIX::Message&, const FIX::SessionID&) override
        /* throw(FieldNotFound,
      IncorrectDataFormat,
      IncorrectTagValue,
      UnsupportedMessageType)*/
        ;
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#endif
    // Member declaration raising the warning.
    // https://stackoverflow.com/questions/18515183/c-overloaded-virtual-function-warning-by-clang

    /**
     * === session ===
     */
    void onMessage(const FIX44::Heartbeat& message,
                   const FIX::SessionID&) override;
    /**
     *  === PreTrade ===
     */
    /**
     *
     * --- QuotationNegotiation ---
     */
    /**
     * request symbology
     */
    void onMessage(const FIX44::QuoteRequest& message,
                   const FIX::SessionID&) override;
    /**
     * cancel quote
     */
    void onMessage(const FIX44::QuoteCancel& message,
                   const FIX::SessionID&) override;

    /**
     *
     * --- MarketData ---
     */

    /**
     * === Trade ===
     */
    /**
     *  brief fix  ExecutionReport <8> message
     *  https://gist.github.com/jdrew1303/e06361070468f6614d52216fb91b79e5
     *  Rules for Matching Engines
     *    The model matching engine supports the following order queues:
     *
     *  Cancel order queue
     *
     *  Market order queues
     *
     *    .Buy
     *    .Sell
     *
     *  Limit order queue
     *    .Buy
     *    .Sell
     *
     *  Stop order queue
     *    .Buy
     *    .Sell
     *
     */
    /**
     * --- SingleGeneralOrderHandling ---
     */

    void onMessage(const FIX44::NewOrderSingle&,
                   const FIX::SessionID&) override;

    void onMessage(const FIX44::OrderCancelRequest&,
                   const FIX::SessionID&) override;

    void onMessage(const FIX44::OrderCancelReject&,
                   const FIX::SessionID&) override;

    void onMessage(const FIX44::OrderCancelReplaceRequest& message,
                   const FIX::SessionID&) override;

    /**
     * === PostTrade ===
     *
     */
#if defined(__clang__)

#pragma clang diagnostic pop
#endif
    void toFeedData(_Resource_ptr ptr, std::shared_ptr<BeamData> beam);

    FIX44::Quote Message();

    void QuoteCancel(const FIX::SessionID&);
    /* =============  MUTATORS      =================== */

    bool processOrder(OrderItem*);
    void processCancel(const std::string& id, const std::string& symbol,
                       const e2::Side&);

    void rejectOrder(const FIX::SessionID&, const FIX::ClOrdID& clOrdID,
                     const FIX::Symbol& symbol, const FIX::Side& side,
                     const std::string& message, e2::Int_e, e2::Int_e,
                     e2::Int_e, e2::Int_e);

    /* void rejectOrder(const OrderLots& order) */
    /* { */
    /*     updateOrder(order, FIX::OrdStatus_REJECTED); */
    /* } */
    void acceptOrder(const OrderLots& order)
    {
        double equity = GlobalMatcher->Equity(order.owner, order.ticket,
                                              FIX::OrdStatus_NEW);

        try {
            updateOrder(order, FIX::OrdStatus_NEW, equity);
        }
        catch (std::exception& e) {
            log::bug("accept:", e.what());
        }
    }
    void fillOrder(const OrderLots& order)
    {
        const char status = order.isFilled ? FIX::OrdStatus_FILLED
                                           : FIX::OrdStatus_PARTIALLY_FILLED;
        double equity =
            GlobalMatcher->Equity(order.owner, order.ticket, status);

        updateOrder(order, status, equity);
    }

    void cancelOrder(const OrderLots& order)
    {
        double equity = GlobalMatcher->Equity(order.owner, order.ticket,
                                              FIX::OrdStatus_CANCELED);

        updateOrder(order, FIX::OrdStatus_CANCELED, equity);
    }

    void matcher(std::string, e2::Int_e, e2::Int_e, e2::Int_e);
    bool end();
    void FeedDataHandle();
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    int E2LScript(e2::OrdType ordType, e2::Side side, double order_qty,
                  e2::Int_e symbol);

    void lob(const FIX::SessionID& sessionid,

             const FIX::Symbol& symbol, const FIX::Side& side,
             const FIX::OrdType& ordType, const FIX::OrderQty& orderQty,
             const FIX::Price& price, const FIX::StopPx& stopx,
             const FIX::ClOrdID& clOrdID, const FIX::OrderID& oid,
             const FIX::ComplianceID& quantId, FIX::TradeDate);

    /* =============  DATA MEMBERS  =================== */

    bool _is_end = false;

    FeedData _fdata;
    std::shared_ptr<ScriptThread> _program = nullptr;

    /**
     * NewOrderSingle
     * OrderReplaceRequest
     *
     */
#define onMess(message)                                                        \
    do {                                                                       \
        FIX::Symbol symbol;                                                    \
        FIX::Side side;                                                        \
        FIX::OrdType ordType;                                                  \
        FIX::OrderQty orderQty;                                                \
        FIX::Price price;                                                      \
        FIX::StopPx stopx;                                                     \
        FIX::ClOrdID clOrdID;                                                  \
        FIX::Account account;                                                  \
        FIX::OrderID oid;                                                      \
        FIX::ComplianceID quantId;                                             \
        FIX::TradeDate tradeDate;                                              \
        message.get(clOrdID);                                                  \
        message.get(symbol);                                                   \
        message.get(side);                                                     \
        message.get(ordType);                                                  \
        message.get(orderQty);                                                 \
        message.getFieldIfSet(price);                                          \
        message.getFieldIfSet(stopx);                                          \
        message.getFieldIfSet(oid);                                            \
        message.getFieldIfSet(quantId);                                        \
        message.getFieldIfSet(tradeDate);                                      \
        lob(sessionID, symbol, side, ordType, orderQty, price, stopx, clOrdID, \
            oid, quantId, tradeDate);                                          \
    } while (0)

}; /* -----  end of class FixApplication  ----- */

}  // namespace e2q
#endif /* ----- #ifndef FIXAPPLICATION_INC  ----- */
