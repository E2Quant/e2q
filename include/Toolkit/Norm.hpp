/*
 * =====================================================================================
 *
 *       Filename:  Norm.hpp
 *
 *    Description:  Norm
 *
 *        Version:  1.0
 *        Created:  2023年07月27日 09时47分17秒
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

#ifndef NORM_INC
#define NORM_INC

#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "E2L/E2LType.hpp"
#include "assembler/BaseType.hpp"

namespace e2q {

template <typename BaseType>
using BasePtr = std::shared_ptr<BaseType>;

template <typename... Args>
using func_type = std::function<void(Args...)>;

template <typename ret, typename... Args>
using func_type_ret = std::function<ret(Args...)>;

/**
 * bit  time
 */
typedef uint64_t _millisecond;

/**
 * 类型
 */
typedef int64_t SeqType;

/**
 * e2q system number type
 */
typedef SeqType NumberType;

typedef std::atomic<SeqType> atomic_seqtype;

/**
 * trading_protocols 是转成 单笔 bar ->  tick
message type
0 -- index
1 -- base symbol data

Side
0 - bid buy
1 - ask sell

| time           | frame | Side   | Qty  | Price   | Message Type | Stock |
|----------------|-------|--------|------|---------|--------------|-------|
| 28802131792425 |       | 0      | 3000 | 14.7200 | 0            | USO   |
|  ...           |       |        |      |         |              |       |
| 28802131843697 |       |        |      |         | 1            |       |

*/

/**
 * OMS Quote FeedData
 */
enum Trading {
    t_time = 0,
    t_frame,
    t_side,
    t_qty,
    t_price,
    t_msg,
    t_stock,
    t_adjprice
}; /* ----------  end of enum Trading  ---------- */

typedef enum Trading Trading;
#define trading_protocols 8

/**
 * ohlc time type
 */
typedef SeqType _ohlc_t;

/**
 * extend BAR index
 */

enum OHLC_T {
    ohlc_t = 0,
    open_t,
    high_t,
    low_t,
    close_t,
    volume_t,
    adjclose_t
}; /* ----------  end of enum OHLC_T  ---------- */

typedef enum OHLC_T OHLC_T;

// #define BAR_TYPE_UNIT _ohlc_t, OHLC_TYPE_UNIT
#define DEF_OHLC_T "ohlc_t"
#define DEF_OPEN "open"
#define DEF_HIGH "high"
#define DEF_LOW "low"
#define DEF_CLOSE "close"
#define DEF_VOLUME "volume"

#define ohlc_column 7

#define UNIT 1000               //  1 second == 1,000 microsecond
#define MINUTE (UNIT * 60)      // one minute
#define DAY (MINUTE * 60 * 24)  // DAY

/**
 * Silkworm row and column
 */
#define POWNum 9
#define SilkWormRow std::pow(2, POWNum)
#define SilkWormColumn std::pow(2, POWNum)

/**
 *  lock
 */

using BasicLock = std::lock_guard<std::mutex>;

/**
 * sig id
 */

enum __SigId {
    _feedId = 0,  // feed pack
    _strategy_id  // strategy pack
}; /* ----------  end of enum SigId  ---------- */

typedef enum __SigId SigId;

/**
 * for Disruptor , Trigger for status
 */
#define E2Q_RUNING 1
#define E2Q_STOP 0
#define E2Q_EXIST -1

/**
 * 不转小数，需要的时候才转
 */
struct AnalyDict {
    SeqType id;             // unique id for quant node
    SeqType cash;           // allot cash
    int start;              // start timestamp
    int end;                // end timestamp
    int Duration;           //                   3116 days 00:00:00
    SeqType ExposureTime;   // [%]                       94.27
    SeqType EquityFinal;    // [$]  资产净值                   68935.12
    SeqType EquityPeak;     // [$]   资产最高值                   68991.22
    SeqType Return;         // [%]  回报率                            589.35
    SeqType BuyAndHold;     // Return [%]                  703.46
    SeqType ReturnAnn;      // (Ann.) [%]                       25.42
    SeqType VolatilityAnn;  // (Ann.) [%]                   38.43
    SeqType SharpeRatio;    //                        0.66
    SeqType SortinoRatio;   //                          1.30
    SeqType CalmarRatio;    //                            0.77
    SeqType MaxDrawdown;    // [%]                      -33.08
    SeqType AvgDrawdown;    // [%]                       -5.58
    SeqType MaxDrawdownDuration;  //      688 days 00:00:00
    SeqType AvgDrawdownDuration;  //       41 days 00:00:00
    // # Trades                                   93
    SeqType WinRate;           // [%]                            53.76
    SeqType BestTrade;         // [%]                          57.12
    SeqType WorstTrade;        // [%]                        -16.63
    SeqType AvgTrade;          // [%]                           1.96
    SeqType MaxTradeDuration;  //         121 days 00:00:00
    SeqType AvgTradeDuration;  //          32 days 00:00:00
    SeqType ProfitFactor;      //                            2.13
    SeqType Expectancy;        // [%]                           6.91
    SeqType SQN;               //                                      1.78
    SeqType KellyCriterion;    //                        0.6134
    std::string curve;         //                          Equ...
}; /* ----------  end of struct AnalyDict  ---------- */

typedef struct AnalyDict AnalyDict;

/**
 * 交易时间
 */
struct __TradeTime {
    std::size_t open_hour;
    std::size_t open_min;
    std::size_t close_hour;
    std::size_t close_min;
}; /* ----------  end of struct __TradeTime  ---------- */

typedef struct __TradeTime TradeTime;

struct __TraderData {
    long qty;          // ticket -> qty
    double adj_price;  // open adj_price
    double equity;  //  ordstatu(filled, partially_filled) long:实际支出的资产,
                    //  short: 当前 order 的收益
    double margin;  //  freeze 初始冻结资产
}; /* ----------  end of struct __TraderData  ---------- */

typedef struct __TraderData TraderData_t;

/**
 * cash info
 */
struct __TraderInfo {
    int fix_id = 0;
    double total_cash;  // 余额

    //  tick -> TraderData_t
    std::map<std::size_t, TraderData_t> order_cash;  // 当前一笔 订单的资产
}; /* ----------  end of struct TraderInfo  ---------- */

typedef struct __TraderInfo TraderInfo;

/**
 * Ex-rights/Ex-dividend of Emerging Stocks
 *
 */

enum ExType {
    _right = 0,
    _dividend = 1,  // 除权除息
    _shares = 5,    //  股本变化
    _other          //  转配股上市
}; /* ----------  end of enum ExType  ---------- */

typedef enum ExType ExType;

struct __Shares {
    e2::Int_e _Outstanding = 0;   // 除权除息 前流通
    e2::Int_e _Outstandend = 0;   // 除权除息 后流通
    e2::Int_e _MarketCaping = 0;  //  前总市值
    e2::Int_e _MarketCapend = 0;  //  后总市值

}; /* ----------  end of struct __Shares  ---------- */

typedef struct __Shares MShares;

struct __ExRD {
    float _cash = 0;         // cash dividend 分红
    std::size_t _share = 0;  // 送转股
    ExType _extype;          //
    MShares _mshare;
    e2::Int_e _ymd = 0;  // 20070626 ->  yyyymmdd
}; /* ----------  end of struct __ExRD  ---------- */

typedef struct __ExRD ExRD;

struct __MarketInfo {
    // 一手 == 100股
    double _lot_and_share = 100.0;

    /**
     * 10% 保证金率
     */
    double _margin_rate = 0.01;

    double equity(double price, double qty)
    {
        double _price = NUMBERVAL(price);
        return qty * _price * _lot_and_share * (1 + _margin_rate);
    }

    double effect(double price, double qty)
    {
        double effect_equity = _lot_and_share * qty * price;

        return effect_equity;
    }

    double exdr_cash(double qty, double cash)
    {
        double all_cash = qty * cash * _lot_and_share;

        return all_cash;
    }

    // e2l cfi code,  std::string data current stock code
    std::map<int, std::string> _fix_symbols;  // fix full symbols

    std::string _fix_cfg;               //  cfg path
    std::vector<TradeTime> _tradetime;  // trade time
    e2::InitOk _ok;                     // init ok
    e2::Int_e _offer_time = 100;        //  每笔报价时间的间隔

    std::string log_topic = "";  // log kafka topic
                                 //
    std::string _source;         // csv is file dir, kafka is host:port //
}; /* ----------  end of struct __MarketInfo  ---------- */

typedef struct __MarketInfo MarketInfo;

/**
 * FIX44::ExecutionReport
 * open or close order
 * closing 正下单平仓的状态,否会多次平仓
 */
enum __TradeStatus {
    PENDING = 0,       // [open, close] 挂单中 <-- OrdersTotal
    FILLED,            // [close]  <-- history
    PARTIALLY_FILLED,  // [open, close]交易中<-- OrdersTotal
    MARKET,            // [open] 交易完成<-- OrdersTotal
    CLOSEING,          // [closing] <-- close orders
    CLOSED,            // [open] closed  <-- history
    REJECT,            // reject <---history

}; /* ----------  end of enum __TradeStatus  ---------- */

typedef enum __TradeStatus TradeStatus;

struct __OrderInfo {
    e2::Int_e symbol = 0;
    e2::Int_e ticket = 0;  // 订单号
    e2::OrdStatus stat;
    e2::Side side;
    e2::Int_e qty = 0;  // 委託成功數量
    e2::Int_e price = 0;
    e2::Int_e adjpx = 0;  // adj close
    e2::Int_e stoppx = 0;
    e2::Int_e slippage = 0;
    e2::OrdType ordtype;
    e2::Int_e cumqty = 0;                        // 已成交數量
    e2::Int_e avgpx = 0;                         // 委託成功 价格
    e2::Int_e leavesqty = 0;                     /// 剩餘有效量
    e2::Int_e openqty = 0;                       // 可平仓数量
    e2::Int_e closeTck = 0;                      // 平 仓哪一个 ticket 的值
    e2::Int_e tdate = 0;                         //  trade date
    TradeStatus trading = TradeStatus::PENDING;  //  close tradeing
}; /* ----------  end of struct __OrderInfo  ---------- */

typedef struct __OrderInfo OrderInfo;
// Order No. {cl0id, OrderInfo}
typedef std::map<std::string, OrderInfo> OrderInfoMap;
// Order No. {ticket, cl0id}
typedef std::map<e2::Int_e, std::string> OrderTicketMap;

#define IDSymbol(id) \
    do {             \
    } while (0)

#define SymbolID(sym) \
    do {              \
    } while (0)

// https://en.wikipedia.org/wiki/ISO_10962
/**
 * ea node data
 */
struct __FinancialInstrument : public MarketInfo {
    std::vector<e2::TimeFrames> _tf;
    //  弱匹配 id 不一定等于 symbol 的代码 ,以后再优化 吧
    std::vector<size_t> _symbols;
    e2::Offers _offers;  // Offers 报价的方式

    int8_t _gmt = 0;  // gmt  0 = localtime, 1 = gmt+0

    std::size_t _current_index;  // order select api index

    std::size_t _market_pending;  // filled or partially_filled
    /**
     *  只记录当前的状态，过程的细节，通过外部记录，比如 db
     */

    // e2::Int_e quantId = 0;    // quantId for thread id
    std::map<e2::Int_e, OrderInfoMap> _OrderIds;
    OrderTicketMap _OrderTicket;
    AnalyDict _analy;

    std::map<std::thread::id, e2::Int_e> _quantId;  // ea id

    bool _onOpen = false;  // bar on open ,默认不处理

    bool _volume_append = false;   // 默认 volume 不累加
    TraderInfo _cash;              // cash
    double _freeze_cash;           // 下单的时候，先freeze, 真实下单成功了，减去
    std::size_t _freeze_time = 0;  // freeze time
}; /* ----------  end of struct __FinancialInstrument  ---------- */

typedef struct __FinancialInstrument FinancialInstrument;

/**
 * bot limit order book
 */
enum BotLob {
    noll_bot = 0,
    giveaway = 1,
    zic,
}; /* ----------  end of enum BotLob  ---------- */

typedef enum BotLob BotLob;

/**
 * oms database
 */
struct __FinancialFabricate : public MarketInfo {
    // 默认是观察员状态，即只记录交易，但现金和投资组合的价值不会发生变化
    e2::SettleInstMode _sim = e2::SettleInstMode::_observer;

    // 默认是 tif_day
    e2::TimeInForce _tif = e2::TimeInForce::tif_day;

    long effectQty(double price, double margin)
    {
        double _price = NUMBERVAL(price);
        double qty = margin / (_price * _lot_and_share);
        return std::floor(qty);
    }

    BotLob lob;

    // 默认是没有的
    e2::Bool _enable_exrd = e2::Bool::B_FALSE;

    //    std::size_t  symbol code
    std::map<std::size_t, std::vector<ExRD>> _exrd;
    /**
     * 是否暂定交易
     */
    bool _StopOrder = false;

    std::uint8_t _settlement =
        1;  // T+0 T+1 ... fix UnderlyingSettlementType(975)

    std::string _desc;
    std::string _topic;               // kafka topic
    std::string _ccy;                 // currency
    e2::MKType _csv_kafka;            // csv or kafka
    e2::MKType _tick_bar;             //  tick or bar
    e2::Int_e _commission;            // commission
    e2::SymbolTradeMode _trade_mode;  // SymbolTradeMode

    // every one quant process order queue
    std::vector<std::pair<size_t, OrderInfoMap>> _QuantOrders;

    /**
     * anly node process analy
     */
    std::vector<AnalyDict> AnalyDictList;

    // 实时的价格 {symbol,  array}
    std::map<size_t, std::array<SeqType, trading_protocols>> _stock;

    // 版本ID
    int _QuantVerId = 0;

    /**
     * 撮合的时间，主要是回测
     */
    e2::MatchEvent _ME = e2::MatchEvent::ME_OrderIn;

    /**
     * 默认 b-Book, 本地撮合
     */
    e2::BookType _BookType = e2::BookType::BBook;

    /**
     * exdr ticket -> qty, in order match use
     */
    std::map<SeqType, double> _exdr_qty;
}; /* ----------  end of struct __FinancialFabricate  ---------- */

typedef struct __FinancialFabricate FinancialFabricate;

/**
 * oms database
 */
using TradType = std::function<void(std::array<SeqType, trading_protocols> &)>;

/*
 * =====================================================================================
 *        Class:  CompareItem
 *  Description:  for Engine , btree ...
 * =====================================================================================
 */
struct CompareItem {
    bool operator<(const CompareItem &rhs) const;
    bool operator>(const CompareItem &rhs) const;

    bool operator==(const CompareItem &rhs) const;

    friend std::ostream &operator<<(std::ostream &ost, const CompareItem &);

}; /* ----------  end of struct bItem  ---------- */

/**
 *  auto-increment type
 */
struct AutoIncrement {
    AutoIncrement() { init(); }
    AutoIncrement(SeqType i) { _storeId.store(i, std::memory_order_release); }

    ~AutoIncrement() {}
    SeqType StoreId()
    {
        SeqType inc = 1;
        return _storeId.fetch_add(inc, std::memory_order_release) + inc;
    }
    SeqType Id() { return _storeId.load(std::memory_order_acquire); }
    void init() { _storeId.store(0, std::memory_order_release); }

    atomic_seqtype _storeId;  //
    std::thread::id _tid;     // std::thread::id
}; /* ----------  end of struct AutoIncrement  ---------- */
}  // namespace e2q
#endif /* ----- #ifndef NORM_INC  ----- */
