--
-- PostgreSQL database dump
--

-- Dumped from database version 15.5 (Debian 15.5-0+deb12u1)
-- Dumped by pg_dump version 15.5 (Debian 15.5-0+deb12u1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: api; Type: SCHEMA; Schema: -; Owner: dbuser
--

CREATE SCHEMA api;


ALTER SCHEMA api OWNER TO dbuser;

--
-- Name: tablefunc; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS tablefunc WITH SCHEMA public;


--
-- Name: EXTENSION tablefunc; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION tablefunc IS 'functions that manipulate whole tables, including crosstab';


--
-- Name: quant_profit(integer); Type: FUNCTION; Schema: api; Owner: dbuser
--

CREATE FUNCTION api.quant_profit(_qid integer) RETURNS TABLE(id bigint, profit_x double precision, profit_sum double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
RETURN QUERY
    SELECT
    id,
    "margin",
    "profit",
    to_timestamp("ctime") AS date,
    "side",
    diff as "profit_x",
    sum(diff) over (
        order by id
    ) as "profit_sum"
from (
        SELECT
            id, "margin", "profit", "ticket", "side", "ctime", (
                 CASE WHEN "side" !=3 THEN  
                    "profit" - coalesce(
                    lag("margin", 1) OVER (
                        ORDER BY id
                    ), ~( SELECT (
                            (
                                1.0 / (
                                    SELECT count(DISTINCT "quantid")
                                    from "analse"
                                )
                            ) * 1000000.0
                        )::numeric::integer -1)
                    )
                ELSE 
                 "profit"                      
                END                
            ) as diff
        from "trade_report"
        WHERE
            "ticket" in (
                SELECT "ticket"
                from "trades"
                WHERE
                    "quantid" = _qid
                    AND "stat" = 2
                    AND "side" != 3
            )
    ) as profitx;
    
END
$$;


ALTER FUNCTION api.quant_profit(_qid integer) OWNER TO dbuser;

--
-- Name: quant_profit(integer, double precision); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_profit(_qid integer, _defcash double precision) RETURNS TABLE(id integer, margin double precision, profits double precision, pday text, pside integer, profit_x double precision, profit_sum double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
SELECT
    _qid AS id,
    profitx.margin,
    profitx.profit,
    to_char(profitx.ctime, 'YYYY-MM-DD HH24:MI:SS') AS pday,
    profitx.side as pside,
    profitx.diff as "profit_x",
    sum(profitx.diff) over (
        order by profitx.id
    ) as "profit_sum"
from (
        SELECT ntrade_report.id,ntrade_report.margin, ntrade_report.profit,ntrade_report.ticket,ntrade_report.side, ntrade_report.ctime
, (
                 CASE WHEN ntrade_report.side != 3 THEN  
                    ntrade_report.profit - coalesce(
                    lag(ntrade_report.margin, 1) OVER (
                        ORDER BY ntrade_report.id
                    ), ~( SELECT (
                            round(
                                CAST(
                                    float8 (
                                        1.0 / (
                                             SELECT  count(DISTINCT "argv") as num
                                            from "analse"
                                            WHERE
                                                "verid" = (
                                                    SELECT "verid"
                                                    FROM "analse"
                                                    WHERE
                                                        "quantid" = _qid
                                                    LIMIT 1
                                                )
                                                AND "name" = (
                                                    SELECT "name"
                                                    from "analse"
                                                    WHERE
                                                        "quantid" = _qid
                                                ) 
                                        )
                                    ) as numeric
                                ), 2
                            ) * _defcash
                        )::numeric::integer -1)
                    )
                ELSE 
                ntrade_report.profit                      
                END                
            ) as diff            
 FROM (
    SELECT
            tr.id,              
            (CASE 
                WHEN tr."side" != 2 THEN  
               tr.margin - COALESCE( (SELECT sum(profit) FROM trade_report WHERE "ticket" =  tr."ticket" AND "side"= 3  ) ,0)
                ELSE  
                tr."margin"
            END ) as margin , 
            tr.profit,
            tr.ticket, tr.side,    (to_timestamp(tr.ctime / 1000) + ((tr.ctime % 1000 ) || ' milliseconds') :: INTERVAL) AS ctime    
        from "trade_report" tr
        WHERE
            tr."ticket" in (
                SELECT "ticket"
                from "trades"
                WHERE
                    "quantid" = _qid
                    AND "stat" = 2
                    AND trades.side != 3
            )
            AND tr."side"!=3 ) as ntrade_report 
    ) as profitx;
END; $$;


ALTER FUNCTION public.quant_profit(_qid integer, _defcash double precision) OWNER TO dbuser;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: analselog; Type: TABLE; Schema: api; Owner: dbuser
--

CREATE TABLE api.analselog (
    id integer NOT NULL,
    quantid integer,
    "values" double precision,
    type integer,
    ctime bigint,
    key integer DEFAULT 0
);


ALTER TABLE api.analselog OWNER TO dbuser;

--
-- Name: TABLE analselog; Type: COMMENT; Schema: api; Owner: dbuser
--

COMMENT ON TABLE api.analselog IS '记录指标数值的';


--
-- Name: COLUMN analselog.quantid; Type: COMMENT; Schema: api; Owner: dbuser
--

COMMENT ON COLUMN api.analselog.quantid IS '对应 analse 的 quantid';


--
-- Name: COLUMN analselog."values"; Type: COMMENT; Schema: api; Owner: dbuser
--

COMMENT ON COLUMN api.analselog."values" IS '数据';


--
-- Name: COLUMN analselog.type; Type: COMMENT; Schema: api; Owner: dbuser
--

COMMENT ON COLUMN api.analselog.type IS '类型';


--
-- Name: COLUMN analselog.ctime; Type: COMMENT; Schema: api; Owner: dbuser
--

COMMENT ON COLUMN api.analselog.ctime IS '时间';


--
-- Name: COLUMN analselog.key; Type: COMMENT; Schema: api; Owner: dbuser
--

COMMENT ON COLUMN api.analselog.key IS 'key';


--
-- Name: analselog_id_seq; Type: SEQUENCE; Schema: api; Owner: dbuser
--

ALTER TABLE api.analselog ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME api.analselog_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: stockinfo; Type: TABLE; Schema: api; Owner: dbuser
--

CREATE TABLE api.stockinfo (
    id integer NOT NULL,
    symbol integer DEFAULT 0,
    stock character varying(255)
);


ALTER TABLE api.stockinfo OWNER TO dbuser;

--
-- Name: TABLE stockinfo; Type: COMMENT; Schema: api; Owner: dbuser
--

COMMENT ON TABLE api.stockinfo IS '保存股票信息';


--
-- Name: COLUMN stockinfo.symbol; Type: COMMENT; Schema: api; Owner: dbuser
--

COMMENT ON COLUMN api.stockinfo.symbol IS 'cficode';


--
-- Name: COLUMN stockinfo.stock; Type: COMMENT; Schema: api; Owner: dbuser
--

COMMENT ON COLUMN api.stockinfo.stock IS 'stock name';


--
-- Name: todos_id_seq; Type: SEQUENCE; Schema: api; Owner: dbuser
--

ALTER TABLE api.stockinfo ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME api.todos_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: account; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.account (
    id integer NOT NULL,
    sessionid integer,
    balance double precision,
    credit double precision,
    equity double precision,
    leverage double precision,
    freemargin double precision,
    margin double precision,
    profit double precision,
    ctime integer
);


ALTER TABLE public.account OWNER TO dbuser;

--
-- Name: TABLE account; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON TABLE public.account IS '账户信息';


--
-- Name: COLUMN account.sessionid; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.account.sessionid IS 'fixsession id';


--
-- Name: COLUMN account.balance; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.account.balance IS '账户余额';


--
-- Name: COLUMN account.credit; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.account.credit IS '帐户信用
';


--
-- Name: COLUMN account.equity; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.account.equity IS '账户的权益价值';


--
-- Name: COLUMN account.leverage; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.account.leverage IS '账户杠杆';


--
-- Name: COLUMN account.freemargin; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.account.freemargin IS '可用保证金';


--
-- Name: COLUMN account.margin; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.account.margin IS '账户保证金';


--
-- Name: COLUMN account.profit; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.account.profit IS '账户利润';


--
-- Name: COLUMN account.ctime; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.account.ctime IS 'ctime';


--
-- Name: account_id_seq; Type: SEQUENCE; Schema: public; Owner: dbuser
--

ALTER TABLE public.account ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.account_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: analse; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.analse (
    id integer NOT NULL,
    aid integer,
    quantid integer,
    name character varying(255),
    argv character varying(255),
    verid integer,
    ctime integer,
    profit double precision DEFAULT 0
);


ALTER TABLE public.analse OWNER TO dbuser;

--
-- Name: TABLE analse; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON TABLE public.analse IS '记录参数的内容,AnalseDB';


--
-- Name: COLUMN analse.aid; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analse.aid IS '指标id';


--
-- Name: COLUMN analse.quantid; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analse.quantid IS '策略id';


--
-- Name: COLUMN analse.name; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analse.name IS '指标名字';


--
-- Name: COLUMN analse.argv; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analse.argv IS 'argv list';


--
-- Name: COLUMN analse.verid; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analse.verid IS '版本 id';


--
-- Name: COLUMN analse.ctime; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analse.ctime IS '创建时间';


--
-- Name: COLUMN analse.profit; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analse.profit IS '回测最后的收益';


--
-- Name: analytics; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.analytics (
    "Start" integer,
    "End" integer,
    "Duration" integer,
    "ExposureTime" numeric,
    "EquityFinal" bigint,
    "EquityPeak" bigint,
    "Return" bigint,
    "BuyAndHold" bigint,
    "ReturnAnn" bigint,
    "VolatilityAnn" bigint,
    "SharpeRatio" bigint,
    "SortinoRatio" bigint,
    "CalmarRatio" bigint,
    "MaxDrawdown" bigint,
    "AvgDrawdown" bigint,
    "MaxDrawdownDuration" bigint,
    "AvgDrawdownDuration" bigint,
    "Alpha" bigint,
    "Beta" bigint,
    id bigint NOT NULL
);


ALTER TABLE public.analytics OWNER TO dbuser;

--
-- Name: TABLE analytics; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON TABLE public.analytics IS '总的信息';


--
-- Name: COLUMN analytics."Start"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."Start" IS 'start time';


--
-- Name: COLUMN analytics."End"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."End" IS 'end time';


--
-- Name: COLUMN analytics."Duration"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."Duration" IS 'Duration days';


--
-- Name: COLUMN analytics."ExposureTime"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."ExposureTime" IS 'Exposure Time [%]                       94.27';


--
-- Name: COLUMN analytics."EquityFinal"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."EquityFinal" IS 'Equity Final [$]                     68935.12';


--
-- Name: COLUMN analytics."EquityPeak"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."EquityPeak" IS 'Equity Peak [$]                      68991.22';


--
-- Name: COLUMN analytics."Return"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."Return" IS 'Return [%]                             589.35';


--
-- Name: COLUMN analytics."BuyAndHold"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."BuyAndHold" IS 'Buy & Hold Return [%]                  703.46';


--
-- Name: COLUMN analytics."ReturnAnn"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."ReturnAnn" IS 'Return (Ann.) [%]                       25.42';


--
-- Name: COLUMN analytics."VolatilityAnn"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."VolatilityAnn" IS 'Volatility (Ann.) [%]                   38.43';


--
-- Name: COLUMN analytics."SharpeRatio"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."SharpeRatio" IS 'Sharpe Ratio                             0.66';


--
-- Name: COLUMN analytics."SortinoRatio"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."SortinoRatio" IS 'Sortino Ratio                            1.30';


--
-- Name: COLUMN analytics."CalmarRatio"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."CalmarRatio" IS 'Calmar Ratio                             0.77';


--
-- Name: COLUMN analytics."MaxDrawdown"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."MaxDrawdown" IS 'Max. Drawdown [%]                      -33.08';


--
-- Name: COLUMN analytics."AvgDrawdown"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."AvgDrawdown" IS 'Avg. Drawdown [%]                       -5.58';


--
-- Name: COLUMN analytics."MaxDrawdownDuration"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."MaxDrawdownDuration" IS 'Max. Drawdown Duration      688 days 00:00:00';


--
-- Name: COLUMN analytics."AvgDrawdownDuration"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analytics."AvgDrawdownDuration" IS 'Avg. Drawdown Duration       41 days 00:00:00';


--
-- Name: analytics_id_seq; Type: SEQUENCE; Schema: public; Owner: dbuser
--

CREATE SEQUENCE public.analytics_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.analytics_id_seq OWNER TO dbuser;

--
-- Name: analytics_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: dbuser
--

ALTER SEQUENCE public.analytics_id_seq OWNED BY public.analytics.id;


--
-- Name: comment; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.comment (
    id integer NOT NULL,
    ticket bigint DEFAULT 0,
    side integer,
    quantid integer,
    oe integer
);


ALTER TABLE public.comment OWNER TO dbuser;

--
-- Name: TABLE comment; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON TABLE public.comment IS 'ticket的日志';


--
-- Name: COLUMN comment.ticket; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.comment.ticket IS '对应的ticket';


--
-- Name: COLUMN comment.side; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.comment.side IS '交易方向';


--
-- Name: COLUMN comment.quantid; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.comment.quantid IS 'quantid';


--
-- Name: COLUMN comment.oe; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.comment.oe IS 'order event';


--
-- Name: comment_id_seq; Type: SEQUENCE; Schema: public; Owner: dbuser
--

ALTER TABLE public.comment ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.comment_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: exdr; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.exdr (
    id integer NOT NULL,
    symbol integer,
    cash double precision,
    shares double precision,
    extype integer,
    ymd integer,
    outstanding double precision,
    outstandend double precision,
    marketcaping double precision,
    marketcapend double precision
);


ALTER TABLE public.exdr OWNER TO dbuser;

--
-- Name: TABLE exdr; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON TABLE public.exdr IS '除权除息';


--
-- Name: COLUMN exdr.symbol; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.exdr.symbol IS '股票名 cficode';


--
-- Name: COLUMN exdr.cash; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.exdr.cash IS '分红';


--
-- Name: COLUMN exdr.shares; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.exdr.shares IS '转股数';


--
-- Name: trades; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.trades (
    id bigint NOT NULL,
    symbol integer DEFAULT 0,
    ticket bigint,
    stat integer DEFAULT 0 NOT NULL,
    side integer DEFAULT 0 NOT NULL,
    qty bigint DEFAULT 0 NOT NULL,
    price double precision DEFAULT 0 NOT NULL,
    stoppx double precision DEFAULT 0,
    slippage integer DEFAULT 0,
    ordtype integer DEFAULT 0 NOT NULL,
    cumqty bigint DEFAULT 0,
    avgpx double precision DEFAULT 0,
    leavesqty integer DEFAULT 0,
    openqty integer DEFAULT 0,
    closetck bigint DEFAULT 0,
    ctime bigint DEFAULT 0,
    quantid bigint DEFAULT 0,
    otime bigint DEFAULT 0,
    adjpx double precision DEFAULT 0,
    amount double precision DEFAULT 0
);


ALTER TABLE public.trades OWNER TO dbuser;

--
-- Name: TABLE trades; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON TABLE public.trades IS '交易表';


--
-- Name: COLUMN trades.symbol; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.symbol IS '股票代码cfi code';


--
-- Name: COLUMN trades.ticket; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.ticket IS '订单号';


--
-- Name: COLUMN trades.stat; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.stat IS '状态
0 = new
1 = Partially_filled,
2 = Filled';


--
-- Name: COLUMN trades.side; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.side IS 'side
1 = buy
2 = sell';


--
-- Name: COLUMN trades.qty; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.qty IS '订单量';


--
-- Name: COLUMN trades.price; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.price IS '开仓 交易价格';


--
-- Name: COLUMN trades.stoppx; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.stoppx IS '平仓价格';


--
-- Name: COLUMN trades.slippage; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.slippage IS '滑点';


--
-- Name: COLUMN trades.ordtype; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.ordtype IS 'market = 1
limit = 2
stop = 3
stop_limit = 4';


--
-- Name: COLUMN trades.cumqty; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.cumqty IS '当前累计成交的量';


--
-- Name: COLUMN trades.avgpx; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.avgpx IS '平均价';


--
-- Name: COLUMN trades.leavesqty; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.leavesqty IS '剩下多少没成交';


--
-- Name: COLUMN trades.openqty; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.openqty IS '剩下多少没平仓';


--
-- Name: COLUMN trades.closetck; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.closetck IS '平仓哪个ticket';


--
-- Name: COLUMN trades.ctime; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.ctime IS 'market time';


--
-- Name: COLUMN trades.quantid; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.quantid IS '策略 id ';


--
-- Name: COLUMN trades.otime; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.otime IS 'order send tocket time';


--
-- Name: COLUMN trades.adjpx; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.adjpx IS '当前计算的adj价格';


--
-- Name: COLUMN trades.amount; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trades.amount IS '为此订单的成交累总金额';


--
-- Name: e2q_history; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_history AS
 SELECT buy.symbol,
    ( SELECT stockinfo.stock
           FROM api.stockinfo
          WHERE (buy.symbol = stockinfo.symbol)
         LIMIT 1) AS stock,
    buy.price AS buy_price,
    (to_timestamp(((buy.ctime / 1000))::double precision) + (((buy.ctime % (1000)::bigint) || ' milliseconds'::text))::interval) AS buy_time,
    sell.stoppx AS stop_price,
    (to_timestamp(((sell.ctime / 1000))::double precision) + (((sell.ctime % (1000)::bigint) || ' milliseconds'::text))::interval) AS stop_time,
    sell.adjpx AS sell_adjpx,
    buy.adjpx AS buy_adjpx,
    round(((((sell.adjpx - buy.adjpx) / buy.adjpx) * (100)::double precision))::numeric, 3) AS profit,
    (sell.closetck)::text AS closetck,
        CASE
            WHEN (com.oe = 1) THEN 'StopLoss'::text
            WHEN (com.oe = 2) THEN 'StopProfit'::text
            ELSE 'StrategicClosing'::text
        END AS "LossOrProfit",
    COALESCE(( SELECT sum(exdr.cash) AS sum
           FROM public.exdr
          WHERE ((exdr.symbol = buy.symbol) AND ((to_char((((to_timestamp(((buy.ctime / 1000))::double precision) + (((buy.ctime % (1000)::bigint) || ' milliseconds'::text))::interval))::date)::timestamp with time zone, 'YYYYMMDD'::text))::integer <= exdr.ymd) AND (exdr.ymd <= (to_char((((to_timestamp(((sell.ctime / 1000))::double precision) + (((sell.ctime % (1000)::bigint) || ' milliseconds'::text))::interval))::date)::timestamp with time zone, 'YYYYMMDD'::text))::integer))), (0)::double precision) AS cash,
    COALESCE(( SELECT sum(exdr.shares) AS sum
           FROM public.exdr
          WHERE ((exdr.symbol = buy.symbol) AND ((to_char((((to_timestamp(((buy.ctime / 1000))::double precision) + (((buy.ctime % (1000)::bigint) || ' milliseconds'::text))::interval))::date)::timestamp with time zone, 'YYYYMMDD'::text))::integer <= exdr.ymd) AND (exdr.ymd <= (to_char((((to_timestamp(((sell.ctime / 1000))::double precision) + (((sell.ctime % (1000)::bigint) || ' milliseconds'::text))::interval))::date)::timestamp with time zone, 'YYYYMMDD'::text))::integer))), (0)::double precision) AS share,
    a.quantid,
    buy.ticket AS bticket,
    sell.ticket AS sticket,
    ( SELECT analselog."values"
           FROM api.analselog
          WHERE (analselog.key = buy.ticket)
         LIMIT 1) AS "position"
   FROM public.trades buy,
    public.trades sell,
    public.analse a,
    public.comment com
  WHERE ((buy.ticket = sell.closetck) AND (buy.stat = 2) AND (sell.stat = 2) AND (sell.stoppx > (0)::double precision) AND (buy.price > (0)::double precision) AND (com.ticket = sell.closetck) AND (buy.quantid = a.quantid))
  ORDER BY buy.id;


ALTER TABLE public.e2q_history OWNER TO dbuser;

--
-- Name: e2q_postion; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_postion AS
 SELECT a.quantid,
    l."values",
    l.type,
    (to_timestamp(((l.ctime / 1000))::double precision) + (((l.ctime % (1000)::bigint) || ' milliseconds'::text))::interval) AS date,
    (((a.name)::text || '_'::text) || (a.argv)::text) AS rule
   FROM api.analselog l,
    public.analse a
  WHERE (a.quantid = l.quantid)
  ORDER BY l.ctime DESC;


ALTER TABLE public.e2q_postion OWNER TO dbuser;

--
-- Name: e2q_primitive; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_primitive AS
 SELECT t.id,
    t.stat,
    t.side,
    t.price,
    t.adjpx,
    t.stoppx,
    t.cumqty,
    t.leavesqty,
    t.openqty,
    t.qty,
    (to_timestamp(((t.ctime / 1000))::double precision) + (((t.ctime % (1000)::bigint) || ' milliseconds'::text))::interval) AS date,
    a.argv,
    a.name,
    t.ticket,
    t.closetck,
    t.amount
   FROM public.trades t,
    public.analse a
  WHERE (t.quantid = a.quantid)
  ORDER BY t.id;


ALTER TABLE public.e2q_primitive OWNER TO dbuser;

--
-- Name: e2q_profit; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_profit AS
 SELECT DISTINCT analse.argv,
    sum(analse.profit) AS profits
   FROM public.analse
  GROUP BY analse.argv
  ORDER BY (sum(analse.profit)) DESC;


ALTER TABLE public.e2q_profit OWNER TO dbuser;

--
-- Name: VIEW e2q_profit; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON VIEW public.e2q_profit IS '各参数的收益';


--
-- Name: exdr_id_seq; Type: SEQUENCE; Schema: public; Owner: dbuser
--

ALTER TABLE public.exdr ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.exdr_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: fixsession; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.fixsession (
    id integer NOT NULL,
    beginstring character varying(255) NOT NULL,
    sendercompid character varying(255),
    targetcompid character varying(255) NOT NULL,
    filestorepath character varying(255),
    datadictionary character varying(255) NOT NULL,
    ctime integer,
    host character varying(255),
    port integer
);


ALTER TABLE public.fixsession OWNER TO dbuser;

--
-- Name: TABLE fixsession; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON TABLE public.fixsession IS '记录 fix session';


--
-- Name: COLUMN fixsession.beginstring; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.fixsession.beginstring IS 'beginstring';


--
-- Name: COLUMN fixsession.sendercompid; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.fixsession.sendercompid IS 'sendercompid';


--
-- Name: COLUMN fixsession.targetcompid; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.fixsession.targetcompid IS 'targetcompid';


--
-- Name: COLUMN fixsession.filestorepath; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.fixsession.filestorepath IS 'filestorepath';


--
-- Name: COLUMN fixsession.datadictionary; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.fixsession.datadictionary IS 'datadictionary';


--
-- Name: COLUMN fixsession.ctime; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.fixsession.ctime IS 'ctime';


--
-- Name: COLUMN fixsession.host; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.fixsession.host IS 'SocketConnectHost';


--
-- Name: COLUMN fixsession.port; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.fixsession.port IS 'SocketConnectPort';


--
-- Name: fixsession_id_seq; Type: SEQUENCE; Schema: public; Owner: dbuser
--

ALTER TABLE public.fixsession ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.fixsession_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: indicator_id_seq; Type: SEQUENCE; Schema: public; Owner: dbuser
--

ALTER TABLE public.analse ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.indicator_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: ohlc; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.ohlc (
    date timestamp with time zone,
    open numeric,
    low numeric,
    high numeric,
    close numeric,
    volume bigint,
    key character varying(255),
    id integer NOT NULL
);


ALTER TABLE public.ohlc OWNER TO dbuser;

--
-- Name: ohlc_id_seq; Type: SEQUENCE; Schema: public; Owner: dbuser
--

CREATE SEQUENCE public.ohlc_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.ohlc_id_seq OWNER TO dbuser;

--
-- Name: ohlc_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: dbuser
--

ALTER SEQUENCE public.ohlc_id_seq OWNED BY public.ohlc.id;


--
-- Name: profit; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.profit (
    id integer NOT NULL,
    "WinRate" bigint,
    "BestTrade" bigint,
    "WorstTrade" bigint,
    "AvgTrade" bigint,
    "MaxTradeDuration" integer,
    "AvgTradeDuration" integer,
    "ProfitFactor" bigint,
    "Expectancy" bigint,
    "SQN" bigint,
    "KellyCriterion" bigint
);


ALTER TABLE public.profit OWNER TO dbuser;

--
-- Name: TABLE profit; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON TABLE public.profit IS 'Trades table';


--
-- Name: COLUMN profit."WinRate"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.profit."WinRate" IS 'Win Rate [%]                            53.76 ';


--
-- Name: COLUMN profit."BestTrade"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.profit."BestTrade" IS 'Best Trade [%]                          57.12';


--
-- Name: COLUMN profit."WorstTrade"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.profit."WorstTrade" IS 'Worst Trade [%]                        -16.63';


--
-- Name: COLUMN profit."AvgTrade"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.profit."AvgTrade" IS 'Avg. Trade [%]                           1.96';


--
-- Name: COLUMN profit."MaxTradeDuration"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.profit."MaxTradeDuration" IS 'Max. Trade Duration         121 days 00:00:00';


--
-- Name: COLUMN profit."AvgTradeDuration"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.profit."AvgTradeDuration" IS 'Avg. Trade Duration          32 days 00:00:00';


--
-- Name: COLUMN profit."ProfitFactor"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.profit."ProfitFactor" IS 'Profit Factor                            2.13';


--
-- Name: COLUMN profit."Expectancy"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.profit."Expectancy" IS 'Expectancy [%]                           6.91';


--
-- Name: COLUMN profit."SQN"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.profit."SQN" IS 'SQN                                      1.78';


--
-- Name: COLUMN profit."KellyCriterion"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.profit."KellyCriterion" IS 'Kelly Criterion                        0.6134';


--
-- Name: trade_info; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.trade_info (
    id integer NOT NULL,
    version character varying(255),
    desz text,
    ctime integer DEFAULT 0 NOT NULL,
    active integer DEFAULT 0
);


ALTER TABLE public.trade_info OWNER TO dbuser;

--
-- Name: TABLE trade_info; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON TABLE public.trade_info IS '策略的版本号';


--
-- Name: COLUMN trade_info.version; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_info.version IS '版本号';


--
-- Name: COLUMN trade_info.desz; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_info.desz IS '备注';


--
-- Name: COLUMN trade_info.ctime; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_info.ctime IS 'create time';


--
-- Name: COLUMN trade_info.active; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_info.active IS '当前活跃的版本';


--
-- Name: trade_info_id_seq; Type: SEQUENCE; Schema: public; Owner: dbuser
--

ALTER TABLE public.trade_info ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.trade_info_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: trade_report; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.trade_report (
    id integer NOT NULL,
    ticket bigint,
    sessionid integer,
    balance double precision,
    credit double precision DEFAULT 0,
    equity double precision DEFAULT 0,
    leverage double precision DEFAULT 1,
    freemargin double precision DEFAULT 0,
    margin double precision DEFAULT 0,
    profit double precision DEFAULT 0,
    ctime bigint,
    side integer DEFAULT 1
);


ALTER TABLE public.trade_report OWNER TO dbuser;

--
-- Name: TABLE trade_report; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON TABLE public.trade_report IS '订单报表';


--
-- Name: COLUMN trade_report.ticket; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_report.ticket IS 'ticket';


--
-- Name: COLUMN trade_report.sessionid; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_report.sessionid IS 'fixsession id';


--
-- Name: COLUMN trade_report.balance; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_report.balance IS '账户余额';


--
-- Name: COLUMN trade_report.credit; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_report.credit IS '帐户信用
';


--
-- Name: COLUMN trade_report.equity; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_report.equity IS '账户的权益价值';


--
-- Name: COLUMN trade_report.leverage; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_report.leverage IS '账户杠杆';


--
-- Name: COLUMN trade_report.freemargin; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_report.freemargin IS '可用保证金';


--
-- Name: COLUMN trade_report.margin; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_report.margin IS '账户保证金';


--
-- Name: COLUMN trade_report.profit; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_report.profit IS '账户利润';


--
-- Name: COLUMN trade_report.ctime; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_report.ctime IS 'ctime';


--
-- Name: COLUMN trade_report.side; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.trade_report.side IS '开仓平仓,1:buy,2:sell,3exdr';


--
-- Name: trade_report_id_seq; Type: SEQUENCE; Schema: public; Owner: dbuser
--

ALTER TABLE public.trade_report ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.trade_report_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- Name: trades_id_seq; Type: SEQUENCE; Schema: public; Owner: dbuser
--

CREATE SEQUENCE public.trades_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.trades_id_seq OWNER TO dbuser;

--
-- Name: trades_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: dbuser
--

ALTER SEQUENCE public.trades_id_seq OWNED BY public.trades.id;


--
-- Name: analytics id; Type: DEFAULT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.analytics ALTER COLUMN id SET DEFAULT nextval('public.analytics_id_seq'::regclass);


--
-- Name: ohlc id; Type: DEFAULT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.ohlc ALTER COLUMN id SET DEFAULT nextval('public.ohlc_id_seq'::regclass);


--
-- Name: trades id; Type: DEFAULT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.trades ALTER COLUMN id SET DEFAULT nextval('public.trades_id_seq'::regclass);


--
-- Data for Name: analselog; Type: TABLE DATA; Schema: api; Owner: dbuser
--

COPY api.analselog (id, quantid, "values", type, ctime, key) FROM stdin;
\.


--
-- Data for Name: stockinfo; Type: TABLE DATA; Schema: api; Owner: dbuser
--

COPY api.stockinfo (id, symbol, stock) FROM stdin;
\.


--
-- Data for Name: account; Type: TABLE DATA; Schema: public; Owner: dbuser
--

COPY public.account (id, sessionid, balance, credit, equity, leverage, freemargin, margin, profit, ctime) FROM stdin;
3	3	0	0	0	1	0	0	0	1723003101
4	4	0	0	0	1	0	0	0	1723003101
15	15	0	0	0	1	0	0	0	1723003101
14	14	0	0	0	1	0	0	0	1723003101
24	24	0	0	0	1	0	0	0	1723003101
25	25	0	0	0	1	0	0	0	1723003101
38	38	0	0	0	1	0	0	0	1723003101
21	21	0	0	0	1	0	0	0	1723003101
12	12	0	0	0	1	0	0	0	1723003101
13	13	0	0	0	1	0	0	0	1723003101
16	16	0	0	0	1	0	0	0	1723003101
17	17	0	0	0	1	0	0	0	1723003101
45	45	0	0	0	1	0	0	0	1723003101
48	48	0	0	0	1	0	0	0	1723003101
50	50	0	0	0	1	0	0	0	1723003101
34	34	0	0	0	1	0	0	0	1723003101
35	35	0	0	0	1	0	0	0	1723003101
19	19	0	0	0	1	0	0	0	1723003101
26	26	0	0	0	1	0	0	0	1723003101
27	27	0	0	0	1	0	0	0	1723003101
28	28	0	0	0	1	0	0	0	1723003101
29	29	0	0	0	1	0	0	0	1723003101
30	30	0	0	0	1	0	0	0	1723003101
31	31	0	0	0	1	0	0	0	1723003101
23	23	0	0	0	1	0	0	0	1723003101
7	7	0	0	0	1	0	0	0	1723003101
9	9	0	0	0	1	0	0	0	1723003101
10	10	0	0	0	1	0	0	0	1723003101
8	8	0	0	0	1	0	0	0	1723003101
6	6	0	0	0	1	0	0	0	1723003101
5	5	0	0	0	1	0	0	0	1723003101
42	42	0	0	0	1	0	0	0	1723003101
41	41	0	0	0	1	0	0	0	1723003101
49	49	0	0	0	1	0	0	0	1723003101
22	22	0	0	0	1	0	0	0	1723003101
46	46	0	0	0	1	0	0	0	1723003101
47	47	0	0	0	1	0	0	0	1723003101
33	33	0	0	0	1	0	0	0	1723003101
39	39	0	0	0	1	0	0	0	1723003101
40	40	0	0	0	1	0	0	0	1723003101
44	44	0	0	0	1	0	0	0	1723003101
36	36	0	0	0	1	0	0	0	1723003101
37	37	0	0	0	1	0	0	0	1723003101
20	20	0	0	0	1	0	0	0	1723003101
18	18	0	0	0	1	0	0	0	1723003101
11	11	0	0	0	1	0	0	0	1723003101
43	43	0	0	0	1	0	0	0	1723003101
32	32	0	0	0	1	0	0	0	1723003101
2	2	0	0	0	1	0	0	0	1723003101
1	1	0	0	0	1	0	0	0	1723003101
\.


--
-- Data for Name: analse; Type: TABLE DATA; Schema: public; Owner: dbuser
--

COPY public.analse (id, aid, quantid, name, argv, verid, ctime, profit) FROM stdin;
\.


--
-- Data for Name: analytics; Type: TABLE DATA; Schema: public; Owner: dbuser
--

COPY public.analytics ("Start", "End", "Duration", "ExposureTime", "EquityFinal", "EquityPeak", "Return", "BuyAndHold", "ReturnAnn", "VolatilityAnn", "SharpeRatio", "SortinoRatio", "CalmarRatio", "MaxDrawdown", "AvgDrawdown", "MaxDrawdownDuration", "AvgDrawdownDuration", "Alpha", "Beta", id) FROM stdin;
\.


--
-- Data for Name: comment; Type: TABLE DATA; Schema: public; Owner: dbuser
--

COPY public.comment (id, ticket, side, quantid, oe) FROM stdin;
\.


--
-- Data for Name: exdr; Type: TABLE DATA; Schema: public; Owner: dbuser
--

COPY public.exdr (id, symbol, cash, shares, extype, ymd, outstanding, outstandend, marketcaping, marketcapend) FROM stdin;
\.


--
-- Data for Name: fixsession; Type: TABLE DATA; Schema: public; Owner: dbuser
--

COPY public.fixsession (id, beginstring, sendercompid, targetcompid, filestorepath, datadictionary, ctime, host, port) FROM stdin;
1	FIX.4.4	EXECUTOR	CLIENT1	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
2	FIX.4.4	EXECUTOR	CLIENT2	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
3	FIX.4.4	EXECUTOR	CLIENT3	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
4	FIX.4.4	EXECUTOR	CLIENT4	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
5	FIX.4.4	EXECUTOR	CLIENT5	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
6	FIX.4.4	EXECUTOR	CLIENT6	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
7	FIX.4.4	EXECUTOR	CLIENT7	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
8	FIX.4.4	EXECUTOR	CLIENT8	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
9	FIX.4.4	EXECUTOR	CLIENT9	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
10	FIX.4.4	EXECUTOR	CLIENT10	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
11	FIX.4.4	EXECUTOR	CLIENT11	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
12	FIX.4.4	EXECUTOR	CLIENT12	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
13	FIX.4.4	EXECUTOR	CLIENT13	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
14	FIX.4.4	EXECUTOR	CLIENT14	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
15	FIX.4.4	EXECUTOR	CLIENT15	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
16	FIX.4.4	EXECUTOR	CLIENT16	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
17	FIX.4.4	EXECUTOR	CLIENT17	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
18	FIX.4.4	EXECUTOR	CLIENT18	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
19	FIX.4.4	EXECUTOR	CLIENT19	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
20	FIX.4.4	EXECUTOR	CLIENT20	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
21	FIX.4.4	EXECUTOR	CLIENT21	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
22	FIX.4.4	EXECUTOR	CLIENT22	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
23	FIX.4.4	EXECUTOR	CLIENT23	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
24	FIX.4.4	EXECUTOR	CLIENT24	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
25	FIX.4.4	EXECUTOR	CLIENT25	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
26	FIX.4.4	EXECUTOR	CLIENT26	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
27	FIX.4.4	EXECUTOR	CLIENT27	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
28	FIX.4.4	EXECUTOR	CLIENT28	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
29	FIX.4.4	EXECUTOR	CLIENT29	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
30	FIX.4.4	EXECUTOR	CLIENT30	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
31	FIX.4.4	EXECUTOR	CLIENT31	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
32	FIX.4.4	EXECUTOR	CLIENT32	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
33	FIX.4.4	EXECUTOR	CLIENT33	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
34	FIX.4.4	EXECUTOR	CLIENT34	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
35	FIX.4.4	EXECUTOR	CLIENT35	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
36	FIX.4.4	EXECUTOR	CLIENT36	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
37	FIX.4.4	EXECUTOR	CLIENT37	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
38	FIX.4.4	EXECUTOR	CLIENT38	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
39	FIX.4.4	EXECUTOR	CLIENT39	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
40	FIX.4.4	EXECUTOR	CLIENT40	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
41	FIX.4.4	EXECUTOR	CLIENT41	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
42	FIX.4.4	EXECUTOR	CLIENT42	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
43	FIX.4.4	EXECUTOR	CLIENT43	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
44	FIX.4.4	EXECUTOR	CLIENT44	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
45	FIX.4.4	EXECUTOR	CLIENT45	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
46	FIX.4.4	EXECUTOR	CLIENT46	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
47	FIX.4.4	EXECUTOR	CLIENT47	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
48	FIX.4.4	EXECUTOR	CLIENT48	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
49	FIX.4.4	EXECUTOR	CLIENT49	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
50	FIX.4.4	EXECUTOR	CLIENT50	store	/opt/e2q/spec/FIX44.xml	1723003101	127.0.0.1	5001
\.


--
-- Data for Name: ohlc; Type: TABLE DATA; Schema: public; Owner: dbuser
--

COPY public.ohlc (date, open, low, high, close, volume, key, id) FROM stdin;
\.


--
-- Data for Name: profit; Type: TABLE DATA; Schema: public; Owner: dbuser
--

COPY public.profit (id, "WinRate", "BestTrade", "WorstTrade", "AvgTrade", "MaxTradeDuration", "AvgTradeDuration", "ProfitFactor", "Expectancy", "SQN", "KellyCriterion") FROM stdin;
\.


--
-- Data for Name: trade_info; Type: TABLE DATA; Schema: public; Owner: dbuser
--

COPY public.trade_info (id, version, desz, ctime, active) FROM stdin;
1	1.2.3	1.2.3	1721724103	0
2	1.2.4	1.2.4	1721898359	0
4	1.2.5	1.2.5	1722233331	0
5	1.2.6	1.2.6	1722678439	0
6	1.2.7	1.2.7	1722679135	0
7	1.2.8	1.2.8	1728452488	0
13	1.3.2	1.3.2	1744183496	0
14	1.3.3	1.3.3	1744183532	0
11	1.3.0	1.3.0	1744090538	0
12	1.3.1	1.3.1	1744093993	1
\.


--
-- Data for Name: trade_report; Type: TABLE DATA; Schema: public; Owner: dbuser
--

COPY public.trade_report (id, ticket, sessionid, balance, credit, equity, leverage, freemargin, margin, profit, ctime, side) FROM stdin;
\.


--
-- Data for Name: trades; Type: TABLE DATA; Schema: public; Owner: dbuser
--

COPY public.trades (id, symbol, ticket, stat, side, qty, price, stoppx, slippage, ordtype, cumqty, avgpx, leavesqty, openqty, closetck, ctime, quantid, otime, adjpx, amount) FROM stdin;
\.


--
-- Name: analselog_id_seq; Type: SEQUENCE SET; Schema: api; Owner: dbuser
--

SELECT pg_catalog.setval('api.analselog_id_seq', 1, false);


--
-- Name: todos_id_seq; Type: SEQUENCE SET; Schema: api; Owner: dbuser
--

SELECT pg_catalog.setval('api.todos_id_seq', 1, false);


--
-- Name: account_id_seq; Type: SEQUENCE SET; Schema: public; Owner: dbuser
--

SELECT pg_catalog.setval('public.account_id_seq', 50, true);


--
-- Name: analytics_id_seq; Type: SEQUENCE SET; Schema: public; Owner: dbuser
--

SELECT pg_catalog.setval('public.analytics_id_seq', 1, false);


--
-- Name: comment_id_seq; Type: SEQUENCE SET; Schema: public; Owner: dbuser
--

SELECT pg_catalog.setval('public.comment_id_seq', 1, false);


--
-- Name: exdr_id_seq; Type: SEQUENCE SET; Schema: public; Owner: dbuser
--

SELECT pg_catalog.setval('public.exdr_id_seq', 1, false);


--
-- Name: fixsession_id_seq; Type: SEQUENCE SET; Schema: public; Owner: dbuser
--

SELECT pg_catalog.setval('public.fixsession_id_seq', 50, true);


--
-- Name: indicator_id_seq; Type: SEQUENCE SET; Schema: public; Owner: dbuser
--

SELECT pg_catalog.setval('public.indicator_id_seq', 1, false);


--
-- Name: ohlc_id_seq; Type: SEQUENCE SET; Schema: public; Owner: dbuser
--

SELECT pg_catalog.setval('public.ohlc_id_seq', 1, false);


--
-- Name: trade_info_id_seq; Type: SEQUENCE SET; Schema: public; Owner: dbuser
--

SELECT pg_catalog.setval('public.trade_info_id_seq', 14, true);


--
-- Name: trade_report_id_seq; Type: SEQUENCE SET; Schema: public; Owner: dbuser
--

SELECT pg_catalog.setval('public.trade_report_id_seq', 1, false);


--
-- Name: trades_id_seq; Type: SEQUENCE SET; Schema: public; Owner: dbuser
--

SELECT pg_catalog.setval('public.trades_id_seq', 1, false);


--
-- Name: analselog analselog_pkey; Type: CONSTRAINT; Schema: api; Owner: dbuser
--

ALTER TABLE ONLY api.analselog
    ADD CONSTRAINT analselog_pkey PRIMARY KEY (id);


--
-- Name: stockinfo todos_pkey; Type: CONSTRAINT; Schema: api; Owner: dbuser
--

ALTER TABLE ONLY api.stockinfo
    ADD CONSTRAINT todos_pkey PRIMARY KEY (id);


--
-- Name: profit Trades_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.profit
    ADD CONSTRAINT "Trades_pkey" PRIMARY KEY (id);


--
-- Name: account account_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.account
    ADD CONSTRAINT account_pkey PRIMARY KEY (id);


--
-- Name: analytics analytics_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.analytics
    ADD CONSTRAINT analytics_pkey PRIMARY KEY (id);


--
-- Name: comment comment_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.comment
    ADD CONSTRAINT comment_pkey PRIMARY KEY (id);


--
-- Name: fixsession fixsession_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.fixsession
    ADD CONSTRAINT fixsession_pkey PRIMARY KEY (id);


--
-- Name: analse indicator_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.analse
    ADD CONSTRAINT indicator_pkey PRIMARY KEY (id);


--
-- Name: ohlc ohlc_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.ohlc
    ADD CONSTRAINT ohlc_pkey PRIMARY KEY (id);


--
-- Name: trade_info trade_info_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.trade_info
    ADD CONSTRAINT trade_info_pkey PRIMARY KEY (id);


--
-- Name: trade_report trade_report_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.trade_report
    ADD CONSTRAINT trade_report_pkey PRIMARY KEY (id);


--
-- Name: trades trades_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.trades
    ADD CONSTRAINT trades_pkey PRIMARY KEY (id);


--
-- Name: ctime_1731564753404_index; Type: INDEX; Schema: api; Owner: dbuser
--

CREATE INDEX ctime_1731564753404_index ON api.analselog USING btree (ctime);


--
-- Name: idx_1731564760831_index; Type: INDEX; Schema: api; Owner: dbuser
--

CREATE INDEX idx_1731564760831_index ON api.analselog USING btree (type);


--
-- Name: index_ohlc_on_key_and_date; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE UNIQUE INDEX index_ohlc_on_key_and_date ON public.ohlc USING btree (key, date);


--
-- Name: tckidx; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE INDEX tckidx ON public.trades USING btree (ticket);


--
-- Name: tckstatidx; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE INDEX tckstatidx ON public.trades USING btree (ticket, stat);


--
-- Name: ticket_1731489144408_index; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE UNIQUE INDEX ticket_1731489144408_index ON public.comment USING btree (ticket);


--
-- Name: account account_sessionid_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.account
    ADD CONSTRAINT account_sessionid_fkey FOREIGN KEY (sessionid) REFERENCES public.fixsession(id);


--
-- Name: SCHEMA api; Type: ACL; Schema: -; Owner: dbuser
--

GRANT USAGE ON SCHEMA api TO web_anon;


--
-- Name: TABLE analselog; Type: ACL; Schema: api; Owner: dbuser
--

GRANT SELECT ON TABLE api.analselog TO web_anon;


--
-- Name: TABLE stockinfo; Type: ACL; Schema: api; Owner: dbuser
--

GRANT SELECT ON TABLE api.stockinfo TO web_anon;


--
-- PostgreSQL database dump complete
--

