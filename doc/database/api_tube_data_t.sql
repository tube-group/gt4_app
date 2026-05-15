-- public.api_tube_data_t definition

-- DROP TABLE public.api_tube_data_t;

CREATE TABLE public.api_tube_data_t (
	order_no varchar(10) NOT NULL,
	item_no varchar(3) NOT NULL,
	bundle_no varchar(7) NOT NULL,
	weight numeric(7, 3) DEFAULT 0,
	length numeric(6, 3) DEFAULT 0,
	flow_no int4 NOT NULL,
	tube_no int4 DEFAULT 0,

	CONSTRAINT pk_api_tube_data_t PRIMARY KEY (order_no, item_no, flow_no)
);


-- 为各个字段添加注释
COMMENT ON COLUMN public.api_tube_data_t.order_no IS '合同号';
COMMENT ON COLUMN public.api_tube_data_t.item_no IS '项目号';
COMMENT ON COLUMN public.api_tube_data_t.bundle_no IS '管捆号';
COMMENT ON COLUMN public.api_tube_data_t.weight IS '米制重量';
COMMENT ON COLUMN public.api_tube_data_t.length IS '米制长度';
COMMENT ON COLUMN public.api_tube_data_t.flow_no IS '流水号';
COMMENT ON COLUMN public.api_tube_data_t.tube_no IS '管号';

