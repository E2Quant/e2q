--
-- PostgreSQL database dump
--

\restrict UeGOHlELTdQxZjXyqZla9Uylq2goncq9Ucf6DFxMDqcV4BYZMcrH10aN1Xafxy2

-- Dumped from database version 18.4 (Debian 18.4-1.pgdg13+1)
-- Dumped by pg_dump version 18.4 (Debian 18.4-1.pgdg13+1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET transaction_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: trade_status; Type: SCHEMA; Schema: -; Owner: dbuser
--

CREATE SCHEMA trade_status;


ALTER SCHEMA trade_status OWNER TO dbuser;

--
-- Name: tablefunc; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS tablefunc WITH SCHEMA public;


--
-- Name: EXTENSION tablefunc; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION tablefunc IS 'functions that manipulate whole tables, including crosstab';


--
-- Name: tabledefs; Type: TYPE; Schema: public; Owner: dbuser
--

CREATE TYPE public.tabledefs AS ENUM (
    'PKEY_INTERNAL',
    'PKEY_EXTERNAL',
    'FKEYS_INTERNAL',
    'FKEYS_EXTERNAL',
    'COMMENTS',
    'FKEYS_NONE',
    'INCLUDE_TRIGGERS',
    'NO_TRIGGERS'
);


ALTER TYPE public.tabledefs OWNER TO dbuser;

--
-- Name: indicator_adxvma(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.indicator_adxvma(_verid integer) RETURNS TABLE(value double precision, pday text, ltype integer, aname text)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
        SELECT  * FROM indicator_report_log(_verid, 60);
END; $$;


ALTER FUNCTION public.indicator_adxvma(_verid integer) OWNER TO dbuser;

--
-- Name: indicator_report_log(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.indicator_report_log(_verid integer, _type integer) RETURNS TABLE(value double precision, pday text, ltype integer, aname text)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY (
        SELECT   log."values" as value, to_timestamp(
                ((log.ctime / 1000))::double precision
            )::text as pday, log."type" as ltype, ana.name::text as aname
        FROM "analselog" log, "analse"  ana
        WHERE
            log."type" >= _type
            AND log."type" <= (_type + 1)
            AND log."quantid" = ana."quantid"
            AND ana."verid" = _verid
        ORDER BY log."ctime"  
    );
END; $$;


ALTER FUNCTION public.indicator_report_log(_verid integer, _type integer) OWNER TO dbuser;

--
-- Name: indicator_sharpe_ratio(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.indicator_sharpe_ratio(_verid integer) RETURNS TABLE(value double precision, pday text, stock text)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY (
            SELECT "values" as value, to_timestamp(
                    ((ana.ctime / 1000))::double precision
                )::text as pday,(CASE 
                    WHEN key <0 THEN  'index'
                    ELSE  st."stock"
                END)::text as stock
            FROM "analselog" ana, "stockinfo" st
            WHERE
                "type" = 9
                AND st."verid" = _verid
                AND (CASE 
                    WHEN key < 0 THEN  key = (0 - _verid)
                    ELSE  st."symbol"= key
                END)
            ORDER BY ana.ctime
    );
END; $$;


ALTER FUNCTION public.indicator_sharpe_ratio(_verid integer) OWNER TO dbuser;

--
-- Name: pg_get_coldef(text, text, text, boolean); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.pg_get_coldef(in_schema text, in_table text, in_column text, oldway boolean DEFAULT false) RETURNS text
    LANGUAGE plpgsql
    AS $$
 DECLARE
 v_coldef     text;
 v_dt1        text;
 v_dt2        text;
 v_dt3        text;
 v_nullable   boolean;
 v_position   int;
 v_identity   text;
 v_generated  text;
 v_hasdflt    boolean;
 v_dfltexpr   text;
 
 BEGIN
   IF oldway THEN
     SELECT pg_catalog.format_type(a.atttypid, a.atttypmod) INTO v_coldef FROM pg_namespace n, pg_class c, pg_attribute a, pg_type t
     WHERE n.nspname = in_schema AND n.oid = c.relnamespace AND c.relname = in_table AND a.attname = in_column and a.attnum > 0 AND a.attrelid = c.oid AND a.atttypid = t.oid ORDER BY a.attnum;
     -- RAISE NOTICE 'DEBUG: oldway=%',v_coldef;
   ELSE
 
     SELECT CASE WHEN a.atttypid = ANY ('{int,int8,int2}'::regtype[]) AND EXISTS (SELECT FROM pg_attrdef ad WHERE ad.adrelid = a.attrelid AND ad.adnum   = a.attnum AND
 	  pg_get_expr(ad.adbin, ad.adrelid) = 'nextval(''' || (pg_get_serial_sequence (a.attrelid::regclass::text, a.attname))::regclass || '''::regclass)') THEN CASE a.atttypid
 	  WHEN 'int'::regtype  THEN 'serial' WHEN 'int8'::regtype THEN 'bigserial' WHEN 'int2'::regtype THEN 'smallserial' END ELSE format_type(a.atttypid, a.atttypmod) END AS data_type
 	  INTO v_coldef FROM pg_namespace n, pg_class c, pg_attribute a, pg_type t
 	  WHERE n.nspname = in_schema AND n.oid = c.relnamespace AND c.relname = in_table AND a.attname = in_column and a.attnum > 0 AND a.attrelid = c.oid AND a.atttypid = t.oid ORDER BY a.attnum;
 
 
 
   END IF;
   RETURN v_coldef;
 END;
 $$;


ALTER FUNCTION public.pg_get_coldef(in_schema text, in_table text, in_column text, oldway boolean) OWNER TO dbuser;

--
-- Name: pg_get_tabledef(character varying, character varying, boolean, public.tabledefs[]); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.pg_get_tabledef(in_schema character varying, in_table character varying, _verbose boolean, VARIADIC arr public.tabledefs[] DEFAULT '{}'::public.tabledefs[]) RETURNS text
    LANGUAGE plpgsql
    AS $_$
   DECLARE
     v_qualified text := '';
     v_table_ddl text;
     v_table_oid int;
     v_colrec record;
     v_constraintrec record;
     v_trigrec       record;
     v_indexrec record;
     v_rec           record;
     v_constraint_name text;
     v_constraint_def  text;
     v_pkey_def        text := '';
     v_fkey_def        text := '';
     v_fkey_defs       text := '';
     v_trigger text := '';
     v_partition_key text := '';
     v_partbound text;
     v_parent text;
     v_parent_schema text;
     v_persist text;
     v_temp  text := '';
     v_temp2 text;
     v_relopts text;
     v_tablespace text;
     v_pgversion int;
     bSerial boolean;
     bPartition boolean;
     bInheritance boolean;
     bRelispartition boolean;
     constraintarr text[] := '{}';
     constraintelement text;
     bSkip boolean;
 	  bVerbose boolean := False;
 	  v_cnt1   integer;
 	  v_cnt2   integer;
 	  search_path_old text := '';
 	  search_path_new text := '';
 	  v_partial    boolean;
 	  v_pos        integer;
 
 
   	pkcnt            int := 0;
   	fkcnt            int := 0;
 	  trigcnt          int := 0;
 	  cmtcnt           int := 0;
     pktype           tabledefs := 'PKEY_INTERNAL';
     fktype           tabledefs := 'FKEYS_INTERNAL';
     trigtype         tabledefs := 'NO_TRIGGERS';
     arglen           integer;
   	vargs            text;
 	  avarg            tabledefs;
 
 
     v_ret            text;
     v_diag1          text;
     v_diag2          text;
     v_diag3          text;
     v_diag4          text;
     v_diag5          text;
     v_diag6          text;
 
   BEGIN
     SET client_min_messages = 'notice';
     IF _verbose THEN bVerbose = True; END IF;
 
 
 
     arglen := array_length($4, 1);
     IF arglen IS NULL THEN
         -- nothing to do, so assume defaults
         NULL;
     ELSE
 
         IF bVerbose THEN RAISE NOTICE 'arguments=%', $4; END IF;
         FOREACH avarg IN ARRAY $4 LOOP
             IF bVerbose THEN RAISE NOTICE 'arg=%', avarg; END IF;
             IF avarg = 'FKEYS_INTERNAL' OR avarg = 'FKEYS_EXTERNAL' OR avarg = 'FKEYS_NONE' THEN
                 fkcnt = fkcnt + 1;
                 fktype = avarg;
             ELSEIF avarg = 'INCLUDE_TRIGGERS' OR avarg = 'NO_TRIGGERS' THEN
                 trigcnt = trigcnt + 1;
                 trigtype = avarg;
             ELSEIF avarg = 'PKEY_EXTERNAL' THEN
                 pkcnt = pkcnt + 1;
                 pktype = avarg;
             ELSEIF avarg = 'COMMENTS' THEN
                 cmtcnt = cmtcnt + 1;
 
             END IF;
         END LOOP;
         IF fkcnt > 1 THEN
   	        RAISE WARNING 'Only one foreign key option can be provided. You provided %', fkcnt;
 	          RETURN '';
         ELSEIF trigcnt > 1 THEN
             RAISE WARNING 'Only one trigger option can be provided. You provided %', trigcnt;
             RETURN '';
         ELSEIF pkcnt > 1 THEN
             RAISE WARNING 'Only one pkey option can be provided. You provided %', pkcnt;
             RETURN '';
         ELSEIF cmtcnt > 1 THEN
             RAISE WARNING 'Only one comments option can be provided. You provided %', cmtcnt;
             RETURN '';
 
         END IF;
     END IF;
 
     SELECT c.oid, (select setting from pg_settings where name = 'server_version_num') INTO v_table_oid, v_pgversion FROM pg_catalog.pg_class c LEFT JOIN pg_catalog.pg_namespace n ON n.oid = c.relnamespace
     WHERE c.relkind in ('r','p') AND c.relname = in_table AND n.nspname = in_schema;
 
     SELECT setting INTO search_path_old FROM pg_settings WHERE name = 'search_path';
 
     SELECT REPLACE(REPLACE(setting, '"$user"', '$user'), '$user', '"$user"') INTO search_path_old
     FROM pg_settings
     WHERE name = 'search_path';
 
     EXECUTE 'SET search_path = "public"';
     SELECT setting INTO search_path_new FROM pg_settings WHERE name = 'search_path';
 
     IF (v_table_oid IS NULL) THEN
       RAISE EXCEPTION 'table does not exist';
     END IF;
 
 
     SELECT tablespace INTO v_temp FROM pg_tables WHERE schemaname = in_schema and tablename = in_table and tablespace IS NOT NULL;
     IF v_temp IS NULL THEN
       v_tablespace := 'TABLESPACE pg_default';
     ELSE
       v_tablespace := 'TABLESPACE ' || v_temp;
     END IF;
 
 
     WITH relopts AS (SELECT unnest(c.reloptions) relopts FROM pg_class c, pg_namespace n WHERE n.nspname = in_schema and n.oid = c.relnamespace and c.relname = in_table)
     SELECT string_agg(r.relopts, ', ') as relopts INTO v_temp from relopts r;
     IF v_temp IS NULL THEN
       v_relopts := '';
     ELSE
       v_relopts := ' WITH (' || v_temp || ')';
     END IF;
 
 
     v_partbound := '';
     bPartition := False;
     bInheritance := False;
     IF v_pgversion < 100000 THEN
 
       SELECT c2.relname parent, c2.relnamespace::regnamespace INTO v_parent, v_parent_schema from pg_class c1, pg_namespace n, pg_inherits i, pg_class c2
       WHERE n.nspname = in_schema and n.oid = c1.relnamespace and c1.relname = in_table and c1.oid = i.inhrelid and i.inhparent = c2.oid and c1.relkind = 'r';
       IF (v_parent IS NOT NULL) THEN
         bPartition   := True;
         bInheritance := True;
       END IF;
     ELSE
 
       SELECT c2.relname parent, c1.relispartition, pg_get_expr(c1.relpartbound, c1.oid, true), c2.relnamespace::regnamespace INTO v_parent, bRelispartition, v_partbound, v_parent_schema from pg_class c1, pg_namespace n, pg_inherits i, pg_class c2
       WHERE n.nspname = in_schema and n.oid = c1.relnamespace and c1.relname = in_table and c1.oid = i.inhrelid and i.inhparent = c2.oid and c1.relkind = 'r';
       IF (v_parent IS NOT NULL) THEN
         bPartition   := True;
         IF bRelispartition THEN
           bInheritance := False;
         ELSE
           bInheritance := True;
         END IF;
       END IF;
     END IF;
     IF bPartition THEN
 
 		  SELECT count(*) INTO v_cnt1 FROM information_schema.tables t WHERE EXISTS (SELECT REGEXP_MATCHES(s.table_name, '([A-Z]+)','g') FROM information_schema.tables s
 		  WHERE t.table_schema=s.table_schema AND t.table_name=s.table_name AND t.table_schema = in_schema AND t.table_name = in_table AND t.table_type = 'BASE TABLE');
 
 
       SELECT COUNT(*) INTO v_cnt2 FROM pg_get_keywords() WHERE word = in_table AND catcode = 'R';
 
       IF bInheritance THEN
         IF v_cnt1 > 0 OR v_cnt2 > 0 THEN
           v_table_ddl := 'CREATE TABLE ' || in_schema || '."' || in_table || '"( '|| E'\n';
         ELSE
           v_table_ddl := 'CREATE TABLE ' || in_schema || '.' || in_table || '( '|| E'\n';
         END IF;
 
 
       ELSE
         IF v_relopts <> '' THEN
           IF v_cnt1 > 0 OR v_cnt2 > 0 THEN
             v_table_ddl := 'CREATE TABLE ' || in_schema || '."' || in_table || '" PARTITION OF ' || in_schema || '.' || v_parent || ' ' || v_partbound || v_relopts || ' ' || v_tablespace || '; ' || E'\n';
 				  ELSE
 				    v_table_ddl := 'CREATE TABLE ' || in_schema || '.' || in_table || ' PARTITION OF ' || in_schema || '.' || v_parent || ' ' || v_partbound || v_relopts || ' ' || v_tablespace || '; ' || E'\n';
 				  END IF;
         ELSE
           IF v_cnt1 > 0 OR v_cnt2 > 0 THEN
             v_table_ddl := 'CREATE TABLE ' || in_schema || '."' || in_table || '" PARTITION OF ' || in_schema || '.' || v_parent || ' ' || v_partbound || ' ' || v_tablespace || '; ' || E'\n';
 				  ELSE
 				    v_table_ddl := 'CREATE TABLE ' || in_schema || '.' || in_table || ' PARTITION OF ' || in_schema || '.' || v_parent || ' ' || v_partbound || ' ' || v_tablespace || '; ' || E'\n';
 				  END IF;
         END IF;
 
       END IF;
     END IF;
 	  IF bVerbose THEN RAISE NOTICE '(1)tabledef so far: %', v_table_ddl; END IF;
 
     IF NOT bPartition THEN
 
       select c.relpersistence into v_persist from pg_class c, pg_namespace n where n.nspname = in_schema and n.oid = c.relnamespace and c.relname = in_table and c.relkind = 'r';
       IF v_persist = 'u' THEN
         v_temp := 'UNLOGGED';
       ELSIF v_persist = 't' THEN
         v_temp := 'TEMPORARY';
       ELSE
         v_temp := '';
       END IF;
     END IF;
 
 
     IF NOT bPartition THEN
 
       SELECT count(*) INTO v_cnt1 FROM information_schema.tables t WHERE EXISTS (SELECT REGEXP_MATCHES(s.table_name, '([A-Z]+)','g') FROM information_schema.tables s
       WHERE t.table_schema=s.table_schema AND t.table_name=s.table_name AND t.table_schema = in_schema AND t.table_name = in_table AND t.table_type = 'BASE TABLE');
       IF v_cnt1 > 0 THEN
         v_table_ddl := 'CREATE ' || v_temp || ' TABLE ' || in_schema || '."' || in_table || '" (' || E'\n';
       ELSE
         v_table_ddl := 'CREATE ' || v_temp || ' TABLE ' || in_schema || '.' || in_table || ' (' || E'\n';
       END IF;
     END IF;
 
     IF NOT bPartition THEN
       FOR v_colrec IN
         SELECT c.column_name, c.data_type, c.udt_name, c.udt_schema, c.character_maximum_length, c.is_nullable, c.column_default, c.numeric_precision, c.numeric_scale, c.is_identity, c.identity_generation, c.is_generated, c.generation_expression
         FROM information_schema.columns c WHERE (table_schema, table_name) = (in_schema, in_table) ORDER BY ordinal_position
       LOOP
          IF bVerbose THEN RAISE NOTICE '(col loop) name=%  type=%  udt_name=%  default=%  is_generated=%  gen_expr=%', v_colrec.column_name, v_colrec.data_type, v_colrec.udt_name, v_colrec.column_default, v_colrec.is_generated, v_colrec.generation_expression; END IF;
 
          SELECT CASE WHEN pg_get_serial_sequence(quote_ident(in_schema) || '.' || quote_ident(in_table), v_colrec.column_name) IS NOT NULL THEN True ELSE False END into bSerial;
          IF bVerbose THEN
 
            SELECT pg_get_serial_sequence(quote_ident(in_schema) || '.' || quote_ident(in_table), v_colrec.column_name) into v_temp;
            IF v_temp IS NULL THEN v_temp = 'NA'; END IF;
            SELECT pg_get_coldef(in_schema, in_table,v_colrec.column_name) INTO v_diag1;
            RAISE NOTICE 'DEBUG table: %  Column: %  datatype: %  Serial=%  serialval=%  coldef=%', v_qualified, v_colrec.column_name, v_colrec.data_type, bSerial, v_temp, v_diag1;
            RAISE NOTICE 'DEBUG tabledef: %', v_table_ddl;
          END IF;
 
 
          SELECT COUNT(*) INTO v_cnt1 FROM information_schema.columns t WHERE EXISTS (SELECT REGEXP_MATCHES(s.column_name, '([A-Z]+)','g') FROM information_schema.columns s
          WHERE t.table_schema=s.table_schema and t.table_name=s.table_name and t.column_name=s.column_name AND t.table_schema = quote_ident(in_schema) AND column_name = v_colrec.column_name);
 
          SELECT COUNT(*) INTO v_cnt2 FROM pg_get_keywords() WHERE word = v_colrec.column_name AND catcode = 'R';
 
          IF v_cnt1 > 0 OR v_cnt2 > 0 THEN
            v_table_ddl := v_table_ddl || '  "' || v_colrec.column_name || '" ';
          ELSE
            v_table_ddl := v_table_ddl || '  ' || v_colrec.column_name || ' ';
          END IF;
 
          IF v_colrec.is_generated = 'ALWAYS' and v_colrec.generation_expression IS NOT NULL THEN
 
              v_temp = v_colrec.data_type || ' GENERATED ALWAYS AS (' || v_colrec.generation_expression || ') STORED ';
          ELSEIF v_colrec.udt_name in ('geometry', 'box2d', 'box2df', 'box3d', 'geography', 'geometry_dump', 'gidx', 'spheroid', 'valid_detail') THEN
 		         v_temp = v_colrec.udt_name;
 		     ELSEIF v_colrec.data_type = 'USER-DEFINED' THEN
 		         v_temp = v_colrec.udt_schema || '.' || v_colrec.udt_name;
 		     ELSEIF v_colrec.data_type = 'ARRAY' THEN
 
 		         v_temp = pg_get_coldef(in_schema, in_table,v_colrec.column_name);
 
 		     ELSEIF pg_get_serial_sequence(quote_ident(in_schema) || '.' || quote_ident(in_table), v_colrec.column_name) IS NOT NULL THEN
 		         -- Issue#8 fix: handle serial. Note: NOT NULL is implied so no need to declare it explicitly
 		         v_temp = pg_get_coldef(in_schema, in_table,v_colrec.column_name);
 		     ELSE
 		         v_temp = v_colrec.data_type;
          END IF;
 
 
 
 		     IF v_colrec.is_identity = 'YES' THEN
 		         IF v_colrec.identity_generation = 'ALWAYS' THEN
 		             v_temp = v_temp || ' GENERATED ALWAYS AS IDENTITY NOT NULL';
 		         ELSE
 		             v_temp = v_temp || ' GENERATED BY DEFAULT AS IDENTITY NOT NULL';
 		         END IF;
          ELSEIF v_colrec.character_maximum_length IS NOT NULL THEN
              v_temp = v_temp || ('(' || v_colrec.character_maximum_length || ')');
          ELSEIF v_colrec.numeric_precision > 0 AND v_colrec.numeric_scale > 0 THEN
              v_temp = v_temp || '(' || v_colrec.numeric_precision || ',' || v_colrec.numeric_scale || ')';
          END IF;
 
 
          IF bSerial THEN
              v_temp = v_temp || ' NOT NULL';
          ELSEIF v_colrec.is_nullable = 'NO' THEN
              v_temp = v_temp || ' NOT NULL';
          ELSEIF v_colrec.is_nullable = 'YES' THEN
              v_temp = v_temp || ' NULL';
          END IF;
 
 
          IF v_colrec.column_default IS NOT null AND NOT bSerial THEN
 
              v_temp = v_temp || (' DEFAULT ' || v_colrec.column_default);
          END IF;
          v_temp = v_temp || ',' || E'\n';
 
          v_table_ddl := v_table_ddl || v_temp;
 
 
       END LOOP;
     END IF;
     IF bVerbose THEN RAISE NOTICE '(2)tabledef so far: %', v_table_ddl; END IF;
 
     IF v_pgversion < 110000 THEN
       FOR v_constraintrec IN
         SELECT con.conname as constraint_name, con.contype as constraint_type,
           CASE
             WHEN con.contype = 'p' THEN 1 -- primary key constraint
             WHEN con.contype = 'u' THEN 2 -- unique constraint
             WHEN con.contype = 'f' THEN 3 -- foreign key constraint
             WHEN con.contype = 'c' THEN 4
             ELSE 5
           END as type_rank,
           pg_get_constraintdef(con.oid) as constraint_definition
         FROM pg_catalog.pg_constraint con JOIN pg_catalog.pg_class rel ON rel.oid = con.conrelid JOIN pg_catalog.pg_namespace nsp ON nsp.oid = connamespace
         WHERE nsp.nspname = in_schema AND rel.relname = in_table ORDER BY type_rank
       LOOP
         v_constraint_name := v_constraintrec.constraint_name;
         v_constraint_def  := v_constraintrec.constraint_definition;
         IF v_constraintrec.type_rank = 1 THEN
             IF pkcnt = 0 OR pktype = 'PKEY_INTERNAL' THEN
 
                 v_constraint_name := v_constraintrec.constraint_name;
                 v_constraint_def  := v_constraintrec.constraint_definition;
                 v_table_ddl := v_table_ddl || '  ' -- note: two char spacer to start, to indent the column
                   || 'CONSTRAINT' || ' '
                   || v_constraint_name || ' '
                   || v_constraint_def
                   || ',' || E'\n';
             ELSE
 
               SELECT 'ALTER TABLE ONLY ' || in_schema || '.' || c.relname || ' ADD CONSTRAINT ' || r.conname || ' ' || pg_catalog.pg_get_constraintdef(r.oid, true) || ';' INTO v_pkey_def
               FROM pg_catalog.pg_constraint r, pg_class c, pg_namespace n where r.conrelid = c.oid and  r.contype = 'p' and n.oid = r.connamespace and n.nspname = in_schema AND c.relname = in_table and r.conname = v_constraint_name;
             END IF;
             IF bPartition THEN
               continue;
             END IF;
         ELSIF v_constraintrec.type_rank = 3 THEN
 
             IF fktype = 'FKEYS_NONE' THEN
 
                 continue;
             ELSIF fkcnt = 0 OR fktype = 'FKEYS_INTERNAL' THEN
 
                 v_table_ddl := v_table_ddl || '  ' -- note: two char spacer to start, to indent the column
                   || 'CONSTRAINT' || ' '
                   || v_constraint_name || ' '
                   || v_constraint_def
                   || ',' || E'\n';
             ELSE
 
                 SELECT 'ALTER TABLE ONLY ' || n.nspname || '.' || c2.relname || ' ADD CONSTRAINT ' || r.conname || ' ' || pg_catalog.pg_get_constraintdef(r.oid, true) || ';' INTO v_fkey_def
   			        FROM pg_constraint r, pg_class c1, pg_namespace n, pg_class c2 where r.conrelid = c1.oid and  r.contype = 'f' and n.nspname = in_schema and n.oid = r.connamespace and r.conrelid = c2.oid and c2.relname = in_table;
                 v_fkey_defs = v_fkey_defs || v_fkey_def || E'\n';
             END IF;
         ELSE
 
             v_table_ddl := v_table_ddl || '  ' -- note: two char spacer to start, to indent the column
               || 'CONSTRAINT' || ' '
               || v_constraint_name || ' '
               || v_constraint_def
               || ',' || E'\n';
         END IF;
         if bVerbose THEN RAISE NOTICE 'DEBUG4: constraint name=% constraint_def=%', v_constraint_name,v_constraint_def; END IF;
         constraintarr := constraintarr || v_constraintrec.constraint_name:: text;
 
       END LOOP;
     ELSE
       FOR v_constraintrec IN
         SELECT con.conname as constraint_name, con.contype as constraint_type,
           CASE
             WHEN con.contype = 'p' THEN 1 -- primary key constraint
             WHEN con.contype = 'u' THEN 2 -- unique constraint
             WHEN con.contype = 'f' THEN 3 -- foreign key constraint
             WHEN con.contype = 'c' THEN 4
             ELSE 5
           END as type_rank,
           pg_get_constraintdef(con.oid) as constraint_definition
         FROM pg_catalog.pg_constraint con JOIN pg_catalog.pg_class rel ON rel.oid = con.conrelid JOIN pg_catalog.pg_namespace nsp ON nsp.oid = connamespace
         WHERE nsp.nspname = in_schema AND rel.relname = in_table
               --Issue#13 added this condition:
               AND con.conparentid = 0
               ORDER BY type_rank
       LOOP
         v_constraint_name := v_constraintrec.constraint_name;
         v_constraint_def  := v_constraintrec.constraint_definition;
         IF v_constraintrec.type_rank = 1 THEN
             IF pkcnt = 0 OR pktype = 'PKEY_INTERNAL' THEN
                 -- internal def
                 v_constraint_name := v_constraintrec.constraint_name;
                 v_constraint_def  := v_constraintrec.constraint_definition;
                 v_table_ddl := v_table_ddl || '  ' -- note: two char spacer to start, to indent the column
                   || 'CONSTRAINT' || ' '
                   || v_constraint_name || ' '
                   || v_constraint_def
                   || ',' || E'\n';
             ELSE
               SELECT 'ALTER TABLE ONLY ' || in_schema || '.' || c.relname || ' ADD CONSTRAINT ' || r.conname || ' ' || pg_catalog.pg_get_constraintdef(r.oid, true) || ';' INTO v_pkey_def
               FROM pg_catalog.pg_constraint r, pg_class c, pg_namespace n where r.conrelid = c.oid and  r.contype = 'p' and n.oid = r.connamespace and n.nspname = in_schema AND c.relname = in_table;
             END IF;
             IF bPartition THEN
               continue;
             END IF;
         ELSIF v_constraintrec.type_rank = 3 THEN
 
             IF fktype = 'FKEYS_NONE' THEN
                 -- skip
                 continue;
             ELSIF fkcnt = 0 OR fktype = 'FKEYS_INTERNAL' THEN
                 -- internal def
                 v_table_ddl := v_table_ddl || '  ' -- note: two char spacer to start, to indent the column
                   || 'CONSTRAINT' || ' '
                   || v_constraint_name || ' '
                   || v_constraint_def
                   || ',' || E'\n';
             ELSE
 
                 SELECT 'ALTER TABLE ONLY ' || n.nspname || '.' || c2.relname || ' ADD CONSTRAINT ' || r.conname || ' ' || pg_catalog.pg_get_constraintdef(r.oid, true) || ';' INTO v_fkey_def
   			        FROM pg_constraint r, pg_class c1, pg_namespace n, pg_class c2 where r.conrelid = c1.oid and  r.contype = 'f' and n.nspname = in_schema and n.oid = r.connamespace and r.conrelid = c2.oid and c2.relname = in_table and
   			        r.conname = v_constraint_name and r.conparentid = 0;
                 v_fkey_defs = v_fkey_defs || v_fkey_def || E'\n';
             END IF;
         ELSE
 
             v_table_ddl := v_table_ddl || '  ' -- note: two char spacer to start, to indent the column
               || 'CONSTRAINT' || ' '
               || v_constraint_name || ' '
               || v_constraint_def
               || ',' || E'\n';
         END IF;
         if bVerbose THEN RAISE NOTICE 'DEBUG4: constraint name=% constraint_def=%', v_constraint_name,v_constraint_def; END IF;
         constraintarr := constraintarr || v_constraintrec.constraint_name:: text;
 
        END LOOP;
     END IF;
 
 
     select substring(v_table_ddl, length(v_table_ddl) - 1, 1) INTO v_temp;
     IF v_temp = ',' THEN
         v_table_ddl = substr(v_table_ddl, 0, length(v_table_ddl) - 1) || E'\n';
     END IF;
     IF bVerbose THEN RAISE NOTICE '(3)tabledef so far: %', trim(v_table_ddl); END IF;
 
 
     IF bVerbose THEN RAISE NOTICE '(4)tabledef so far: %', v_table_ddl; END IF;
 
 
     IF bPartition and bInheritance THEN
 
       IF v_parent_schema = '' OR v_parent_schema IS NULL THEN v_parent_schema = in_schema; END IF;
       v_table_ddl := v_table_ddl || ') INHERITS (' || v_parent_schema || '.' || v_parent || ') ' || E'\n' || v_relopts || ' ' || v_tablespace || ';' || E'\n';
     END IF;
 
     IF v_pgversion >= 100000 AND NOT bPartition and NOT bInheritance THEN
       SELECT pg_get_partkeydef(c1.oid) as partition_key INTO v_partition_key FROM pg_class c1 JOIN pg_namespace n ON (n.oid = c1.relnamespace) LEFT JOIN pg_partitioned_table p ON (c1.oid = p.partrelid)
       WHERE n.nspname = in_schema and n.oid = c1.relnamespace and c1.relname = in_table and c1.relkind = 'p';
 
       IF v_partition_key IS NOT NULL AND v_partition_key <> '' THEN
         v_table_ddl := v_table_ddl || ') PARTITION BY ' || v_partition_key || ';' || E'\n';
       ELSEIF v_relopts <> '' THEN
         v_table_ddl := v_table_ddl || ') ' || v_relopts || ' ' || v_tablespace || ';' || E'\n';
       ELSE
 
         v_table_ddl := v_table_ddl || ') ' || v_tablespace || ';' || E'\n';
       END IF;
     END IF;
 
     IF bVerbose THEN RAISE NOTICE '(5)tabledef so far: %', v_table_ddl; END IF;
 
     IF v_pkey_def <> '' THEN
         v_table_ddl := v_table_ddl || v_pkey_def || E'\n';
     END IF;
 
 
     IF v_fkey_defs <> '' THEN
 	         v_table_ddl := v_table_ddl || v_fkey_defs || E'\n';
     END IF;
 
     IF bVerbose THEN RAISE NOTICE '(6)tabledef so far: %', v_table_ddl; END IF;
 
     FOR v_indexrec IN
       SELECT indexdef, COALESCE(tablespace, 'pg_default') as tablespace, indexname FROM pg_indexes WHERE (schemaname, tablename) = (in_schema, in_table)
     LOOP
 
       bSkip = False;
       FOREACH constraintelement IN ARRAY constraintarr
       LOOP
          IF constraintelement = v_indexrec.indexname THEN
              -- RAISE NOTICE 'DEBUG7: skipping index, %', v_indexrec.indexname;
              bSkip = True;
              EXIT;
          END IF;
       END LOOP;
       if bSkip THEN CONTINUE; END IF;
 
       v_indexrec.indexdef := REPLACE(v_indexrec.indexdef, 'CREATE INDEX', 'CREATE INDEX IF NOT EXISTS');
       v_indexrec.indexdef := REPLACE(v_indexrec.indexdef, 'CREATE UNIQUE INDEX', 'CREATE UNIQUE INDEX IF NOT EXISTS');
       IF v_partition_key IS NOT NULL AND v_partition_key <> '' THEN
           v_table_ddl := v_table_ddl || v_indexrec.indexdef || ';' || E'\n';
       ELSE
 					select CASE WHEN i.indpred IS NOT NULL THEN True ELSE False END INTO v_partial
 					FROM pg_index i JOIN pg_class c1 ON (i.indexrelid = c1.oid) JOIN pg_class c2 ON (i.indrelid = c2.oid)
 					WHERE c1.relnamespace::regnamespace::text = in_schema AND c2.relnamespace::regnamespace::text = in_schema AND c2.relname = in_table AND c1.relname = v_indexrec.indexname;
           IF v_partial THEN
               -- Put tablespace def before WHERE CLAUSE
               v_temp = v_indexrec.indexdef;
               v_pos = POSITION(' WHERE ' IN v_temp);
               v_temp2 = SUBSTRING(v_temp, v_pos);
               v_temp  = SUBSTRING(v_temp, 1, v_pos);
               v_table_ddl := v_table_ddl || v_temp || ' TABLESPACE ' || v_indexrec.tablespace || v_temp2 || ';' || E'\n';
           ELSE
               v_table_ddl := v_table_ddl || v_indexrec.indexdef || ' TABLESPACE ' || v_indexrec.tablespace || ';' || E'\n';
           END IF;
       END IF;
 
     END LOOP;
     IF bVerbose THEN RAISE NOTICE '(7)tabledef so far: %', v_table_ddl; END IF;
 
     -- Issue#20: added logic for table and column comments
     IF  cmtcnt > 0 THEN
         FOR v_rec IN
           SELECT c.relname, 'COMMENT ON ' || CASE WHEN c.relkind in ('r','p') AND a.attname IS NULL THEN 'TABLE ' WHEN c.relkind in ('r','p') AND a.attname IS NOT NULL THEN 'COLUMN ' WHEN c.relkind = 'f' THEN 'FOREIGN TABLE '
                  WHEN c.relkind = 'm' THEN 'MATERIALIZED VIEW ' WHEN c.relkind = 'v' THEN 'VIEW ' WHEN c.relkind = 'i' THEN 'INDEX ' WHEN c.relkind = 'S' THEN 'SEQUENCE ' ELSE 'XX' END || n.nspname || '.' ||
                  CASE WHEN c.relkind in ('r','p') AND a.attname IS NOT NULL THEN quote_ident(c.relname) || '.' || a.attname ELSE quote_ident(c.relname) END || ' IS '   || quote_literal(d.description) || ';' as ddl
 	   	    FROM pg_class c JOIN pg_namespace n ON (n.oid = c.relnamespace) LEFT JOIN pg_description d ON (c.oid = d.objoid) LEFT JOIN pg_attribute a ON (c.oid = a.attrelid AND a.attnum > 0 and a.attnum = d.objsubid)
 	   	    WHERE d.description IS NOT NULL AND n.nspname = in_schema AND c.relname = in_table ORDER BY 2 desc, ddl
         LOOP
             --RAISE NOTICE 'comments:%', v_rec.ddl;
             v_table_ddl = v_table_ddl || v_rec.ddl || E'\n';
         END LOOP;
     END IF;
     IF bVerbose THEN RAISE NOTICE '(8)tabledef so far: %', v_table_ddl; END IF;
 
     IF trigtype = 'INCLUDE_TRIGGERS' THEN
 	    -- Issue#14: handle multiple triggers for a table
       FOR v_trigrec IN
           select pg_get_triggerdef(t.oid, True) || ';' as triggerdef FROM pg_trigger t, pg_class c, pg_namespace n
           WHERE n.nspname = in_schema and n.oid = c.relnamespace and c.relname = in_table and c.relkind = 'r' and t.tgrelid = c.oid and NOT t.tgisinternal
       LOOP
           v_table_ddl := v_table_ddl || v_trigrec.triggerdef;
           v_table_ddl := v_table_ddl || E'\n';
           IF bVerbose THEN RAISE NOTICE 'triggerdef = %', v_trigrec.triggerdef; END IF;
       END LOOP;
     END IF;
 
     IF bVerbose THEN RAISE NOTICE '(9)tabledef so far: %', v_table_ddl; END IF;
     v_table_ddl := v_table_ddl || E'\n';
     IF bVerbose THEN RAISE NOTICE '(10)tabledef so far: %', v_table_ddl; END IF;
     IF search_path_old = '' THEN
       SELECT set_config('search_path', '', false) into v_temp;
     ELSE
       EXECUTE 'SET search_path = ' || search_path_old;
     END IF;
 
     RETURN v_table_ddl;
 
     EXCEPTION
     WHEN others THEN
     BEGIN
       GET STACKED DIAGNOSTICS v_diag1 = MESSAGE_TEXT, v_diag2 = PG_EXCEPTION_DETAIL, v_diag3 = PG_EXCEPTION_HINT, v_diag4 = RETURNED_SQLSTATE, v_diag5 = PG_CONTEXT, v_diag6 = PG_EXCEPTION_CONTEXT;
       v_ret := 'line=' || v_diag6 || '. '|| v_diag4 || '. ' || v_diag1;
       RAISE EXCEPTION '%', v_ret;
        RETURN '';
     END;
 
   END;
 $_$;


ALTER FUNCTION public.pg_get_tabledef(in_schema character varying, in_table character varying, _verbose boolean, VARIADIC arr public.tabledefs[]) OWNER TO dbuser;

--
-- Name: quant_account(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_account(_verid integer) RETURNS TABLE(targetcompid text, sessionid integer, balance double precision, margin double precision, init_cash double precision, end_cash double precision, mode text)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    (
        SELECT
            f."targetcompid"::text,
            a."sessionid",
            a."balance",
            a."margin",
            COALESCE(  (
                SELECT t_2."credit"
                FROM "trade_report" t_2
                WHERE
                    t_2."side" != 4
                    AND
                    t_2.id in (
                        SELECT data.rid
                        FROM (
                                SELECT DISTINCT
                                    ON (t_1.sessionid) t_1.sessionid, min(t_1.id) AS rid
                                FROM trade_report t_1
                                WHERE t_1."sessionid" = f.id AND "t_1"."side" != 4
                                GROUP BY
                                    t_1.sessionid
                            ) data
                    ) LIMIT 1
            ), (a."balance" + a."margin")) as init_cash,
            (a."balance" + a."margin") as end_cash,
            (
                SELECT ana."name"::text
                from
                    "trade_report" tr,
                    trades t,
                    analse ana
                WHERE
                    tr."sessionid" = f.id
                    AND tr."side" != 4
                    AND t.id = tr."ticket"
                    AND ana."quantid" = t."quantid"
                LIMIT 1
            )::text as "mode"
        FROM
            "account" a,
            fixsession f
        WHERE
            a."sessionid" = f.id
            AND a."verid" = _verid
           
    );
END; $$;


ALTER FUNCTION public.quant_account(_verid integer) OWNER TO dbuser;

--
-- Name: quant_account_credit(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_account_credit(_verid integer) RETURNS TABLE(credit double precision, targetcompid text, pday text)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    (
        SELECT DISTINCT
            ON (fix."targetcompid", tr."ctime") tr."credit",
            fix."targetcompid"::text targetcompid,
            to_timestamp(((tr.ctime / 1000))::double precision)::text AS pday
            FROM
            "trade_report" tr,
            "fixsession" fix
        WHERE
            fix.id = tr."sessionid"
            AND "fix".id IN (
                SELECT
                "sessionid"
                FROM
                "account"
                WHERE
                "verid" = _verid
            )
            AND tr."side"!=4
        ORDER BY
            tr."ctime"
    );
END; $$;


ALTER FUNCTION public.quant_account_credit(_verid integer) OWNER TO dbuser;

--
-- Name: quant_account_credit_bands(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_account_credit_bands(_verid integer) RETURNS TABLE(btargetcompid text, bmax_credit double precision, bmin_credit double precision, binit_credit double precision, bmpday text)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY (
        SELECT
            data.targetcompid,
            data.max_credit,
            data.min_credit,
            (
                SELECT
                credit
                FROM
                quant_account_credit (_verid)
                WHERE
                pday = mpday
                AND targetcompid = data.targetcompid
                LIMIT
                1
            ) as init_credit,
            data.mpday
        FROM
        (
            SELECT
            targetcompid,
            max(credit) as max_credit,
            min(credit) as min_credit,
            min(pday) as mpday
            FROM
            quant_account_credit (_verid)
            GROUP BY
            targetcompid
        ) data    
    );
END; $$;


ALTER FUNCTION public.quant_account_credit_bands(_verid integer) OWNER TO dbuser;

--
-- Name: quant_account_credit_day(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_account_credit_day(_verid integer) RETURNS TABLE(dcredit double precision, dtargetcompid text, dpday text, dreturn_value double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    (
    SELECT
        credit,
        targetcompid,
        pday,
        COALESCE(
            (
            (
                credit - lag(credit, 1) OVER (
                partition by
                    targetcompid
                ORDER BY
                    pday
                )
            ) / lag(credit, 1) OVER (
                partition by
                targetcompid
                ORDER BY
                pday
            ) * 100
            ),
            0
        ) as return_value
    FROM
        quant_account_credit (_verid)
    ORDER BY
        pday
    );
END; $$;


ALTER FUNCTION public.quant_account_credit_day(_verid integer) OWNER TO dbuser;

--
-- Name: quant_account_credit_month(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_account_credit_month(_verid integer) RETURNS TABLE(dcredit double precision, dtargetcompid text, dpday text, dreturn_value double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    (
    SELECT
        credit,
        targetcompid,
        pday,
        COALESCE(
            (
            (
                credit - lag(credit, 1) OVER (
                partition by
                    targetcompid
                ORDER BY
                    pday
                )
            ) / lag(credit, 1) OVER (
                partition by
                targetcompid
                ORDER BY
                pday
            ) * 100
            ),
            0
        ) as return_value
        FROM
        (
            SELECT DISTINCT
            ON (fix."targetcompid", tr."ctime") tr."credit",
            fix."targetcompid"::text targetcompid,
            tr.ctime AS pday
            FROM
            (
                SELECT
                to_char(
                    to_timestamp(((ctime / 1000))::double precision),
                    'YYYY-MM'
                ) as ctime,
                credit,
                sessionid
                FROM
                trade_report WHERE "side"!= 4
            ) tr,
            "fixsession" fix
            WHERE
            fix.id = tr."sessionid"
            AND "fix".id IN (
                SELECT
                "sessionid"
                FROM
                "account"
                WHERE
                "verid" = _verid
            )
            ORDER BY
            tr."ctime"
        ) data
    );
END; $$;


ALTER FUNCTION public.quant_account_credit_month(_verid integer) OWNER TO dbuser;

--
-- Name: quant_account_credit_sum(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_account_credit_sum(_verid integer) RETURNS TABLE(dcredit double precision, dtargetcompid text, dpday text, dreturn_value double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    (
    SELECT
        credit,
        targetcompid,
        pday,
        (
            (
            "credit" - first_value("credit") OVER (
            partition by targetcompid ORDER BY
                pday
            )
            ) / first_value("credit") OVER (
            partition by targetcompid ORDER BY
                pday
            ) * 100
        ) as return_value
    FROM
        quant_account_credit (_verid)
    ORDER BY
        pday
    );
END; $$;


ALTER FUNCTION public.quant_account_credit_sum(_verid integer) OWNER TO dbuser;

--
-- Name: quant_bands(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_bands(_verid integer) RETURNS TABLE(idxs bigint, quantid bigint, name text, argv text, init_cash double precision, min_pro double precision, max_pro double precision, min_diff double precision, max_diff double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    (
        SELECT
        data.idx::bigint as idxs,
        data.quantid,
        data.name::text,
        data.argv::text,
        data.init_cash,
        data.min_pro,
        data.max_pro,
        (
            CASE
                WHEN data.min_pro = 0 THEN 0
                ELSE (data.min_pro - data.init_cash) / data.init_cash *100
            END
        ) as min_diff,
        (
            CASE
                WHEN data.max_pro = 0 THEN 0
                ELSE (data.max_pro - data.init_cash) / data.init_cash *100
            END
        ) as max_diff
    FROM (
            SELECT *, COALESCE(
                    (
                        SELECT vprofit_sum
                        from quant_profit_one_verid (_verid, idx::INT - 1)
                        ORDER BY vprofit_sum
                        LIMIT 1
                    ), 0
                ) min_pro, COALESCE(
                    (
                        SELECT vprofit_sum
                        from quant_profit_one_verid (_verid, idx::INT - 1)
                        ORDER BY vprofit_sum DESC
                        LIMIT 1
                    ), 0
                ) max_pro
            FROM (
                    SELECT ROW_NUMBER() OVER (
                            ORDER BY id
                        ) AS idx, ana."quantid", ana."name", ana.argv, ana.init_cash
                    from "public"."analse" ana
                    WHERE
                        ana."verid" = _verid
                    ORDER BY id
                ) dd
        ) data

    );
END; $$;


ALTER FUNCTION public.quant_bands(_verid integer) OWNER TO dbuser;

--
-- Name: quant_order_day(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_order_day(_verid integer) RETURNS TABLE(rquantid bigint, order_stock text, long_amount double precision, stop_amount double precision, order_day integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    SELECT
        "quantid",
        "stock"::text as order_stock,
        (qty * "buy_price") as long_amount,
        (
            CASE 
                WHEN splits>0 THEN  (qty * "stop_price" * splits)
                ELSE  (qty * "stop_price")
            END
        )
         as stop_amount,
        EXTRACT(
            DAY
            FROM ("stop_time" - "buy_time")
        )::INTEGER as order_day
    FROM "e2q_history"
    WHERE
        "verid" = _verid;
END; $$;


ALTER FUNCTION public.quant_order_day(_verid integer) OWNER TO dbuser;

--
-- Name: quant_order_day_count(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_order_day_count(_verid integer) RETURNS TABLE(profits double precision, time_long text)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    
SELECT ((profit.samounts - profit.lamounts)/profit.lamounts*100) as profits, profit.name  as "time_long" FROM (
SELECT sum(lamounts) as lamounts, sum(samounts) as samounts  , name
FROM (
        SELECT
            count(order_day) as number, sum(long_amount) as lamounts, sum(stop_amount) as samounts, '1-5' as "name"
        from "quant_order_day" (_verid)
        WHERE
            order_day < 5
        GROUP BY
            rquantid
        UNION
        SELECT
            count(order_day) as number, sum(long_amount) as lamounts, sum(stop_amount) as samounts, '5-20' as "name"
        from "quant_order_day" (_verid)
        WHERE
            order_day >= 5
            AND order_day < 20
        GROUP BY
            rquantid
        UNION
        SELECT
            count(order_day) as number, sum(long_amount) as lamounts, sum(stop_amount) as samounts, '20-90' as "name"
        from "quant_order_day" (_verid)
        WHERE
            order_day >= 20
            AND order_day < 90
        GROUP BY
            rquantid
        UNION
        SELECT
            count(order_day) as number, sum(long_amount) as lamounts, sum(stop_amount) as samounts, '90' as "name"
        from "quant_order_day" (_verid)
        WHERE
            order_day >= 90
        GROUP BY
            rquantid
    ) data GROUP BY data.name ORDER BY data.name ) profit order BY profit.name;
END; $$;


ALTER FUNCTION public.quant_order_day_count(_verid integer) OWNER TO dbuser;

--
-- Name: quant_order_day_sum(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_order_day_sum(_verid integer) RETURNS TABLE(profits double precision, time_long text)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    
SELECT ((profit.samounts - profit.lamounts)/profit.lamounts*100) as profits, profit.name  as "time_long" FROM (
SELECT sum(lamounts) as lamounts, sum(samounts) as samounts  , name
FROM (
        SELECT
            count(order_day) as number, sum(long_amount) as lamounts, sum(stop_amount) as samounts, '1-5' as "name"
        from "quant_order_day" (_verid)
        WHERE
            order_day < 5
        GROUP BY
            rquantid
        UNION
        SELECT
            count(order_day) as number, sum(long_amount) as lamounts, sum(stop_amount) as samounts, '5-20' as "name"
        from "quant_order_day" (_verid)
        WHERE
            order_day >= 5
            AND order_day < 20
        GROUP BY
            rquantid
        UNION
        SELECT
            count(order_day) as number, sum(long_amount) as lamounts, sum(stop_amount) as samounts, '20-90' as "name"
        from "quant_order_day" (_verid)
        WHERE
            order_day >= 20
            AND order_day < 90
        GROUP BY
            rquantid
        UNION
        SELECT
            count(order_day) as number, sum(long_amount) as lamounts, sum(stop_amount) as samounts, '90' as "name"
        from "quant_order_day" (_verid)
        WHERE
            order_day >= 90
        GROUP BY
            rquantid
    ) data GROUP BY data.name ORDER BY data.name ) profit order BY profit.name;
END; $$;


ALTER FUNCTION public.quant_order_day_sum(_verid integer) OWNER TO dbuser;

--
-- Name: quant_order_tl(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_order_tl(_verid integer) RETURNS TABLE(rquantid bigint, rvalue double precision, rticket bigint, rvtype text)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    SELECT data.quantid,data.value,data.ticket,data.vtype
FROM (
        (
            SELECT (
                    values
                        * 100.0
                ) as value, key as ticket, (
                    CASE
                        WHEN type = 4 THEN 'take'
                        ELSE 'loss'
                    END
                ) as vtype,quantid
            from analselog
            WHERE
                "type" in (4, 5)
                AND key in (SELECT "bticket" from "e2q_history" WHERE "verid" = _verid)
            ORDER BY ticket
        )
        UNION
        (
            SELECT
                "profit" as value, "bticket" as ticket, 'order' as vtype, quantid
            from "e2q_history"
            WHERE
                "bticket" in (
                    SELECT key
                    from "analselog"
                    WHERE
                        "type" = 5
                    
                ) AND "verid"= _verid
        )
    ) data
ORDER BY data.ticket; 
END; $$;


ALTER FUNCTION public.quant_order_tl(_verid integer) OWNER TO dbuser;

--
-- Name: quant_profit(bigint); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_profit(_qid bigint) RETURNS TABLE(qid bigint, margin double precision, rticket bigint, profits double precision, pday text, pside integer, profit_x double precision, profit_sum double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
SELECT
    profitx.qid,
    profitx.margin,
    profitx.ticket,
    profitx.profit,
    to_char(profitx.ctime, 'YYYY-MM-DD HH24:MI:SS') AS pday,
    profitx.side as pside,
    profitx.diff as "profit_x",
    sum(profitx.diff) over (
        order by profitx.id
    ) as "profit_sum"
FROM (
        SELECT _qid AS qid,ntrade_report.id,ntrade_report.margin, ntrade_report.profit,ntrade_report.ticket,ntrade_report.side, ntrade_report.ctime
, (
                 CASE WHEN ntrade_report.side != 3 THEN  
                    ntrade_report.profit - coalesce(
                    lag(ntrade_report.margin, 1) OVER (
                        ORDER BY ntrade_report.id
                    ), ~( SELECT ( SELECT "init_cash" FROM "analse" WHERE "quantid"=_qid)::numeric::integer -1)
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
               tr.margin - COALESCE( (SELECT sum(profit) FROM trade_report WHERE "ticket" =  tr."ticket" AND "side"= 3 ) ,0)
                ELSE  
                tr."margin"
            END ) as margin , 
            tr.profit,
            tr.ticket, tr.side,    (to_timestamp(tr.ctime / 1000) + ((tr.ctime % 1000 ) || ' milliseconds') :: INTERVAL) AS ctime    
        FROM "trade_report" tr
        WHERE
            tr."ticket" in (
                SELECT id
                FROM "trades"
                WHERE
                    "quantid" = _qid
                    AND "stat" = 0
                    AND trades.side != 3
            )
            AND tr."side"!=3
            AND tr."side"!=4 ) as ntrade_report 
    ) as profitx;
END; $$;


ALTER FUNCTION public.quant_profit(_qid bigint) OWNER TO dbuser;

--
-- Name: quant_profit_mvo(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_profit_mvo(atype integer) RETURNS TABLE(name text, argv text, version text, quantid_mvo bigint, init_cash double precision, profit double precision, postion double precision, verid integer, targetcompid text)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
SELECT
    a.name::text as name ,
    a.argv::text as argv,
    t.version::text as version,
    a.quantid::BIGINT as quantid_mvo ,
    a.init_cash,
    a.profit,
    mvo.values,
    a.verid,
    (
        SELECT f.targetcompid
        FROM trade_report t_1, fixsession f
        WHERE (
                (t_1.sessionid = f.id)
                AND (
                    t_1.ticket IN (
                        SELECT trades.id
                        FROM trades
                        WHERE (trades.quantid = a.quantid)
                    )
                )
            )
        LIMIT 1
    )::text AS targetcompid
FROM analse a, trade_info t, (
        SELECT DISTINCT
            on (ana."quantid") ana."quantid", (
                SELECT "values"
                from "analselog"
                WHERE
                    "type" = atype
                    AND "quantid" = ana."quantid"
                ORDER BY id DESC
                LIMIT 1
            ) as
        values
        from "analselog" ana
        WHERE
            ana."type" = atype
    ) mvo
WHERE t.id = a.verid
AND a."quantid" = mvo.quantid;
END; $$;


ALTER FUNCTION public.quant_profit_mvo(atype integer) OWNER TO dbuser;

--
-- Name: quant_profit_one_verid(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_profit_one_verid(ver_id integer, offset_t integer) RETURNS TABLE(vday text, vprofit_sum double precision, stat text, pqid text, qverid integer)
    LANGUAGE plpgsql
    AS $$
begin
	return query
SELECT
    pday AS vday,
    profit_sum AS vprofit_sum ,
    CASE
        WHEN pside = 1 THEN '开仓'
        ELSE '平仓'
    END AS stat,
    qid::text AS pqid,
    ver_id AS qverid
FROM "quant_profit" (
        (
            SELECT  a."quantid"
            FROM
                "analse" a
            WHERE a."verid" = ver_id
            ORDER BY id
            OFFSET offset_t LIMIT 1
        )
    );
end;
$$;


ALTER FUNCTION public.quant_profit_one_verid(ver_id integer, offset_t integer) OWNER TO dbuser;

--
-- Name: quant_return(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_return(_cficode integer) RETURNS TABLE(rstock text, pday text, price double precision, return_value double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    (
        SELECT (
            SELECT "stock"
            from "stockinfo"
            WHERE
                "symbol" = (CASE 
                            WHEN _cficode < 0 THEN  0
                            ELSE  _cficode
                        END)
            LIMIT 1
        )::text as rstock,
        to_char(
            to_timestamp(
                ((ctime / 1000))::double precision
            ),
            'YYYY-MM-DD HH24:MI:SS'
        ) as pday,
        "values" as price,
        (
            (
                "values" - first_value("values") OVER (
                    ORDER BY id
                )
            ) / first_value("values") OVER (
                ORDER BY id
            ) * 100
        ) as return_value
    FROM "analselog"
    WHERE
        "type" = 3
        AND "key" = _cficode 
    ORDER BY id
    );
END; $$;


ALTER FUNCTION public.quant_return(_cficode integer) OWNER TO dbuser;

--
-- Name: quant_return_day(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_return_day(_cficode integer) RETURNS TABLE(rstock text, pday text, price double precision, return_value double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    (
        SELECT (
        SELECT "stock"
        from "stockinfo"
        WHERE
            "symbol" = (CASE 
                            WHEN _cficode < 0 THEN  0
                            ELSE  _cficode
                        END)
        LIMIT 1
    )::text as rstock,
    to_char(
        to_timestamp(
            ((ctime / 1000))::double precision
        ),
        'YYYY-MM-DD HH24:MI:SS'
    ) as pday,
    "values" as price,
    COALESCE(
        (
            (
                values
                    - lag(
                        values
,
                            1
                    ) OVER (
                        ORDER BY id
                    )
            ) / lag(
                values
,
                    1
            ) OVER (
                ORDER BY id
            ) 
        ),
        0
    ) as return_value
FROM "analselog"
WHERE
    "type" = 3
    AND "key" = _cficode 
ORDER BY id
    );
END; $$;


ALTER FUNCTION public.quant_return_day(_cficode integer) OWNER TO dbuser;

--
-- Name: quant_return_fmonth(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_return_fmonth(_cficode integer) RETURNS TABLE(rstock text, pday text, price double precision, return_value double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    (
        
       SELECT 
       (SELECT "stock" from "stockinfo" WHERE "symbol" =  (CASE 
                            WHEN _cficode < 0 THEN  0
                            ELSE  _cficode
                        END) LIMIT 1)::text as rstock,
       return_month.ctime, return_month.price, COALESCE(
        (
            (
                return_month.price - lag(return_month.price, 1) OVER (
                    ORDER BY return_month.ctime
                )
            ) / lag(return_month.price, 1) OVER (
                ORDER BY return_month.ctime
            ) * 100
        ), 0
    ) as return_value
FROM (
        SELECT DISTINCT
            on (data.ctime) data.ctime, data.price
        FROM (
                SELECT to_char(
                        to_timestamp(
                            ((ctime / 1000))::double precision
                        ), 'YYYY-MM'
                    ) as ctime, "values" as price
                FROM "analselog"
                WHERE
                    "type" = 3
                    AND "key" = _cficode 
                ORDER BY id
            ) data
    ) return_month

    );
END; $$;


ALTER FUNCTION public.quant_return_fmonth(_cficode integer) OWNER TO dbuser;

--
-- Name: quant_return_month(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_return_month(_cficode integer) RETURNS TABLE(rstock text, pday text, price double precision, return_value double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    (
        
        SELECT
                (SELECT "stock" from "stockinfo" WHERE "symbol" =  (CASE 
                            WHEN _cficode < 0 THEN  0
                            ELSE  _cficode
                        END) LIMIT 1)::text as rstock,
                return_month.ctime, return_month.price, 
                (
                    (
                        return_month.price - first_value(return_month.price) OVER (
                            ORDER BY return_month.ctime
                        )
                    ) / first_value(return_month.price) OVER (
                        ORDER BY return_month.ctime
                    ) * 100
                ) as return_value
        FROM (
                SELECT DISTINCT
                    on (data.ctime) data.ctime, data.price
                FROM (
                        SELECT to_char(
                                to_timestamp(
                                    ((ctime / 1000))::double precision
                                ), 'YYYY-MM'
                            ) as ctime, "values" as price
                        FROM "analselog"
                        WHERE
                            "type" = 3
                            AND "key" = _cficode 
                        ORDER BY id
                    ) data
            ) return_month

    );
END; $$;


ALTER FUNCTION public.quant_return_month(_cficode integer) OWNER TO dbuser;

--
-- Name: quant_risk_profix(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_risk_profix(_verid integer) RETURNS TABLE(rid integer, rquantid bigint, rday text, rcredit double precision, rpcredit_first double precision, rpcredit_pre double precision)
    LANGUAGE plpgsql
    AS $$
begin
	return query
		SELECT data.id, data.quantid, data.day::text, data.credit, data.pcredit_first, (
                data.pcredit_first / (
                    first_value(data."credit") OVER (
                        ORDER BY data.id
                    )
                ) * 100
            ) as pcredit_pre
        FROM (
                SELECT tr.id, ts.quantid, to_timestamp(
                        ((tr.ctime / 1000))::double precision
                    ) AS day, tr.credit, tr.credit - (
                        first_value(tr.credit) OVER (
                            ORDER BY tr.id
                        )
                    ) pcredit_first
                from
                    "trade_report" tr, trades ts, stockinfo si
                WHERE
                    tr.ticket = ts.id
                    AND tr."side" != 4
                    AND ts.symbol = si.id
                    AND si.verid = _verid
                ORDER BY tr.id
            ) data;
end;
$$;


ALTER FUNCTION public.quant_risk_profix(_verid integer) OWNER TO dbuser;

--
-- Name: quant_risk_profix_month(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_risk_profix_month(_verid integer) RETURNS TABLE(rid integer, rquantid bigint, rday text, rcredit double precision, rpcredit_first double precision, rpcredit_pre double precision)
    LANGUAGE plpgsql
    AS $$
begin
	return query
            
    SELECT data.id,data.quantid, data.day::text, data.credit, data.pcredit_first, (
            data.pcredit_first / (
                first_value(data."credit") OVER (
                    ORDER BY data.id
                )
            )*100
        ) as pcredit_pre
    FROM (
            SELECT DISTINCT
                on (mdata.day) mdata.day, mdata.id,mdata.quantid, mdata.credit, mdata.pcredit_first
            FROM (
                    SELECT tr.id, ts.quantid, 
                        to_char(
                        to_timestamp(
                            ((tr.ctime / 1000))::double precision
                        ), 'YYYY-MM'
                    ) as day
                        , tr.credit, tr.credit - (
                            first_value(tr.credit) OVER (
                                ORDER BY tr.id
                            )
                        ) pcredit_first
                    from
                        "trade_report" tr, trades ts, stockinfo si
                    WHERE
                        tr.ticket = ts.id
                        AND tr."side" != 4
                        AND ts.symbol = si.id
                        AND si.verid = _verid
                    ORDER BY tr.id
                ) mdata
        ) data;
end;
$$;


ALTER FUNCTION public.quant_risk_profix_month(_verid integer) OWNER TO dbuser;

--
-- Name: quant_take_loss(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.quant_take_loss(_verid integer) RETURNS TABLE(type integer, stat text, quantid bigint, number integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    (
        SELECT DISTINCT
            ON (l."type",l."quantid") l."type",
            (
                CASE
                WHEN l."type" = 101 THEN '止损'
                WHEN l."type" = 102 THEN '止赢'
                ELSE '策略平仓'
                END
            ) AS stat,
            l."quantid",
            count(l."type")::INTEGER AS number
        FROM
            "analselog" l,
            "analse" a
        WHERE
            l.type >= 100
            AND l.type <= 102
            AND a."verid" = _verid
            AND a."quantid" = l."quantid"
        GROUP BY
            l."quantid",
            l.type ORDER BY l."quantid"

    );
END; $$;


ALTER FUNCTION public.quant_take_loss(_verid integer) OWNER TO dbuser;

--
-- Name: risk_balance_for_day(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_balance_for_day(_verid integer, _init_cash integer) RETURNS TABLE(tday text, balances double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
   
    SELECT pdays AS tday, sum(filled_balance) balances FROM (
    SELECT *
            from risk_balance_for_total_loop (_verid, _init_cash)
    ) tdata GROUP BY tdata.pdays ORDER BY  tdata.pdays;
END; $$;


ALTER FUNCTION public.risk_balance_for_day(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_balance_for_total(integer, integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_balance_for_total(_verid integer, _fix_id integer, _init_cash integer) RETURNS TABLE(pdays text, filled_targetcompid text, filled_balance double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    WITH data_with_groups AS ( 
        SELECT DISTINCT on( ptime.pday) pday, pdata.targetcompid, pdata.balance, COUNT(pdata.balance) OVER (
                    order by pday
                ) AS idx_balance
            FROM (
                    SELECT DISTINCT
                        to_timestamp(
                            ((ctime / 1000))::double precision
                        )::text AS pday
                    FROM trade_report
                    WHERE
                        "side" != 4
                        AND
                        "ticket" in (
                            SELECT id
                            FROM "trades"
                            WHERE
                                "quantid" in (
                                    SELECT "quantid"
                                    FROM "analse"
                                    WHERE
                                        "verid" = _verid
                                )
                        )
                ) ptime
                LEFT JOIN (
                    SELECT fix.targetcompid, tr.balance, to_timestamp(
                            ((tr.ctime / 1000))::double precision
                        )::text AS pday
                    FROM
                        "trade_report" tr, "fixsession" fix
                    WHERE
                        fix.id = tr."sessionid"
                        AND tr."side" != 4
                        AND "fix".id IN (
                            SELECT "sessionid"
                            FROM "account"
                            WHERE
                                "verid" = _verid
                        )
                        AND "fix".id = _fix_id
                    ORDER BY tr.ctime
                ) pdata USING (pday)
    )
    SELECT
        pday as pdays,
        COALESCE(
            max(targetcompid) OVER (
                PARTITION BY
                    idx_balance
                ORDER BY idx_balance
            ),
             'CLIENT' || _fix_id || ''
        ) AS filled_targetcompid,
        COALESCE(
            max(balance) OVER (
                PARTITION BY
                    idx_balance
                ORDER BY idx_balance
            ),
            _init_cash
        ) AS filled_balance
    FROM data_with_groups ORDER BY pdays;
END; $$;


ALTER FUNCTION public.risk_balance_for_total(_verid integer, _fix_id integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_balance_for_total_loop(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_balance_for_total_loop(_verid integer, _init_cash integer) RETURNS TABLE(pdays text, filled_targetcompid text, filled_balance double precision)
    LANGUAGE plpgsql
    AS $$
DECLARE
    acc_res RECORD;
BEGIN
    FOR acc_res IN SELECT sessionid from account WHERE verid = _verid
   LOOP
      RETURN QUERY EXECUTE
      'select * from risk_balance_for_total('|| _verid || ','|| acc_res.sessionid || ',' || _init_cash ||')';
   END LOOP;
END
$$;


ALTER FUNCTION public.risk_balance_for_total_loop(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_credit_for_day(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_credit_for_day(_verid integer, _init_cash integer) RETURNS TABLE(tday text, credits double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    SELECT pdays AS tday, sum(filled_credit) credits
    FROM (
            SELECT *
            from risk_credit_for_total_loop (_verid, _init_cash)
        ) tdata
    GROUP BY
        tdata.pdays
    ORDER BY tdata.pdays DESC;
END; $$;


ALTER FUNCTION public.risk_credit_for_day(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_credit_for_total(integer, integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_credit_for_total(_verid integer, _fix_id integer, _init_cash integer) RETURNS TABLE(pdays text, filled_targetcompid text, filled_credit double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY         
    WITH
        data_with_groups AS (
            SELECT DISTINCT on( ptime.pday) pday, pdata.targetcompid, pdata.credit, COUNT(pdata.credit) OVER (
                    order by pday
                ) AS idx_credit
            FROM (
                    SELECT DISTINCT
                        to_timestamp(
                            ((ctime / 1000))::double precision
                        )::text AS pday
                    FROM trade_report
                    WHERE
                        "side" != 4
                        AND "ticket" in (
                            SELECT id
                            FROM "trades"
                            WHERE
                                "quantid" in (
                                    SELECT "quantid"
                                    FROM "analse"
                                    WHERE
                                        "verid" = _verid
                                )
                        )
                ) ptime
                LEFT JOIN (
                    SELECT fix.targetcompid, tr."credit", to_timestamp(
                            ((tr.ctime / 1000))::double precision
                        )::text AS pday
                    FROM
                        "trade_report" tr, "fixsession" fix
                    WHERE
                        fix.id = tr."sessionid"
                        AND "fix".id IN (
                            SELECT "sessionid"
                            FROM "account"
                            WHERE
                                "verid" = _verid
                        )
                        AND "fix".id = _fix_id
                    ORDER BY tr.ctime
                ) pdata USING (pday)
        )
    SELECT
        pday as pdays,
        -- targetcompid,
        -- credit,
        -- idx_credit,
        -- Fill NULLs within each group with the first non-NULL value
        COALESCE(
            max(targetcompid) OVER (
                PARTITION BY
                    idx_credit
                ORDER BY idx_credit
            ),
            'CLIENT' || _fix_id || ''
        ) AS filled_targetcompid,
        COALESCE(
            max(credit) OVER (
                PARTITION BY
                    idx_credit
                ORDER BY idx_credit
            ),
             _init_cash
        ) AS filled_credit
    FROM data_with_groups;
END; $$;


ALTER FUNCTION public.risk_credit_for_total(_verid integer, _fix_id integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_credit_for_total_loop(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_credit_for_total_loop(_verid integer, _init_cash integer) RETURNS TABLE(pdays text, filled_targetcompid text, filled_credit double precision)
    LANGUAGE plpgsql
    AS $$
DECLARE
    acc_res RECORD;
BEGIN
    FOR acc_res IN SELECT sessionid from account WHERE verid = _verid
   LOOP
      RETURN QUERY EXECUTE
      'select * from risk_credit_for_total('|| _verid || ','|| acc_res.sessionid || ',' || _init_cash ||')';
   END LOOP;
END
$$;


ALTER FUNCTION public.risk_credit_for_total_loop(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_drawdown_for_month(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_drawdown_for_month(_verid integer, _init_cash integer) RETURNS TABLE(rday text, return_rates double precision, cumulative_returns double precision, peak_values double precision, drawdown_rates double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
WITH
    cumulative_returns AS (
        -- 步骤1：计算累计收益率（毛收益率累乘）
        SELECT
            tdays as tradedate,
            returns_month as return_rate,
            -- 计算毛收益率（1 + 收益率），然后累乘得到累计净值
            EXP(
                SUM(
                    LN(
                        1 + COALESCE(returns_month / 100.0, 0)
                    )
                ) OVER (
                    ORDER BY tdays
                )
            ) AS cumulative_return
        FROM risk_returns_for_month (_verid, _init_cash)
    ),
    drawdown_calc AS (
        -- 步骤2：计算每个时间点的历史峰值
        SELECT
            (tradedate || '')::TEXT as tradedate,
            return_rate,
            cumulative_return,
            MAX(cumulative_return) OVER (
                ORDER BY tradedate
            ) AS peak_value,
            -- 步骤3：计算回撤率
            (
                MAX(cumulative_return) OVER (
                    ORDER BY tradedate
                ) - cumulative_return
            ) / MAX(cumulative_return) OVER (
                ORDER BY tradedate
            ) AS drawdown_rate
        FROM cumulative_returns
    )
    -- 步骤4：取最大回撤率
SELECT
(tradedate || '')::TEXT as rday, return_rate AS return_rates  ,  cumulative_return as cumulative_returns ,peak_value as peak_values  , drawdown_rate as drawdown_rates
    -- ROUND((MAX(drawdown_rate) * 100)::numeric, 4) AS max_drawdown_percent,
    -- (
    --     SELECT peak_value
    --     FROM drawdown_calc
    --     WHERE
    --         drawdown_rate = (
    --             SELECT MAX(drawdown_rate)
    --             FROM drawdown_calc
    --         )
    --     LIMIT 1
    -- ) AS drawdown_start_value
FROM drawdown_calc;
END; $$;


ALTER FUNCTION public.risk_drawdown_for_month(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_drawdown_loop_month(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_drawdown_loop_month(_verid integer, _init_cash integer) RETURNS TABLE(rday text, return_rates double precision, cumulative_returns double precision, peak_values double precision, drawdown_rates double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
WITH
    cumulative_returns AS (
        -- 步骤1：计算累计收益率（毛收益率累乘）
        SELECT
            tdays as tradedate,
            returns_month as return_rate,
            -- 计算毛收益率（1 + 收益率），然后累乘得到累计净值
            EXP(
                SUM(
                    LN(
                        1 + COALESCE(returns_month / 100.0, 0)
                    )
                ) OVER (
                    ORDER BY tdays
                )
            ) AS cumulative_return
        FROM risk_returns_loop_month (_verid, _init_cash)
    ),
    drawdown_calc AS (
        -- 步骤2：计算每个时间点的历史峰值
        SELECT
            (tradedate || '')::TEXT as tradedate,
            return_rate,
            cumulative_return,
            MAX(cumulative_return) OVER (
                ORDER BY tradedate
            ) AS peak_value,
            -- 步骤3：计算回撤率
            (
                MAX(cumulative_return) OVER (
                    ORDER BY tradedate
                ) - cumulative_return
            ) / MAX(cumulative_return) OVER (
                ORDER BY tradedate
            ) AS drawdown_rate
        FROM cumulative_returns
    )
    -- 步骤4：取最大回撤率
SELECT
(tradedate || '')::TEXT as rday, return_rate AS return_rates  ,  cumulative_return as cumulative_returns ,peak_value as peak_values  , drawdown_rate as drawdown_rates
    -- ROUND((MAX(drawdown_rate) * 100)::numeric, 4) AS max_drawdown_percent,
    -- (
    --     SELECT peak_value
    --     FROM drawdown_calc
    --     WHERE
    --         drawdown_rate = (
    --             SELECT MAX(drawdown_rate)
    --             FROM drawdown_calc
    --         )
    --     LIMIT 1
    -- ) AS drawdown_start_value
FROM drawdown_calc;
END; $$;


ALTER FUNCTION public.risk_drawdown_loop_month(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_drawdown_max_month(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_drawdown_max_month(_verid integer, _init_cash integer) RETURNS TABLE(max_drawdown_rate numeric)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    SELECT ROUND(MAX(loss_rate)::NUMERIC, 5) AS max_drawdown_rate
    FROM (
        SELECT
            a.tdays AS peak_date, a.cumulative_return AS peak_value, b.tdays AS trough_date, b.cumulative_return AS trough_value, (
                a.cumulative_return - b.cumulative_return
            ) / a.cumulative_return AS loss_rate
        FROM (
                -- 先计算累计收益率
                SELECT tdays, EXP(
                        SUM(
                            LN(1 + COALESCE(returns_month/100.0, 0))
                        ) OVER (
                            ORDER BY tdays
                        )
                    ) AS cumulative_return
                FROM risk_returns_loop_month (_verid, _init_cash)
            ) a
            JOIN (
                SELECT tdays, EXP(
                        SUM(
                            LN(1 + COALESCE(returns_month/100.0, 0))
                        ) OVER (
                            ORDER BY tdays
                        )
                    ) AS cumulative_return
                FROM risk_returns_loop_month (_verid, _init_cash)
            ) b ON a.tdays < b.tdays -- 只考虑后续日期
        WHERE
            a.cumulative_return > b.cumulative_return -- 只考虑下跌情况
    ) s;
END; $$;


ALTER FUNCTION public.risk_drawdown_max_month(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_margin_for_day(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_margin_for_day(_verid integer, _init_cash integer) RETURNS TABLE(tday text, margins double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
   
    SELECT pdays AS tday, sum(filled_margin) margins FROM (
        SELECT *
            from risk_margin_for_total_loop (_verid, _init_cash)
    ) tdata GROUP BY tdata.pdays ORDER BY  tdata.pdays;
END; $$;


ALTER FUNCTION public.risk_margin_for_day(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_margin_for_total(integer, integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_margin_for_total(_verid integer, _fix_id integer, _init_cash integer) RETURNS TABLE(pdays text, filled_targetcompid text, filled_margin double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    WITH data_with_groups AS ( 
        SELECT DISTINCT on( ptime.pday) pday, pdata.targetcompid, pdata.margin, COUNT(pdata.margin) OVER (
                    order by pday
                ) AS idx_margin
            FROM (
                    SELECT DISTINCT
                        to_timestamp(
                            ((ctime / 1000))::double precision
                        )::text AS pday
                    FROM trade_report
                    WHERE
                        "side" != 4
                        AND
                        "ticket" in (
                            SELECT id
                            FROM "trades"
                            WHERE
                                "quantid" in (
                                    SELECT "quantid"
                                    FROM "analse"
                                    WHERE
                                        "verid" = _verid
                                )
                        )
                ) ptime
                LEFT JOIN (
                    SELECT fix.targetcompid, tr.margin, to_timestamp(
                            ((tr.ctime / 1000))::double precision
                        )::text AS pday
                    FROM
                        "trade_report" tr, "fixsession" fix
                    WHERE
                        fix.id = tr."sessionid"
                        AND "fix".id IN (
                            SELECT "sessionid"
                            FROM "account"
                            WHERE
                                "verid" = _verid
                        )
                        AND "fix".id = _fix_id
                        AND tr."side" != 4
                    ORDER BY tr.ctime
                ) pdata USING (pday)
    )
    SELECT
        pday as pdays,
        COALESCE(
            max(targetcompid) OVER (
                PARTITION BY
                    idx_margin
                ORDER BY idx_margin
            ),
            'CLIENT' || _fix_id || ''
        ) AS filled_targetcompid,
        COALESCE(
            max(margin) OVER (
                PARTITION BY
                    idx_margin
                ORDER BY idx_margin
            ),
            _init_cash
        ) AS filled_margin
    FROM data_with_groups ORDER BY pdays;
END; $$;


ALTER FUNCTION public.risk_margin_for_total(_verid integer, _fix_id integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_margin_for_total_loop(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_margin_for_total_loop(_verid integer, _init_cash integer) RETURNS TABLE(pdays text, filled_targetcompid text, filled_margin double precision)
    LANGUAGE plpgsql
    AS $$
DECLARE
    acc_res RECORD;
BEGIN
    FOR acc_res IN SELECT sessionid from account WHERE verid = _verid
   LOOP
      RETURN QUERY EXECUTE
      'select * from risk_margin_for_total('|| _verid || ','|| acc_res.sessionid || ',' || _init_cash ||')';
   END LOOP;
END
$$;


ALTER FUNCTION public.risk_margin_for_total_loop(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_returns_for_day(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_returns_for_day(_verid integer, _init_cash integer) RETURNS TABLE(tdays text, credits double precision, returns_day double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    SELECT rdata.tday as tdays, rdata.credits, (
            (
                rdata.credits - first_value(rdata.credits) OVER (
                    ORDER BY rdata.tday
                )
            ) / first_value(rdata.credits) OVER (
                ORDER BY rdata.tday
            ) * 100.0
        ) AS returns_day
    FROM (
            SELECT * FROM risk_credit_for_day(_verid,_init_cash)
        ) rdata;

END; $$;


ALTER FUNCTION public.risk_returns_for_day(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_returns_for_month(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_returns_for_month(_verid integer, _init_cash integer) RETURNS TABLE(tdays text, credits double precision, returns_month double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    SELECT
    rdata.pdays,
    rdata.credits,
    (
        (
        rdata.credits - first_value(rdata.credits) OVER (
            ORDER BY
            rdata.pdays
        )
        ) / first_value(rdata.credits) OVER (
        ORDER BY
            rdata.pdays
        ) * 100.0
    ) AS returns_month
    FROM
    (
        SELECT DISTINCT
        ON (mdatas.pdays) pdays,
        mdatas.credits
        FROM
        (
            SELECT
            substring(
                mdata.tdays
                FROM
                0 FOR 8
            ) AS pdays,
            mdata.credits
            FROM
            (
                SELECT * FROM risk_returns_for_day(_verid,_init_cash)
            ) mdata
        ) mdatas
    ) rdata;

END; $$;


ALTER FUNCTION public.risk_returns_for_month(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_returns_loop_day(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_returns_loop_day(_verid integer, _init_cash integer) RETURNS TABLE(tdays text, credits double precision, returns_day double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    SELECT rdata.tday as tdays, rdata.credits, (
            (
                rdata.credits - lag(rdata.credits,1) OVER (
                    ORDER BY rdata.tday
                )
            ) / lag(rdata.credits,1) OVER (
                ORDER BY rdata.tday
            ) * 100.0
        ) AS returns_day
    FROM (
            SELECT * FROM risk_credit_for_day(_verid,_init_cash)
        ) rdata;

END; $$;


ALTER FUNCTION public.risk_returns_loop_day(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: risk_returns_loop_month(integer, integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.risk_returns_loop_month(_verid integer, _init_cash integer) RETURNS TABLE(tdays text, credits double precision, returns_month double precision)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    SELECT
    rdata.pdays,
    rdata.credits,
    (
        (
        rdata.credits - lag(rdata.credits,1) OVER (
            ORDER BY
            rdata.pdays
        )
        ) / lag(rdata.credits,1) OVER (
        ORDER BY
            rdata.pdays
        ) * 100.0
    ) AS returns_month
    FROM
    (
        SELECT DISTINCT
        ON (mdatas.pdays) pdays,
        mdatas.credits
        FROM
        (
            SELECT
            substring(
                mdata.tdays
                FROM
                0 FOR 8
            ) AS pdays,
            mdata.credits
            FROM
            (
                SELECT * FROM risk_returns_for_day(_verid,_init_cash)
            ) mdata
        ) mdatas
    ) rdata;

END; $$;


ALTER FUNCTION public.risk_returns_loop_month(_verid integer, _init_cash integer) OWNER TO dbuser;

--
-- Name: trade_detail(integer); Type: FUNCTION; Schema: public; Owner: dbuser
--

CREATE FUNCTION public.trade_detail(_side integer) RETURNS TABLE(id bigint, verid integer, symbol integer, stock character varying, open_price double precision, open_qty bigint, open_time text, open_ticket text, amount double precision, quantid text)
    LANGUAGE plpgsql
    AS $$
BEGIN
    RETURN QUERY 
    SELECT
        buy.id,
        (
            SELECT stockinfo.verid
            FROM stockinfo
            WHERE (buy.symbol = stockinfo.id)
            LIMIT 1
        ) AS verid,
        (
            SELECT stockinfo.symbol
            FROM stockinfo
            WHERE (stockinfo.id = buy.symbol)
            LIMIT 1
        ) AS symbol,
        (
            SELECT stockinfo.stock
            FROM stockinfo
            WHERE (stockinfo.id = buy.symbol)
            LIMIT 1
        ) AS stock,
        buy.price AS open_price,
        (buy.qty * 100) AS open_qty,
        to_char(
            to_timestamp(
                ((buy.ctime / 1000))::double precision
            ),
            'YYYY/MM/DD'::text
        ) AS open_time,
        (buy.ticket)::text AS open_ticket,
        buy.amount,
        (buy.quantid)::text AS quantid
    FROM trades buy, analse ana
    WHERE (
            (buy.side = _side)        
            AND (ana.quantid = buy.quantid)
            AND  buy."ticket" NOT IN ( SELECT "ticket" from "trades" WHERE "side"= _side AND  "stat"=2)
        )
    ORDER BY buy.ctime;
END; $$;


ALTER FUNCTION public.trade_detail(_side integer) OWNER TO dbuser;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: account; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.account (
    id integer NOT NULL,
    sessionid integer,
    balance double precision DEFAULT 0,
    credit double precision DEFAULT 0,
    equity double precision DEFAULT 0,
    leverage double precision DEFAULT 1,
    freemargin double precision DEFAULT 0,
    margin double precision DEFAULT 0,
    profit double precision DEFAULT 0,
    ctime integer DEFAULT 0,
    verid integer
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

COMMENT ON COLUMN public.account.credit IS '帐户信用';


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
    quantid bigint,
    name character varying(255),
    argv character varying(255),
    verid integer,
    ctime integer,
    profit double precision DEFAULT 0,
    postion double precision DEFAULT 0,
    init_cash double precision DEFAULT 0
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
-- Name: COLUMN analse.postion; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analse.postion IS '仓位百分比';


--
-- Name: COLUMN analse.init_cash; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analse.init_cash IS '初始化资金';


--
-- Name: analselog; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.analselog (
    id integer NOT NULL,
    quantid bigint,
    "values" double precision,
    type integer,
    ctime bigint,
    key integer DEFAULT 0
);


ALTER TABLE public.analselog OWNER TO dbuser;

--
-- Name: TABLE analselog; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON TABLE public.analselog IS '记录指标数值的';


--
-- Name: COLUMN analselog.quantid; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analselog.quantid IS '对应 analse 的 quantid';


--
-- Name: COLUMN analselog."values"; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analselog."values" IS '数据';


--
-- Name: COLUMN analselog.type; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analselog.type IS '类型';


--
-- Name: COLUMN analselog.ctime; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analselog.ctime IS '时间';


--
-- Name: COLUMN analselog.key; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.analselog.key IS 'key';


--
-- Name: analselog_id_seq; Type: SEQUENCE; Schema: public; Owner: dbuser
--

ALTER TABLE public.analselog ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME public.analselog_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


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


ALTER SEQUENCE public.analytics_id_seq OWNER TO dbuser;

--
-- Name: analytics_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: dbuser
--

ALTER SEQUENCE public.analytics_id_seq OWNED BY public.analytics.id;


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
    port integer,
    login smallint DEFAULT 0 NOT NULL
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
-- Name: COLUMN fixsession.login; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.fixsession.login IS '0,未登录。1，已登录';


--
-- Name: stockinfo; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.stockinfo (
    id integer NOT NULL,
    symbol integer DEFAULT 0,
    stock character varying(255),
    verid integer,
    ctime integer DEFAULT 0,
    dtime integer DEFAULT 0,
    adj_price double precision DEFAULT 0
);


ALTER TABLE public.stockinfo OWNER TO dbuser;

--
-- Name: TABLE stockinfo; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON TABLE public.stockinfo IS '保存股票信息';


--
-- Name: COLUMN stockinfo.symbol; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.stockinfo.symbol IS 'cficode';


--
-- Name: COLUMN stockinfo.stock; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.stockinfo.stock IS 'stock name';


--
-- Name: COLUMN stockinfo.verid; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.stockinfo.verid IS '版本 id';


--
-- Name: COLUMN stockinfo.ctime; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.stockinfo.ctime IS '上市时间';


--
-- Name: COLUMN stockinfo.dtime; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.stockinfo.dtime IS '退市时间';


--
-- Name: COLUMN stockinfo.adj_price; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.stockinfo.adj_price IS '复权价格';


--
-- Name: trade_report; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.trade_report (
    id integer NOT NULL,
    ticket bigint NOT NULL,
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

COMMENT ON COLUMN public.trade_report.side IS '开仓平仓,1:buy,2:sell,3 exdr,4 commission';


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
    leavesqty bigint DEFAULT 0,
    openqty bigint DEFAULT 0,
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

COMMENT ON COLUMN public.trades.stat IS '订单状态统一使用 e2::OrdStatus enum';


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
-- Name: e2q_cash; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_cash AS
 SELECT quantid,
    name,
    stock,
    verid,
    credit AS init_cash,
    end_credit AS now_cash,
    (end_credit - credit) AS diff_cash,
    (((end_credit - credit) / credit) * (100)::double precision) AS diff_per,
    day,
    end_day
   FROM ( SELECT t.quantid,
            s.stock,
            s.verid,
            s.symbol,
            r.credit,
            ( SELECT fixsession.targetcompid
                   FROM public.fixsession
                  WHERE (fixsession.id = r.sessionid)
                 LIMIT 1) AS name,
            to_timestamp(((r.ctime / 1000))::double precision) AS day,
            ( SELECT r_1.credit
                   FROM ( SELECT t_1_1.quantid,
                            max(t_1_1.id) AS tid,
                            t_1_1.symbol
                           FROM public.trades t_1_1,
                            public.trade_report r_1_1
                          WHERE ((r_1_1.sessionid = r.sessionid) AND (r_1_1.ticket = t_1_1.id) AND (r_1_1.side <> 4) AND (r_1_1.id IN ( SELECT trp.rid
                                   FROM ( SELECT DISTINCT ON (trade_report.sessionid) trade_report.sessionid,
    max(trade_report.id) AS rid
   FROM public.trade_report
  WHERE (trade_report.side <> 4)
  GROUP BY trade_report.sessionid) trp)))
                          GROUP BY t_1_1.symbol, t_1_1.quantid) t_1,
                    public.trade_report r_1
                  WHERE ((r_1.side <> 4) AND (r_1.ticket = t_1.tid))
                 LIMIT 1) AS end_credit,
            ( SELECT to_timestamp(((r_1.ctime / 1000))::double precision) AS day
                   FROM ( SELECT t_1_1.quantid,
                            max(t_1_1.id) AS tid,
                            t_1_1.symbol
                           FROM public.trades t_1_1,
                            public.trade_report r_1_1
                          WHERE ((r_1_1.sessionid = r.sessionid) AND (r_1_1.ticket = t_1_1.id) AND (r_1_1.side <> 4) AND (r_1_1.id IN ( SELECT trp.rid
                                   FROM ( SELECT DISTINCT ON (trade_report.sessionid) trade_report.sessionid,
    max(trade_report.id) AS rid
   FROM public.trade_report
  WHERE (trade_report.side <> 4)
  GROUP BY trade_report.sessionid) trp)))
                          GROUP BY t_1_1.symbol, t_1_1.quantid) t_1,
                    public.trade_report r_1
                  WHERE ((r_1.side <> 4) AND (r_1.ticket = t_1.tid))
                 LIMIT 1) AS end_day
           FROM public.trades t,
            public.stockinfo s,
            public.trade_report r
          WHERE ((t.symbol = s.id) AND (s.symbol > 0) AND (r.ticket = t.id) AND (r.side <> 4) AND (r.id IN ( SELECT trp.rid
                   FROM ( SELECT DISTINCT ON (trade_report.sessionid) trade_report.sessionid,
                            min(trade_report.id) AS rid
                           FROM public.trade_report
                          WHERE (trade_report.side <> 4)
                          GROUP BY trade_report.sessionid) trp)))) cash_info;


ALTER VIEW public.e2q_cash OWNER TO dbuser;

--
-- Name: e2q_cash_se; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_cash_se AS
 SELECT quantid,
    name,
    stock,
    tid,
    credit,
    verid,
    symbol,
    day,
    stat
   FROM ( SELECT t.quantid,
            s.stock,
            t.id AS tid,
            r.credit,
            ( SELECT fixsession.targetcompid
                   FROM public.fixsession
                  WHERE (fixsession.id = r.sessionid)
                 LIMIT 1) AS name,
            s.verid,
            t.symbol,
            to_timestamp(((r.ctime / 1000))::double precision) AS day,
            'init'::text AS stat
           FROM public.trades t,
            public.stockinfo s,
            public.trade_report r
          WHERE ((t.symbol = s.id) AND (s.symbol > 0) AND (r.ticket = t.id) AND (r.side <> 4) AND (r.id IN ( SELECT trp.rid
                   FROM ( SELECT DISTINCT ON (trade_report.sessionid) trade_report.sessionid,
                            min(trade_report.id) AS rid
                           FROM public.trade_report
                          WHERE (trade_report.side <> 4)
                          GROUP BY trade_report.sessionid) trp)))
        UNION
         SELECT t.quantid,
            s.stock,
            t.tid,
            r.credit,
            ( SELECT fixsession.targetcompid
                   FROM public.fixsession
                  WHERE (fixsession.id = r.sessionid)
                 LIMIT 1) AS name,
            s.verid,
            t.symbol,
            to_timestamp(((r.ctime / 1000))::double precision) AS day,
            'end'::text AS stat
           FROM ( SELECT t_1.quantid,
                    max(t_1.id) AS tid,
                    t_1.symbol
                   FROM public.trades t_1,
                    public.stockinfo s_1,
                    public.trade_report r_1
                  WHERE ((t_1.symbol = s_1.id) AND (s_1.symbol > 0) AND (r_1.ticket = t_1.id) AND (r_1.side <> 4) AND (r_1.id IN ( SELECT trp.rid
                           FROM ( SELECT DISTINCT ON (trade_report.sessionid) trade_report.sessionid,
                                    max(trade_report.id) AS rid
                                   FROM public.trade_report
                                  WHERE (trade_report.side <> 4)
                                  GROUP BY trade_report.sessionid) trp)))
                  GROUP BY t_1.symbol, t_1.quantid) t,
            public.stockinfo s,
            public.trade_report r
          WHERE ((t.symbol = s.id) AND (s.symbol > 0) AND (r.ticket = t.tid) AND (r.side <> 4))) cash_data
  ORDER BY quantid, tid;


ALTER VIEW public.e2q_cash_se OWNER TO dbuser;

--
-- Name: exdr; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.exdr (
    id integer NOT NULL,
    symbol integer,
    cash double precision DEFAULT 0,
    shares double precision DEFAULT 0,
    extype integer,
    ymd integer,
    outstanding double precision,
    outstandend double precision,
    marketcaping double precision,
    marketcapend double precision,
    split double precision DEFAULT 0
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
-- Name: COLUMN exdr.split; Type: COMMENT; Schema: public; Owner: dbuser
--

COMMENT ON COLUMN public.exdr.split IS 'ETF 类的拆分';


--
-- Name: e2q_history; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_history AS
 SELECT buy.id AS sid,
    ( SELECT stockinfo.verid
           FROM public.stockinfo
          WHERE (buy.symbol = stockinfo.id)
         LIMIT 1) AS verid,
    ( SELECT stockinfo.symbol
           FROM public.stockinfo
          WHERE (buy.symbol = stockinfo.id)
         LIMIT 1) AS symobl,
    ( SELECT stockinfo.stock
           FROM public.stockinfo
          WHERE (buy.symbol = stockinfo.id)
         LIMIT 1) AS stock,
    buy.price AS buy_price,
    (to_timestamp(((buy.ctime / 1000))::double precision) + (((buy.ctime % (1000)::bigint) || ' milliseconds'::text))::interval) AS buy_time,
    sell.stoppx AS stop_price,
    (to_timestamp(((sell.ctime / 1000))::double precision) + (((sell.ctime % (1000)::bigint) || ' milliseconds'::text))::interval) AS stop_time,
    sell.adjpx AS sell_adjpx,
    buy.adjpx AS buy_adjpx,
    (sell.closetck)::text AS closetck,
    COALESCE(( SELECT sum(exdr.cash) AS sum
           FROM public.exdr
          WHERE ((exdr.symbol = buy.symbol) AND ((to_char((((to_timestamp(((buy.ctime / 1000))::double precision) + (((buy.ctime % (1000)::bigint) || ' milliseconds'::text))::interval))::date)::timestamp with time zone, 'YYYYMMDD'::text))::integer <= exdr.ymd) AND (exdr.ymd <= (to_char((((to_timestamp(((sell.ctime / 1000))::double precision) + (((sell.ctime % (1000)::bigint) || ' milliseconds'::text))::interval))::date)::timestamp with time zone, 'YYYYMMDD'::text))::integer))), (0)::double precision) AS cash,
    COALESCE(( SELECT sum(exdr.shares) AS sum
           FROM public.exdr
          WHERE ((exdr.symbol = buy.symbol) AND ((to_char((((to_timestamp(((buy.ctime / 1000))::double precision) + (((buy.ctime % (1000)::bigint) || ' milliseconds'::text))::interval))::date)::timestamp with time zone, 'YYYYMMDD'::text))::integer <= exdr.ymd) AND (exdr.ymd <= (to_char((((to_timestamp(((sell.ctime / 1000))::double precision) + (((sell.ctime % (1000)::bigint) || ' milliseconds'::text))::interval))::date)::timestamp with time zone, 'YYYYMMDD'::text))::integer))), (0)::double precision) AS share,
    COALESCE(( SELECT (sum(exdr.split) * (10.0)::double precision) AS sum
           FROM public.exdr
          WHERE ((exdr.symbol = buy.symbol) AND ((to_char((((to_timestamp(((buy.ctime / 1000))::double precision) + (((buy.ctime % (1000)::bigint) || ' milliseconds'::text))::interval))::date)::timestamp with time zone, 'YYYYMMDD'::text))::integer <= exdr.ymd) AND (exdr.ymd <= (to_char((((to_timestamp(((sell.ctime / 1000))::double precision) + (((sell.ctime % (1000)::bigint) || ' milliseconds'::text))::interval))::date)::timestamp with time zone, 'YYYYMMDD'::text))::integer))), (0)::double precision) AS splits,
    a.quantid,
    buy.ticket AS bticket,
    sell.ticket AS sticket,
    ( SELECT analselog."values"
           FROM public.analselog
          WHERE ((analselog.key = buy.ticket) AND (analselog.type = 2))
         LIMIT 1) AS "position",
    ( SELECT round(((report.profits - report.margins))::numeric, 3) AS amount
           FROM ( SELECT sum(trade_report.margin) AS margins,
                    sum(trade_report.profit) AS profits
                   FROM public.trade_report
                  WHERE ((trade_report.ticket IN ( SELECT trades.id
                           FROM public.trades
                          WHERE (trades.ticket = ANY (ARRAY[buy.ticket, sell.ticket])))) AND (trade_report.side <> 4))) report) AS amount,
    ( SELECT round(((((report.profits - report.margins) / report.margins) * (100)::double precision))::numeric, 3) AS amount
           FROM ( SELECT sum(trade_report.margin) AS margins,
                    sum(trade_report.profit) AS profits
                   FROM public.trade_report
                  WHERE ((trade_report.ticket IN ( SELECT trades.id
                           FROM public.trades
                          WHERE (trades.ticket = ANY (ARRAY[buy.ticket, sell.ticket])))) AND (trade_report.side <> 4))) report) AS profit,
    sell.qty
   FROM public.trades buy,
    public.trades sell,
    public.analse a
  WHERE ((buy.ticket = sell.closetck) AND (buy.stat = 2) AND (sell.stat = 2) AND (sell.stoppx > (0)::double precision) AND (buy.price > (0)::double precision) AND (buy.quantid = a.quantid))
  ORDER BY buy.id;


ALTER VIEW public.e2q_history OWNER TO dbuser;

--
-- Name: e2q_postion; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_postion AS
 SELECT a.verid,
    a.quantid,
    l."values",
    l.type,
    (to_timestamp(((l.ctime / 1000))::double precision) + (((l.ctime % (1000)::bigint) || ' milliseconds'::text))::interval) AS date,
    (((a.name)::text || '_'::text) || (a.argv)::text) AS rule
   FROM public.analselog l,
    public.analse a
  WHERE ((a.quantid = l.quantid) AND (l.type = 2))
  ORDER BY l.ctime DESC;


ALTER VIEW public.e2q_postion OWNER TO dbuser;

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
-- Name: e2q_profit; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_profit AS
 SELECT a.name,
    a.argv,
    t.version,
    (a.quantid)::text AS quantid,
    a.init_cash,
    a.profit,
    a.postion,
    a.verid,
    ( SELECT f.targetcompid
           FROM public.trade_report t_1,
            public.fixsession f
          WHERE ((t_1.side <> 4) AND ((t_1.sessionid = f.id) AND (t_1.ticket IN ( SELECT trades.id
                   FROM public.trades
                  WHERE (trades.quantid = a.quantid)))))
         LIMIT 1) AS targetcompid
   FROM public.analse a,
    public.trade_info t
  WHERE (t.id = a.verid);


ALTER VIEW public.e2q_profit OWNER TO dbuser;

--
-- Name: e2q_risk_performance; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_risk_performance AS
 SELECT
        CASE
            WHEN (ana.type = 37) THEN 'Sharpe_Ratio'::text
            WHEN (ana.type = 36) THEN 'Annual_volatility'::text
            ELSE 'Expected_annual_return'::text
        END AS risk,
    ana.quantid,
    a.verid,
    ana."values",
    to_timestamp(((ana.ctime / 1000))::double precision) AS day
   FROM public.analselog ana,
    public.analse a
  WHERE ((ana.type = ANY (ARRAY[35, 36, 37])) AND (ana.quantid = a.quantid))
  ORDER BY ana.ctime DESC;


ALTER VIEW public.e2q_risk_performance OWNER TO dbuser;

--
-- Name: e2q_risk_profit; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_risk_profit AS
 SELECT quantid,
    to_timestamp(((ctime / 1000))::double precision) AS pday,
    amount,
    buy_amount,
    ( SELECT stockinfo.stock
           FROM public.stockinfo
          WHERE (stockinfo.id = p.symbol)
         LIMIT 1) AS stock,
    ( SELECT stockinfo.verid
           FROM public.stockinfo
          WHERE (stockinfo.id = p.symbol)
         LIMIT 1) AS verid,
        CASE
            WHEN (side = 2) THEN (amount - buy_amount)
            ELSE (0)::double precision
        END AS profit,
        CASE
            WHEN (side = 2) THEN (trunc(((((amount - buy_amount) / buy_amount) * (100)::double precision))::numeric, 3))::double precision
            ELSE (0)::double precision
        END AS profit_pre
   FROM ( SELECT data.symbol,
            data.side,
            data.quantid,
            data.ctime,
            data.amount,
                CASE
                    WHEN (data.side = 2) THEN ( SELECT trades.amount
                       FROM public.trades
                      WHERE ((trades.ticket = data.closetck) AND (trades.stat = 2) AND (trades.side = 1))
                     LIMIT 1)
                    ELSE (0)::double precision
                END AS buy_amount
           FROM ( SELECT trades.symbol,
                    trades.ticket,
                    trades.closetck,
                    trades.side,
                    trades.quantid,
                    trades.ctime,
                    trades.amount
                   FROM public.trades
                  WHERE (trades.stat = 2)
                  ORDER BY trades.symbol, trades.id) data) p;


ALTER VIEW public.e2q_risk_profit OWNER TO dbuser;

--
-- Name: e2q_risk_profit_count; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_risk_profit_count AS
 SELECT count(
        CASE
            WHEN (profit <= ('-20.0'::numeric)::double precision) THEN profit
            ELSE NULL::double precision
        END) AS "profit<=-20.0",
    count(
        CASE
            WHEN ((profit > ('-20.0'::numeric)::double precision) AND (profit <= ('-10.0'::numeric)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "-20.0<profit<=-10.0",
    count(
        CASE
            WHEN ((profit > ('-10.0'::numeric)::double precision) AND (profit <= ('-5.0'::numeric)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "-10.0<profit<=-5.0",
    count(
        CASE
            WHEN ((profit > ('-5.0'::numeric)::double precision) AND (profit <= (0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "-5.0<profit<=0",
    count(
        CASE
            WHEN ((profit > (0)::double precision) AND (profit <= (5.0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "0<profit<=5.0",
    count(
        CASE
            WHEN ((profit > (5.0)::double precision) AND (profit <= (10.0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "5.0<profit<=10.0",
    count(
        CASE
            WHEN ((profit > (10.0)::double precision) AND (profit <= (30.0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "10.0<profit<=30.0",
    count(
        CASE
            WHEN ((profit > (30.0)::double precision) AND (profit <= (50.0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "30.0<profit<=50.0",
    count(
        CASE
            WHEN ((profit > (50.0)::double precision) AND (profit <= (80.0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "50.0<profit<=80.0",
    count(
        CASE
            WHEN ((profit > (80.0)::double precision) AND (profit <= (100.0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "80.0<profit<=100.0",
    count(
        CASE
            WHEN ((profit > (100.0)::double precision) AND (profit <= (120.0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "100.0<profit<=120.0",
    count(
        CASE
            WHEN ((profit > (120.0)::double precision) AND (profit <= (140.0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "120.0<profit<=140.0",
    count(
        CASE
            WHEN ((profit > (140.0)::double precision) AND (profit <= (160.0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "140.0<profit<=160.0",
    count(
        CASE
            WHEN ((profit > (160.0)::double precision) AND (profit <= (180.0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "160.0<profit<=180.0",
    count(
        CASE
            WHEN ((profit > (180.0)::double precision) AND (profit <= (200.0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "180.0<profit<=200.0",
    count(
        CASE
            WHEN ((profit > (200.0)::double precision) AND (profit <= (220.0)::double precision)) THEN profit
            ELSE NULL::double precision
        END) AS "200.0<profit<=220.0",
    count(
        CASE
            WHEN (profit > (220.0)::double precision) THEN profit
            ELSE NULL::double precision
        END) AS "220.0<profit"
   FROM ( SELECT (((datas.credits - (( SELECT ((data_0.number)::numeric * 1000000.0) AS cash
                   FROM ( SELECT DISTINCT account.verid,
                            count(account.id) AS number
                           FROM public.account
                          WHERE (account.verid = datas.verid)
                          GROUP BY account.verid) data_0
                 LIMIT 1))::double precision) / (( SELECT ((data_1.number)::numeric * 1000000.0) AS cash
                   FROM ( SELECT DISTINCT account.verid,
                            count(account.id) AS number
                           FROM public.account
                          WHERE (account.verid = datas.verid)
                          GROUP BY account.verid) data_1
                 LIMIT 1))::double precision) * (100.0)::double precision) AS profit,
            datas.verid
           FROM ( SELECT sum(account.credit) AS credits,
                    account.verid
                   FROM public.account
                  GROUP BY account.verid
                  ORDER BY account.verid) datas) profits;


ALTER VIEW public.e2q_risk_profit_count OWNER TO dbuser;

--
-- Name: e2q_risk_profit_count_list; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_risk_profit_count_list AS
 SELECT credits,
    (((credits - (( SELECT ((data_0.number)::numeric * 1000000.0) AS cash
           FROM ( SELECT DISTINCT account.verid,
                    count(account.id) AS number
                   FROM public.account
                  WHERE (account.verid = datas.verid)
                  GROUP BY account.verid) data_0
         LIMIT 1))::double precision) / (( SELECT ((data_1.number)::numeric * 1000000.0) AS cash
           FROM ( SELECT DISTINCT account.verid,
                    count(account.id) AS number
                   FROM public.account
                  WHERE (account.verid = datas.verid)
                  GROUP BY account.verid) data_1
         LIMIT 1))::double precision) * (100.0)::double precision) AS profit,
    verid
   FROM ( SELECT sum(account.credit) AS credits,
            account.verid
           FROM public.account
          GROUP BY account.verid
          ORDER BY account.verid) datas
  ORDER BY credits;


ALTER VIEW public.e2q_risk_profit_count_list OWNER TO dbuser;

--
-- Name: e2q_risk_profit_count_row; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_risk_profit_count_row AS
 SELECT json_each_text.key,
    (json_each_text.value)::integer AS value
   FROM (( SELECT row_to_json(t.*) AS line
           FROM public.e2q_risk_profit_count t) r
     CROSS JOIN LATERAL json_each_text(r.line) json_each_text(key, value));


ALTER VIEW public.e2q_risk_profit_count_row OWNER TO dbuser;

--
-- Name: e2q_symbol_pool; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_symbol_pool AS
 SELECT quantid,
    count,
    verid,
    stock,
    trader_number,
    (count - trader_number) AS trading_number
   FROM ( SELECT ings.quantid,
            ings.count,
            ings.verid,
            ings.stock,
            ( SELECT ends.count
                   FROM ( SELECT DISTINCT ON (trend.quantid) trend.quantid,
                            count(trend.id) AS count,
                            siend.verid,
                            siend.stock,
                            (trend.side - 1) AS stat
                           FROM public.trades trend,
                            public.stockinfo siend
                          WHERE ((trend.side = 2) AND (trend.stat = 2) AND (siend.id = trend.symbol))
                          GROUP BY trend.quantid, siend.verid, siend.stock, trend.side) ends
                  WHERE (ends.quantid = ings.quantid)
                 LIMIT 1) AS trader_number
           FROM ( SELECT DISTINCT ON (tr.quantid) tr.quantid,
                    count(tr.id) AS count,
                    si.verid,
                    si.stock,
                    (tr.side - 1) AS stat
                   FROM public.trades tr,
                    public.stockinfo si
                  WHERE ((tr.side = 1) AND (tr.stat = 2) AND (si.id = tr.symbol))
                  GROUP BY tr.quantid, si.verid, si.stock, tr.side) ings) data;


ALTER VIEW public.e2q_symbol_pool OWNER TO dbuser;

--
-- Name: e2q_symbol_status; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_symbol_status AS
 SELECT id,
    stock,
    sday,
    status,
    verid
   FROM ( SELECT stockinfo.id,
            stockinfo.stock,
            to_char(to_timestamp((stockinfo.ctime)::double precision), 'YYYY-MM-DD HH:MI:SS'::text) AS sday,
            '交易'::text AS status,
            stockinfo.verid
           FROM public.stockinfo
          WHERE (stockinfo.symbol > 0)
        UNION
         SELECT stockinfo.id,
            stockinfo.stock,
                CASE
                    WHEN (stockinfo.dtime > 0) THEN to_char(to_timestamp((stockinfo.dtime)::double precision), 'YYYY-MM-DD HH:MI:SS'::text)
                    ELSE to_char(now(), 'YYYY-MM-DD HH:MI:SS'::text)
                END AS sday,
            '退出'::text AS status,
            stockinfo.verid
           FROM public.stockinfo
          WHERE (stockinfo.symbol > 0)) data;


ALTER VIEW public.e2q_symbol_status OWNER TO dbuser;

--
-- Name: e2q_trade_detail; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_trade_detail AS
 SELECT buy.id,
    ( SELECT stockinfo.verid
           FROM public.stockinfo
          WHERE (buy.symbol = stockinfo.id)
         LIMIT 1) AS verid,
    ( SELECT stockinfo.symbol
           FROM public.stockinfo
          WHERE (stockinfo.id = buy.symbol)
         LIMIT 1) AS symbol,
    ( SELECT stockinfo.stock
           FROM public.stockinfo
          WHERE (stockinfo.id = buy.symbol)
         LIMIT 1) AS stock,
    buy.price AS open_price,
    buy.qty AS open_qty,
    to_char(to_timestamp(((buy.ctime / 1000))::double precision), 'YYYY/MM/DD'::text) AS open_time,
    (buy.ticket)::text AS ticket,
    buy.amount,
    (buy.quantid)::text AS quantid,
    ana.name,
    ana.argv
   FROM public.trades buy,
    public.analse ana
  WHERE ((buy.side = 1) AND (ana.quantid = buy.quantid) AND (NOT (buy.ticket IN ( SELECT trades.ticket
           FROM public.trades
          WHERE ((trades.side = 1) AND (trades.stat = 2))))))
  ORDER BY buy.ctime;


ALTER VIEW public.e2q_trade_detail OWNER TO dbuser;

--
-- Name: quotes; Type: TABLE; Schema: public; Owner: dbuser
--

CREATE TABLE public.quotes (
    code text,
    active1 bigint,
    price double precision,
    last_close double precision,
    open double precision,
    high double precision,
    low double precision,
    servertime text,
    vol bigint,
    cur_vol bigint,
    amount double precision,
    s_vol bigint,
    b_vol bigint,
    bid1 double precision,
    ask1 double precision,
    bid_vol1 bigint,
    ask_vol1 bigint,
    bid2 double precision,
    ask2 double precision,
    bid_vol2 bigint,
    ask_vol2 bigint,
    bid3 double precision,
    ask3 double precision,
    bid_vol3 bigint,
    ask_vol3 bigint,
    bid4 double precision,
    ask4 double precision,
    bid_vol4 bigint,
    ask_vol4 bigint,
    bid5 double precision,
    ask5 double precision,
    bid_vol5 bigint,
    ask_vol5 bigint,
    active2 bigint,
    volume bigint,
    utime bigint,
    verid bigint
);


ALTER TABLE public.quotes OWNER TO dbuser;

--
-- Name: e2q_trading; Type: VIEW; Schema: public; Owner: dbuser
--

CREATE VIEW public.e2q_trading AS
 SELECT buy.id,
    sk.verid,
    sk.symbol,
    sk.stock,
    buy.price AS open_price,
    COALESCE(( SELECT quotes.price
           FROM public.quotes
          WHERE ((quotes.verid = sk.verid) AND (quotes.code = (sk.stock)::text))
         LIMIT 1), (0)::double precision) AS price,
    buy.qty AS open_qty,
    to_char(to_timestamp(((buy.ctime / 1000))::double precision), 'YYYY/MM/DD'::text) AS open_time,
    (buy.ticket)::text AS ticket,
    buy.amount,
    (buy.quantid)::text AS quantid,
    ana.name,
    ana.argv,
    round(((((sk.adj_price - buy.adjpx) / buy.adjpx) * (100.0)::double precision))::numeric, 3) AS profit
   FROM public.trades buy,
    public.analse ana,
    public.stockinfo sk
  WHERE ((buy.side = 1) AND (buy.stat = 2) AND (NOT (buy.ticket IN ( SELECT trades.closetck
           FROM public.trades
          WHERE ((trades.side = 2) AND (trades.stat = 2))))) AND (ana.quantid = buy.quantid) AND (sk.id = buy.symbol))
  ORDER BY buy.ctime;


ALTER VIEW public.e2q_trading OWNER TO dbuser;

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


ALTER SEQUENCE public.ohlc_id_seq OWNER TO dbuser;

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
-- Name: todos_id_seq; Type: SEQUENCE; Schema: public; Owner: dbuser
--

ALTER TABLE public.stockinfo ALTER COLUMN id ADD GENERATED BY DEFAULT AS IDENTITY (
    SEQUENCE NAME public.todos_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


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


ALTER SEQUENCE public.trades_id_seq OWNER TO dbuser;

--
-- Name: trades_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: dbuser
--

ALTER SEQUENCE public.trades_id_seq OWNED BY public.trades.id;


--
-- Name: symbol_risk; Type: TABLE; Schema: trade_status; Owner: dbuser
--

CREATE TABLE trade_status.symbol_risk (
    id integer NOT NULL,
    symbol character varying(20),
    start_time timestamp without time zone,
    now_time timestamp without time zone,
    risk double precision,
    verid integer
);


ALTER TABLE trade_status.symbol_risk OWNER TO dbuser;

--
-- Name: TABLE symbol_risk; Type: COMMENT; Schema: trade_status; Owner: dbuser
--

COMMENT ON TABLE trade_status.symbol_risk IS '记录不同版本当前所有symbol的 risk 的值';


--
-- Name: COLUMN symbol_risk.symbol; Type: COMMENT; Schema: trade_status; Owner: dbuser
--

COMMENT ON COLUMN trade_status.symbol_risk.symbol IS 'symbol';


--
-- Name: COLUMN symbol_risk.start_time; Type: COMMENT; Schema: trade_status; Owner: dbuser
--

COMMENT ON COLUMN trade_status.symbol_risk.start_time IS 'risk start time';


--
-- Name: COLUMN symbol_risk.now_time; Type: COMMENT; Schema: trade_status; Owner: dbuser
--

COMMENT ON COLUMN trade_status.symbol_risk.now_time IS 'risk now';


--
-- Name: COLUMN symbol_risk.risk; Type: COMMENT; Schema: trade_status; Owner: dbuser
--

COMMENT ON COLUMN trade_status.symbol_risk.risk IS 'value';


--
-- Name: COLUMN symbol_risk.verid; Type: COMMENT; Schema: trade_status; Owner: dbuser
--

COMMENT ON COLUMN trade_status.symbol_risk.verid IS 'public trade_info id';


--
-- Name: symbol_risk_id_seq; Type: SEQUENCE; Schema: trade_status; Owner: dbuser
--

ALTER TABLE trade_status.symbol_risk ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME trade_status.symbol_risk_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


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
-- Name: analselog analselog_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.analselog
    ADD CONSTRAINT analselog_pkey PRIMARY KEY (id);


--
-- Name: analytics analytics_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.analytics
    ADD CONSTRAINT analytics_pkey PRIMARY KEY (id);


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
-- Name: stockinfo todos_pkey; Type: CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.stockinfo
    ADD CONSTRAINT todos_pkey PRIMARY KEY (id);


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
-- Name: symbol_risk symbol_risk_pkey; Type: CONSTRAINT; Schema: trade_status; Owner: dbuser
--

ALTER TABLE ONLY trade_status.symbol_risk
    ADD CONSTRAINT symbol_risk_pkey PRIMARY KEY (id);


--
-- Name: ctime_1731564753404_index; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE INDEX ctime_1731564753404_index ON public.analselog USING btree (ctime);


--
-- Name: idx_1731564760831_index; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE INDEX idx_1731564760831_index ON public.analselog USING btree (type);


--
-- Name: index_ohlc_on_key_and_date; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE UNIQUE INDEX index_ohlc_on_key_and_date ON public.ohlc USING btree (key, date);


--
-- Name: key_1746496119188_index; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE INDEX key_1746496119188_index ON public.analselog USING btree (key);


--
-- Name: quantid_1748398520595_index; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE UNIQUE INDEX quantid_1748398520595_index ON public.analse USING btree (quantid);


--
-- Name: stat_price_1746496685474_index; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE INDEX stat_price_1746496685474_index ON public.trades USING btree (stat, price);


--
-- Name: stat_stoppx_1746496694259_index; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE INDEX stat_stoppx_1746496694259_index ON public.trades USING btree (stat, stoppx);


--
-- Name: ticket_1748400505161_index; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE INDEX ticket_1748400505161_index ON public.trades USING btree (ticket);


--
-- Name: verid_1780469364352_index; Type: INDEX; Schema: public; Owner: dbuser
--

CREATE INDEX verid_1780469364352_index ON public.quotes USING btree (verid);


--
-- Name: account account_sessionid_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.account
    ADD CONSTRAINT account_sessionid_fkey FOREIGN KEY (sessionid) REFERENCES public.fixsession(id);


--
-- Name: account account_verid_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.account
    ADD CONSTRAINT account_verid_fkey FOREIGN KEY (verid) REFERENCES public.trade_info(id);


--
-- Name: analse analse_verid_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.analse
    ADD CONSTRAINT analse_verid_fkey FOREIGN KEY (verid) REFERENCES public.trade_info(id);


--
-- Name: analselog analselog_quantid_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.analselog
    ADD CONSTRAINT analselog_quantid_fkey FOREIGN KEY (quantid) REFERENCES public.analse(quantid);


--
-- Name: exdr exdr_symbol_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.exdr
    ADD CONSTRAINT exdr_symbol_fkey FOREIGN KEY (symbol) REFERENCES public.stockinfo(id);


--
-- Name: stockinfo stockinfo_verid_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.stockinfo
    ADD CONSTRAINT stockinfo_verid_fkey FOREIGN KEY (verid) REFERENCES public.trade_info(id);


--
-- Name: trade_report trade_report_sessionid_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.trade_report
    ADD CONSTRAINT trade_report_sessionid_fkey FOREIGN KEY (sessionid) REFERENCES public.fixsession(id);


--
-- Name: trade_report trade_report_ticket_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.trade_report
    ADD CONSTRAINT trade_report_ticket_fkey FOREIGN KEY (ticket) REFERENCES public.trades(id);


--
-- Name: trades trades_quantid_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.trades
    ADD CONSTRAINT trades_quantid_fkey FOREIGN KEY (quantid) REFERENCES public.analse(quantid);


--
-- Name: trades trades_symbol_fkey; Type: FK CONSTRAINT; Schema: public; Owner: dbuser
--

ALTER TABLE ONLY public.trades
    ADD CONSTRAINT trades_symbol_fkey FOREIGN KEY (symbol) REFERENCES public.stockinfo(id);


--
-- PostgreSQL database dump complete
--

\unrestrict UeGOHlELTdQxZjXyqZla9Uylq2goncq9Ucf6DFxMDqcV4BYZMcrH10aN1Xafxy2

