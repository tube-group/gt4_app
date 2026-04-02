-- public.api_tube_data_t definition

-- Drop table

-- DROP TABLE public.api_tube_data_t;

CREATE TABLE public.api_tube_data_t (
	order_no varchar(10) NOT NULL,
	item_no varchar(3) NOT NULL,
	bundle_no varchar(7) NOT NULL,
	melt_no varchar(8) NOT NULL,
	lot_no varchar(7) DEFAULT '',
	weight numeric(7, 3) DEFAULT 0,
	length numeric(6, 3) DEFAULT 0,
	tubeno_ht int4 NOT NULL,
	produce_time varchar(19) DEFAULT '',
	short_flag varchar(1) DEFAULT '0',
	ban_ci varchar(2) DEFAULT '',
	del_flag varchar(1) DEFAULT '',
	del_flag2 varchar(1) NULL,
	tube_no int4 DEFAULT 0,
	CONSTRAINT pk_api_tube_data_t PRIMARY KEY (order_no, item_no, tubeno_ht)
);


-- 为各个字段添加注释
COMMENT ON COLUMN public.api_tube_data_t.order_no IS '合同号';
COMMENT ON COLUMN public.api_tube_data_t.item_no IS '项目号';
COMMENT ON COLUMN public.api_tube_data_t.bundle_no IS '管捆号';
COMMENT ON COLUMN public.api_tube_data_t.melt_no IS '炉号';
COMMENT ON COLUMN public.api_tube_data_t.lot_no IS '试批号';
COMMENT ON COLUMN public.api_tube_data_t.weight IS '米制重量';
--COMMENT ON COLUMN public.api_tube_data_t.length IS '';
--COMMENT ON COLUMN public.api_tube_data_t.tubeno_ht IS '';
COMMENT ON COLUMN public.api_tube_data_t.produce_time IS '生产时间';
--COMMENT ON COLUMN public.api_tube_data_t.short_flag IS '';
COMMENT ON COLUMN public.api_tube_data_t.ban_ci IS '班组';
--COMMENT ON COLUMN public.api_tube_data_t.del_flag IS '';
--COMMENT ON COLUMN public.api_tube_data_t.del_flag2 IS '';
--COMMENT ON COLUMN public.api_tube_data_t.tube_no IS '';