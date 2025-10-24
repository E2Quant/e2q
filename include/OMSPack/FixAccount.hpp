/*
 * =====================================================================================
 *
 *       Filename:  FixAccount.hpp
 *
 *    Description:  FixAccount
 *
 *        Version:  1.0
 *        Created:  2024年01月15日 10时22分36秒
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

#ifndef FIXACCOUNT_INC
#define FIXACCOUNT_INC
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "ControlPack/pack.hpp"
#include "E2L/E2LType.hpp"
#include "E2LScript/ExternClazz.hpp"
#include "FeedPack/FixQuote.hpp"
#include "FixGuard.hpp"
#include "OMSPack/FixGuard.hpp"
#include "OMSPack/IDGenerator.hpp"
#include "OMSPack/foreign.hpp"
#include "OrderBook/Order.hpp"
#include "Toolkit/pack.hpp"
#include "VirtualPack/pack.hpp"
#include "assembler/BaseType.hpp"
#include "quickfix/fix44/BidResponse.h"
#include "quickfix/fix44/ListStatus.h"
#include "quickfix/fix44/QuoteResponse.h"
#if __cplusplus >= 201703L

/* Now remove the trow */
#define throw(...)
#include <quickfix/Application.h>
#include <quickfix/Dictionary.h>
#include <quickfix/Event.h>
#include <quickfix/Exceptions.h>
#include <quickfix/Field.h>
#include <quickfix/FixFieldNumbers.h>
#include <quickfix/FixFields.h>
#include <quickfix/FixValues.h>
#include <quickfix/Message.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/Parser.h>
#include <quickfix/Session.h>
#include <quickfix/SessionID.h>
#include <quickfix/fix44/BidResponse.h>
#include <quickfix/fix44/ExecutionReport.h>
#include <quickfix/fix44/MarketDataSnapshotFullRefresh.h>
#include <quickfix/fix44/MessageCracker.h>
#include <quickfix/fix44/NewOrderSingle.h>
#include <quickfix/fix44/OrderCancelReject.h>
#include <quickfix/fix44/OrderCancelReplaceRequest.h>
#include <quickfix/fix44/Quote.h>
#include <quickfix/fix44/QuoteCancel.h>
#include <quickfix/fix44/QuoteRequest.h>
#include <quickfix/fix44/QuoteStatusReport.h>
#include <quickfix/fix44/TestRequest.h>
#undef throw /* reset */
#endif

namespace e2q {
typedef std::shared_ptr<
    std::function<void(std::array<SeqType, trading_protocols>&)>>
    fixType;
/*
 * ================================
 *        Class:  FixAccount
 *  Description:
 * ================================
 */
class FixAccount : public FIX::Application,
                   public FIX::MessageCracker,
                   public FixGuard {
public:
    /* =============  LIFECYCLE     =================== */
    FixAccount(); /* constructor */
    ~FixAccount() {}
    /* =============  ACCESSORS     =================== */
    // Notification of a session successfully logging on
    void onCreate(const FIX::SessionID& sid) override
    {
        _sender = sid.getSenderCompID();
        _target = sid.getTargetCompID();
        _begin = sid.getBeginString();
    }
    void onLogon(const FIX::SessionID& sid) override
    {
        elog::echo("onLogon:", sid.getSenderCompID().getValue(),
                   " size:", FixPtr->_fix_symbols.size());
    };

    // Notification of a session logging off or disconnecting
    void onLogout(const FIX::SessionID& sid) override {
        //  elog::echo("onLogout:", sid.getSenderCompID().getValue());
    };
    // Notification of admin message being sent to target
    void toAdmin(FIX::Message& msg, const FIX::SessionID& sid) override {
        //          elog::echo(msg.toXML());
    };
    // Notification of app message being sent to target
    void toApp(FIX::Message& msg, const FIX::SessionID& sid) override
    {
//        elog::echo(msg.toString());
#ifdef DEBUG
        try {
            FIX::PossDupFlag possDupFlag;
            if (msg.getHeader().isSetField(possDupFlag)) {
                msg.getHeader().getField(possDupFlag);
                if (possDupFlag) throw FIX::DoNotSend();
            }
        }
        catch (FIX::FieldNotFound& f) {
            elog::bug(f.what(), " field:", f.field);
            // elog::echo(msg.toXML());
        }

#endif
    };
    // Notification of admin message being received from target
    void fromAdmin(const FIX::Message& msg, const FIX::SessionID&) override {
        //  elog::info("fromadmin come in ", msg.toString());
    };
    // Notification of app message being received from target
    void fromApp(const FIX::Message& msg, const FIX::SessionID& sid)

        override
    {
        // elog::info(msg.toXML());
        // elog::info("fromapp come in ........");
        try {
            crack(msg, sid);
        }
        catch (FIX::FieldNotFound& f) {
            elog::bug(f.what());
        }
        catch (FIX::IncorrectDataFormat& i) {
            elog::bug(i.what());
        }
        catch (FIX::IncorrectTagValue& v) {
            elog::bug(v.what());
        }
        catch (FIX::UnsupportedMessageType& t) {
            elog::bug(t.what());
        }
        catch (std::exception& e) {
            elog::bug(e.what());
            elog::echo(msg.toXML());
        }

        //    _event.signal();
    };
    /* =============  MUTATORS      =================== */
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
     *  ===  PreTrade ===
     */
    /**
     *
     * --- QuotationNegotiation ---
     */
    void onMessage(const FIX44::Quote&, const FIX::SessionID&) override;
    void onMessage(const FIX44::QuoteCancel&, const FIX::SessionID&) override;
    void onMessage(const FIX44::MassQuote&, const FIX::SessionID&) override;
    void onMessage(const FIX44::QuoteStatusReport&,
                   const FIX::SessionID&) override;
    void onMessage(const FIX44::QuoteResponse&, const FIX::SessionID&) override;
    /**
     *
     * --- MarketData ---
     */

    void onMessage(const FIX44::MarketDataSnapshotFullRefresh& message,
                   const FIX::SessionID&) override;

    void onMessage(const FIX44::MarketDataRequestReject& message,
                   const FIX::SessionID&) override;

    /**
     * === Trade ===
     */

    /**
     * --- SingleGeneralOrderHandling ---
     */

    void onMessage(const FIX44::ExecutionReport& message,
                   const FIX::SessionID&) override;
    void onMessage(const FIX44::OrderCancelReject& message,
                   const FIX::SessionID&) override;
    void onMessage(const FIX44::OrderStatusRequest& message,
                   const FIX::SessionID&) override;

    void onMessage(const FIX44::BidResponse& message,
                   const FIX::SessionID&) override;

    /**
     * === PostTrade ===
     *
     */
#if defined(__clang__)

#pragma clang diagnostic pop
#endif
    /**
     *  ===  PreTrade ===
     */
    void QuoteRequest(std::vector<std::size_t>&, int isRet);

    /**
     * === Trade ===
     */

    std::size_t NewOrderSingle(Int_e, Int_e side, Int_e qty, Int_e price,
                               Int_e slippage, Int_e ordtype, std::size_t,
                               std::size_t, std::size_t);

    void OrderCancelRequest();
    void OrderReplaceRequest(Int_e, Int_e side, Int_e qty, Int_e stoppx,
                             Int_e slippage, Int_e ordtype, Int_e, std::size_t,
                             std::size_t, std::size_t);
    void OrderCancelReject();

    void defHeader(FIX::Header& header);

    FIX::ClOrdID genClOrdID();
    FIX::OrigClOrdID genOrigClOrdID();
    FIX::Symbol genSymbol(std::string&);
    FIX::Symbol genSymbol(e2::Int_e);

    FIX::OrderQty genOrderQty(Int_e);
    FIX::Price genPrice(Int_e);
    FIX::StopPx genStopPx(Int_e);

    void Init(_Resource_ptr ptr, std::shared_ptr<BeamData> beam_data,
              std::shared_ptr<Shuttle>);

    void CallBack(fixType);
    void wait();
    void QuoteStatusReport(int);
    void quit(const FIX::SessionID&);
    /* =============  OPERATORS     =================== */

protected:
    /* =============  METHODS       =================== */

    /* =============  DATA MEMBERS  =================== */

private:
    /* =============  METHODS       =================== */

    void ChangeTradingStatus(e2::Int_e, std::string&, double);

    void RejectOrCancelNewOrder(e2::Int_e quantId, std::string key,
                                std::size_t ticket, e2::Int_e closetck,
                                std::size_t thread_number, bool rc);
    void UpdateQuantProfit();

    void delisting(std::size_t cficode, std::uint64_t dtime,
                   const FIX::SessionID&);

    void History();

    void TestRequest();
    /* =============  DATA MEMBERS  =================== */
    FIX::SenderCompID _sender;
    FIX::TargetCompID _target;
    FIX::BeginString _begin;
    FIX::Event _event;

    FixQuote _fq;

    std::shared_ptr<BeamData> _beam_data = nullptr;
    fixType _call;

    bool _is_end = false;

    _Resource_ptr _source_ptr{nullptr};
}; /* -----  end of class FixAccount  ----- */

/**
 * fix account extern for el2 script use
 */
extern e2q::FixAccount fix_application;

}  // namespace e2q
#endif /* ----- #ifndef FIXACCOUNT_INC  ----- */
