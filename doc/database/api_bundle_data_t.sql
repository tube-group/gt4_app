-- public.api_bundle_data_t definition

-- Drop table

-- DROP TABLE public.api_bundle_data_t;

-- 创建表
CREATE TABLE public.api_bundle_data_t (
	order_no varchar(10) NOT NULL,
	item_no varchar(3) NOT NULL,
	bundle_no varchar(7) NOT NULL,
	roll_no varchar(6) DEFAULT '',
	melt_no varchar(8) DEFAULT '',
	lot_no varchar(7) DEFAULT '',
	prod_code varchar(1) DEFAULT '',
	prod_cname varchar(30) DEFAULT '',
	mat_no varchar(4) DEFAULT '',
	mat_text varchar(20) DEFAULT '',
	std_sg_code varchar(4) DEFAULT '',
	std_text varchar(100) DEFAULT '',
	sg_text varchar(50) DEFAULT '',
	diameter numeric(6, 2) DEFAULT 0,
	wall_thickness numeric(5, 2) DEFAULT 0,
	weight numeric(7, 3) DEFAULT 0,
	weight_eng numeric(7, 3) DEFAULT 0,
	total_length numeric(7, 3) DEFAULT 0,
	length_eng numeric(7, 3) DEFAULT 0,
	length_from numeric(7, 3) DEFAULT 0,
	length_to numeric(7, 3) DEFAULT 0,
	tube int DEFAULT 0,
	bundle_type varchar(3) DEFAULT '',
	produce_time varchar(14) DEFAULT '',
	ban_ci varchar(2) DEFAULT '',
	hr_ban_ci varchar(2) DEFAULT '',
	product_job_point varchar(4) DEFAULT 'T031',
	direction_code varchar(4) DEFAULT '',
	theory_weight int DEFAULT 0,
	theory_total_length numeric(7, 3) DEFAULT 0,
	last_flow_no int DEFAULT 0,
	end_type_code varchar(1) DEFAULT '',
	end_type_sign varchar(6) DEFAULT '',
	thread_type_code varchar(1) DEFAULT '',
	thread_type_sign varchar(12) DEFAULT '',
	coupling_type_code varchar(1) DEFAULT '',
	coupling_type_sign varchar(12) DEFAULT '',
	pono_id_coupling varchar(8) DEFAULT '',
	lot_no_thread varchar(7) DEFAULT '',
	male_pono_id_coupling varchar(6) DEFAULT '',
	male_lot_no_thread varchar(7) DEFAULT '',
	female_pono_id_coupling varchar(6) DEFAULT '',
	female_lot_no_thread varchar(7) DEFAULT '',
	seam_lot_no varchar(7) DEFAULT '',
	order_no_old varchar(10) DEFAULT '',
	toc varchar(19) DEFAULT '',
	send_flag varchar(1) DEFAULT '0',
	report_flag varchar(1) DEFAULT '0',
	del_flag varchar(1) DEFAULT '',
	gross_weight int DEFAULT 0,
	end_type varchar(12) DEFAULT '',
	thread_type varchar(30) DEFAULT '',
	diameter_down_ctrl numeric(6, 3) DEFAULT 0,
	diameter_up_ctrl numeric(6, 3) DEFAULT 0,
	wal_thick_down_ctrl numeric(5, 2) DEFAULT 0,
	wal_thick_up_ctrl numeric(5, 2) DEFAULT 0,
	weight_per_meter numeric(6, 2) DEFAULT 0,
	weight_ew numeric(5, 2) DEFAULT 0,
	room_no varchar(5) DEFAULT '',
	CONSTRAINT pk_api_bundle_data_t PRIMARY KEY (order_no, item_no, bundle_no)
);


-- 为各个字段添加注释
-- 画面中有的字段
COMMENT ON COLUMN public.api_bundle_data_t.order_no IS '合同号';
COMMENT ON COLUMN public.api_bundle_data_t.item_no IS '项目号';
COMMENT ON COLUMN public.api_bundle_data_t.bundle_no IS '管捆号';
COMMENT ON COLUMN public.api_bundle_data_t.roll_no IS '轧批号';
COMMENT ON COLUMN public.api_bundle_data_t.melt_no IS '炉号';
COMMENT ON COLUMN public.api_bundle_data_t.lot_no IS '试批号';
COMMENT ON COLUMN public.api_bundle_data_t.diameter IS '外径 ';
COMMENT ON COLUMN public.api_bundle_data_t.wall_thickness IS '壁厚 ';
COMMENT ON COLUMN public.api_bundle_data_t.length_from IS '最短';
COMMENT ON COLUMN public.api_bundle_data_t.length_to IS '最长';
COMMENT ON COLUMN public.api_bundle_data_t.tube IS '根数';
COMMENT ON COLUMN public.api_bundle_data_t.last_flow_no IS '最后管号';
COMMENT ON COLUMN public.api_bundle_data_t.produce_time IS '生产时间';
COMMENT ON COLUMN public.api_bundle_data_t.bundle_type IS '管捆状态';
COMMENT ON COLUMN public.api_bundle_data_t.ban_ci IS '班组';
COMMENT ON COLUMN public.api_bundle_data_t.end_type IS '管端型式';
COMMENT ON COLUMN public.api_bundle_data_t.product_job_point IS '作业点代码';
COMMENT ON COLUMN public.api_bundle_data_t.direction_code IS '去向代码';
COMMENT ON COLUMN public.api_bundle_data_t.theory_weight IS '理论重量';
COMMENT ON COLUMN public.api_bundle_data_t.theory_total_length IS '理论长度';
COMMENT ON COLUMN public.api_bundle_data_t.weight IS '米制重量';
COMMENT ON COLUMN public.api_bundle_data_t.weight_eng IS '英制重量';
COMMENT ON COLUMN public.api_bundle_data_t.total_length IS '米制长度';
COMMENT ON COLUMN public.api_bundle_data_t.length_eng IS '英制长度';
COMMENT ON COLUMN public.api_bundle_data_t.mat_text IS '材质正文';
COMMENT ON COLUMN public.api_bundle_data_t.std_text IS '标准正文';
COMMENT ON COLUMN public.api_bundle_data_t.sg_text IS '钢级正文';
COMMENT ON COLUMN public.api_bundle_data_t.thread_type_sign IS '管端类型符号';
COMMENT ON COLUMN public.api_bundle_data_t.end_type_sign IS '螺纹类型符号';
COMMENT ON COLUMN public.api_bundle_data_t.pono_id_coupling IS '接箍炉号';
COMMENT ON COLUMN public.api_bundle_data_t.lot_no_thread IS '接箍批号';
COMMENT ON COLUMN public.api_bundle_data_t.thread_type IS '螺纹类型';

-- 从class1.cs里找到的对应字段
COMMENT ON COLUMN public.api_bundle_data_t.prod_code IS '品名细分类代码';
COMMENT ON COLUMN public.api_bundle_data_t.prod_cname IS '品名细分类';
COMMENT ON COLUMN public.api_bundle_data_t.mat_no IS '材质号';
COMMENT ON COLUMN public.api_bundle_data_t.end_type_code IS '螺纹类型代码';
COMMENT ON COLUMN public.api_bundle_data_t.thread_type_code IS '管端类型代码';
COMMENT ON COLUMN public.api_bundle_data_t.coupling_type_code IS '接箍类型代码';
COMMENT ON COLUMN public.api_bundle_data_t.coupling_type_sign IS '接箍类型符号';
COMMENT ON COLUMN public.api_bundle_data_t.std_sg_code IS '标准钢级代码';
COMMENT ON COLUMN public.api_bundle_data_t.seam_lot_no IS '焊缝试批号';
COMMENT ON COLUMN public.api_bundle_data_t.order_no_old IS '原合同号';
COMMENT ON COLUMN public.api_bundle_data_t.toc IS '时间';
COMMENT ON COLUMN public.api_bundle_data_t.send_flag IS '发送标记';


-- 完全没找到对应的字段
COMMENT ON COLUMN public.api_bundle_data_t.hr_ban_ci IS '';
COMMENT ON COLUMN public.api_bundle_data_t.male_pono_id_coupling IS '';
COMMENT ON COLUMN public.api_bundle_data_t.male_lot_no_thread IS '';
COMMENT ON COLUMN public.api_bundle_data_t.female_pono_id_coupling IS '';
COMMENT ON COLUMN public.api_bundle_data_t.female_lot_no_thread IS '';
COMMENT ON COLUMN public.api_bundle_data_t.report_flag IS '';
COMMENT ON COLUMN public.api_bundle_data_t.del_flag IS '删除标志';
COMMENT ON COLUMN public.api_bundle_data_t.gross_weight IS '毛重';
COMMENT ON COLUMN public.api_bundle_data_t.diameter_down_ctrl IS '外径下限_内控';
COMMENT ON COLUMN public.api_bundle_data_t.diameter_up_ctrl IS '外径上限_内控';
COMMENT ON COLUMN public.api_bundle_data_t.wal_thick_down_ctrl IS '壁厚下限_内控';
COMMENT ON COLUMN public.api_bundle_data_t.wal_thick_up_ctrl IS '壁厚上限_内控';
COMMENT ON COLUMN public.api_bundle_data_t.weight_per_meter IS '米重';
COMMENT ON COLUMN public.api_bundle_data_t.weight_ew IS '称重要求螺纹和接箍EW';
COMMENT ON COLUMN public.api_bundle_data_t.room_no IS '库位号';