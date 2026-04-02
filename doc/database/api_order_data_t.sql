-- api_order_data_t definition

-- Drop table

-- DROP TABLE api_order_data_t;

CREATE TABLE api_order_data_t (
	order_no varchar(10) NOT NULL,
	item_no varchar(3) NOT NULL,
	roll_no varchar(6) DEFAULT '',
	diameter numeric(6, 2) DEFAULT 0,
	wall_thickness numeric(5, 2) DEFAULT 0,
	prod_code varchar(1) DEFAULT '',
	prod_cname varchar(30) DEFAULT '',
	heat_treat_code varchar(1) DEFAULT '',
	heat_treat_text varchar(40) DEFAULT '',
	std_sg_code varchar(4) DEFAULT '',
	std_text varchar(100) DEFAULT '',
	sg_text varchar(50) DEFAULT '',
	mat_no varchar(4) DEFAULT '',
	mat_text varchar(20) DEFAULT '',
	thread_type_code varchar(1) DEFAULT '',
	thread_type_sign varchar(12) DEFAULT '',
	end_type_code varchar(1) DEFAULT '',
	end_type_sign varchar(6) DEFAULT '',
	coupling_type_code varchar(1) DEFAULT '',
	coupling_type_sign varchar(12) DEFAULT '',
	thread_face_treat_mode_code varchar(1) DEFAULT '',
	thread_face_treat_mode varchar(20) DEFAULT '',
	length_from numeric(7, 3) DEFAULT 0,
	length_to numeric(7, 3) DEFAULT 0,
	order_unit_code varchar(2) DEFAULT '',
	order_unit varchar(4) DEFAULT '',
	order_qty numeric(10, 3) DEFAULT 0,
	order_tube int DEFAULT 0,
	order_weight numeric(10, 3) DEFAULT 0,
	fixed_order_weight numeric(10, 3) DEFAULT 0,
	unfixed_order_weight numeric(10, 3) DEFAULT 0,
	delivery_tolerance_code varchar(2) DEFAULT '',
	delivery_tolerance_unit varchar(4) DEFAULT '',
	delivery_tolerance_from int DEFAULT 0,
	delivery_tolerance_to int DEFAULT 0,
	short_rate int DEFAULT 0,
	short_from numeric(5, 2) DEFAULT 0,
	short_to numeric(5, 2) DEFAULT 0,
	single_bundle_weight_max int DEFAULT 0,
	single_bundle_tube_max int DEFAULT 0,
	oil_code varchar(1) DEFAULT '',
	oil_type varchar(50) DEFAULT '',
	stamp_req varchar(300) DEFAULT '',
	stencil_req varchar(400) DEFAULT '',
	label_req_1 varchar(50) DEFAULT '',
	label_req_2 varchar(50) DEFAULT '',
	label_req_3 varchar(50) DEFAULT '',
	label_req_4 varchar(50) DEFAULT '',
	label_req_5 varchar(50) DEFAULT '',
	label_req_6 varchar(50) DEFAULT '',
	label_req_7 varchar(50) DEFAULT '',
	label_req_8 varchar(50) DEFAULT '',
	qual_special_req varchar(100) DEFAULT '',
	produce_special_req varchar(100) DEFAULT '',
	std_pressure_mpa numeric(5, 2) DEFAULT 0,
	std_pressure_psi numeric(8, 2) DEFAULT 0,
	stabilivolt_time_min int DEFAULT 0,
	anneal_flag varchar(1) DEFAULT '',
	weight_per_meter numeric(6, 2) DEFAULT 0,
	weight_ew numeric(5, 2) DEFAULT 0,
	theory_weight_eng numeric(6, 2) DEFAULT 0,
	order_no_old varchar(10) DEFAULT '',
	color_circle varchar(50) DEFAULT '',
	color_circle_pos varchar(20) DEFAULT '',
	finish_number int DEFAULT 0,
	finish_length numeric(10, 3) DEFAULT 0,
	finish_weight numeric(10, 3) DEFAULT 0,
	finish_number_sh int DEFAULT 0,
	finish_length_sh numeric(10, 3) DEFAULT 0,
	finish_weight_sh numeric(10, 3) DEFAULT 0,
	last_flow_no int DEFAULT 0,
	stencil_req_manual varchar(400) DEFAULT '',
	label_req_1_manual varchar(50) DEFAULT '',
	label_req_2_manual varchar(50) DEFAULT '',
	label_req_3_manual varchar(50) DEFAULT '',
	label_req_4_manual varchar(50) DEFAULT '',
	label_req_5_manual varchar(50) DEFAULT '',
	label_req_6_manual varchar(50) DEFAULT '',
	label_req_7_manual varchar(50) DEFAULT '',
	label_req_8_manual varchar(50) DEFAULT '',
	colour varchar(4) DEFAULT '',
	toc varchar(19) DEFAULT '',
	select_flag varchar(1) DEFAULT '0',
	order_qty_l2 numeric(10, 3) DEFAULT 0,
	rolling_diameter numeric(6, 2) DEFAULT 0,
	rolling_thickness numeric(5, 2) DEFAULT 0,
	weight_mode_code varchar(1) DEFAULT '',
	weight_mode_text varchar(4) DEFAULT '',
	p_weight_tolerance_ul numeric(5, 2) DEFAULT 0,
	p_weight_tolerance_ll numeric(5, 2) DEFAULT 0,
	weight_req_flatside_wpe numeric(6, 2) DEFAULT 0,
	control_lenght_ul numeric(7, 3) DEFAULT 0,
	control_lenght_ll numeric(7, 3) DEFAULT 0,
	label_length_type int DEFAULT 0 ,
	label_weight_type int DEFAULT 0 ,
	label_type int DEFAULT 0,
	height numeric(6, 2) DEFAULT 0,
	end_type varchar(12) DEFAULT '',
	coupling_type varchar(30) DEFAULT '',
	mic_no varchar(6) DEFAULT '',
	diameter_down_ctrl numeric(6, 3) DEFAULT 0,
	diameter_up_ctrl numeric(6, 3) DEFAULT 0,
	wal_thick_down_ctrl numeric(5, 2) DEFAULT 0,
	wal_thick_up_ctrl numeric(5, 2) DEFAULT 0,
	height_down_ctrl numeric(6, 2) DEFAULT 0,
	height_up_ctrlv numeric(6, 2) DEFAULT 0,
	length_grade_code varchar(1) DEFAULT '',
	length_grade varchar(20) DEFAULT '',
	thread_type varchar(30) DEFAULT '',
	stamp_req_1_manual varchar(80) DEFAULT '',
	stamp_req_2_manual varchar(80) DEFAULT '',
	stamp_req_3_manual varchar(80) DEFAULT '',
	stamp_req_4_manual varchar(80) DEFAULT '',
	stamp_req_5_manual varchar(80) DEFAULT '',
	stamp_req_6_manual varchar(80) DEFAULT '',
	stamp_req_7_manual varchar(80) DEFAULT '',
	stamp_req_8_manual varchar(80) DEFAULT '',		
	CONSTRAINT pk_api_order_data_t PRIMARY KEY (order_no, item_no)
);


-- 为各个字段添加注释
COMMENT ON COLUMN api_order_data_t.order_no IS '合同号';
COMMENT ON COLUMN api_order_data_t.item_no IS '项目号';
COMMENT ON COLUMN api_order_data_t.roll_no IS '轧批号';
COMMENT ON COLUMN api_order_data_t.diameter IS '外径';
COMMENT ON COLUMN api_order_data_t.wall_thickness IS '壁厚';
COMMENT ON COLUMN api_order_data_t.prod_code IS '品名细分类代码';
COMMENT ON COLUMN api_order_data_t.prod_cname IS '品名细分类';
COMMENT ON COLUMN api_order_data_t.heat_treat_code IS '热处理方式代码';
COMMENT ON COLUMN api_order_data_t.heat_treat_text IS '热处理方式正文';
COMMENT ON COLUMN api_order_data_t.std_sg_code IS '标准钢级代码';
COMMENT ON COLUMN api_order_data_t.std_text IS '标准正文';
COMMENT ON COLUMN api_order_data_t.sg_text IS '钢级正文';
COMMENT ON COLUMN api_order_data_t.mat_no IS '材质号';
COMMENT ON COLUMN api_order_data_t.mat_text IS '材质正文';
COMMENT ON COLUMN api_order_data_t.thread_type_code IS '管端类型代码';
COMMENT ON COLUMN api_order_data_t.thread_type_sign IS '管端类型符号';
COMMENT ON COLUMN api_order_data_t.end_type_code IS '螺纹类型代码';
COMMENT ON COLUMN api_order_data_t.end_type_sign IS '螺纹类型符号';
COMMENT ON COLUMN api_order_data_t.coupling_type_code IS '接箍类型代码';
COMMENT ON COLUMN api_order_data_t.coupling_type_sign IS '接箍类型符号';
COMMENT ON COLUMN api_order_data_t.thread_face_treat_mode_code IS '螺纹表面处理方式代码';
COMMENT ON COLUMN api_order_data_t.thread_face_treat_mode IS '螺纹表面处理方式';
COMMENT ON COLUMN api_order_data_t.length_from IS '订货长度起';
COMMENT ON COLUMN api_order_data_t.length_to IS '订货长度止';
COMMENT ON COLUMN api_order_data_t.order_unit_code IS '订货计量单位代码';
COMMENT ON COLUMN api_order_data_t.order_unit IS '订货计量单位';
COMMENT ON COLUMN api_order_data_t.order_qty IS '订货数量';
COMMENT ON COLUMN api_order_data_t.order_tube IS '订货根数';
COMMENT ON COLUMN api_order_data_t.order_weight IS '订货重量';
COMMENT ON COLUMN api_order_data_t.fixed_order_weight IS '定尺订货重量';
COMMENT ON COLUMN api_order_data_t.unfixed_order_weight IS '非定尺订货重量';
COMMENT ON COLUMN api_order_data_t.delivery_tolerance_code IS '提货公差单位代码';
COMMENT ON COLUMN api_order_data_t.delivery_tolerance_unit IS '提货公差单位';
COMMENT ON COLUMN api_order_data_t.delivery_tolerance_from IS '提货公差起';
COMMENT ON COLUMN api_order_data_t.delivery_tolerance_to IS '提货公差至';
COMMENT ON COLUMN api_order_data_t.short_rate IS '短尺率';
COMMENT ON COLUMN api_order_data_t.short_from IS '短尺长度起';
COMMENT ON COLUMN api_order_data_t.short_to IS '短尺长度至';
COMMENT ON COLUMN api_order_data_t.single_bundle_weight_max IS '最大单捆重量';
COMMENT ON COLUMN api_order_data_t.single_bundle_tube_max IS '最大单捆根数';
COMMENT ON COLUMN api_order_data_t.oil_code IS '涂油代码';
COMMENT ON COLUMN api_order_data_t.oil_type IS '涂油正文';
COMMENT ON COLUMN api_order_data_t.stamp_req IS '压印要求';
COMMENT ON COLUMN api_order_data_t.stencil_req IS '喷印要求';
COMMENT ON COLUMN api_order_data_t.label_req_1 IS '标签要求1';
COMMENT ON COLUMN api_order_data_t.label_req_2 IS '标签要求2';
COMMENT ON COLUMN api_order_data_t.label_req_3 IS '标签要求3';
COMMENT ON COLUMN api_order_data_t.label_req_4 IS '标签要求4';
COMMENT ON COLUMN api_order_data_t.label_req_5 IS '标签要求5';
COMMENT ON COLUMN api_order_data_t.label_req_6 IS '标签要求6';
COMMENT ON COLUMN api_order_data_t.label_req_7 IS '标签要求7';
COMMENT ON COLUMN api_order_data_t.label_req_8 IS '标签要求8';
COMMENT ON COLUMN api_order_data_t.qual_special_req IS '生产特殊要求';
COMMENT ON COLUMN api_order_data_t.produce_special_req IS '质量特殊要求';
COMMENT ON COLUMN api_order_data_t.std_pressure_mpa IS '标准水压压力（MPA)';
COMMENT ON COLUMN api_order_data_t.std_pressure_psi IS '标准水压压力 (PSI)';
COMMENT ON COLUMN api_order_data_t.stabilivolt_time_min IS '最小稳压时间';
COMMENT ON COLUMN api_order_data_t.anneal_flag IS '退火标志';
COMMENT ON COLUMN api_order_data_t.weight_per_meter IS '米重';
COMMENT ON COLUMN api_order_data_t.weight_ew IS 'EW值';
COMMENT ON COLUMN api_order_data_t.theory_weight_eng IS '名义重量';
COMMENT ON COLUMN api_order_data_t.order_no_old IS '原合同号';
COMMENT ON COLUMN api_order_data_t.color_circle IS '色环';
COMMENT ON COLUMN api_order_data_t.color_circle_pos IS '色环位置';
COMMENT ON COLUMN api_order_data_t.finish_number IS '完成根数';
COMMENT ON COLUMN api_order_data_t.finish_length IS '完成长度';
COMMENT ON COLUMN api_order_data_t.finish_weight IS '完成重量';
COMMENT ON COLUMN api_order_data_t.finish_number_sh IS '短尺完成根数';
COMMENT ON COLUMN api_order_data_t.finish_length_sh IS '短尺完成长度';
COMMENT ON COLUMN api_order_data_t.finish_weight_sh IS '短尺完成重量';
COMMENT ON COLUMN api_order_data_t.last_flow_no IS '最后管号';
COMMENT ON COLUMN api_order_data_t.stencil_req_manual IS '人工喷印要求';
COMMENT ON COLUMN api_order_data_t.label_req_1_manual IS '人工自由格式打印要求1';
COMMENT ON COLUMN api_order_data_t.label_req_2_manual IS '人工自由格式打印要求2';
COMMENT ON COLUMN api_order_data_t.label_req_3_manual IS '人工自由格式打印要求3';
COMMENT ON COLUMN api_order_data_t.label_req_4_manual IS '人工自由格式打印要求4';
COMMENT ON COLUMN api_order_data_t.label_req_5_manual IS '人工自由格式打印要求5';
COMMENT ON COLUMN api_order_data_t.label_req_6_manual IS '人工自由格式打印要求6';
COMMENT ON COLUMN api_order_data_t.label_req_7_manual IS '人工自由格式打印要求7';
COMMENT ON COLUMN api_order_data_t.label_req_8_manual IS '人工自由格式打印要求8';
COMMENT ON COLUMN api_order_data_t.colour IS '标签颜色';
COMMENT ON COLUMN api_order_data_t.toc IS '时间';
COMMENT ON COLUMN api_order_data_t.select_flag IS '选择标志: 0-未选, 1-已选';
COMMENT ON COLUMN api_order_data_t.order_qty_l2 IS 'L2订货数量';
COMMENT ON COLUMN api_order_data_t.rolling_diameter IS '轧制外径';
COMMENT ON COLUMN api_order_data_t.rolling_thickness IS '轧制壁厚';
COMMENT ON COLUMN api_order_data_t.weight_mode_code IS '计重方式代码';
COMMENT ON COLUMN api_order_data_t.weight_mode_text IS '计重方式正文';
COMMENT ON COLUMN api_order_data_t.p_weight_tolerance_ul IS '单根重量允差上限';
COMMENT ON COLUMN api_order_data_t.p_weight_tolerance_ll IS '单根重量允差下限';
COMMENT ON COLUMN api_order_data_t.weight_req_flatside_wpe IS '称重要求平端WPE';
COMMENT ON COLUMN api_order_data_t.control_lenght_ul IS '内控长度上限';
COMMENT ON COLUMN api_order_data_t.control_lenght_ll IS '内控长度下限';
COMMENT ON COLUMN api_order_data_t.label_length_type IS '实验室长度类型: 0-无, 1-有';
COMMENT ON COLUMN api_order_data_t.label_weight_type IS '实验室重量类型: 0-无, 1-有';
COMMENT ON COLUMN api_order_data_t.label_type IS 'EMF标签类型: 0-无, 1-有';
COMMENT ON COLUMN api_order_data_t.height IS '高度 (mm)';
COMMENT ON COLUMN api_order_data_t.end_type IS '端部类型描述';
COMMENT ON COLUMN api_order_data_t.coupling_type IS '接箍类型描述';
COMMENT ON COLUMN api_order_data_t.mic_no IS 'MIC编号';
COMMENT ON COLUMN api_order_data_t.diameter_down_ctrl IS '外径下控制线 (mm)';
COMMENT ON COLUMN api_order_data_t.diameter_up_ctrl IS '外径上控制线 (mm)';
COMMENT ON COLUMN api_order_data_t.wal_thick_down_ctrl IS '壁厚下控制线 (mm)';
COMMENT ON COLUMN api_order_data_t.wal_thick_up_ctrl IS '壁厚上控制线 (mm)';
COMMENT ON COLUMN api_order_data_t.height_down_ctrl IS '高度下控制线 (mm)';
COMMENT ON COLUMN api_order_data_t.height_up_ctrlv IS '高度上控制线 (mm)';
COMMENT ON COLUMN api_order_data_t.length_grade_code IS '长度等级编码';
COMMENT ON COLUMN api_order_data_t.length_grade IS '长度等级描述';
COMMENT ON COLUMN api_order_data_t.thread_type IS '螺纹类型描述';
COMMENT ON COLUMN stamp_req_1_manual.thread_type IS '针刻印要求1';
COMMENT ON COLUMN stamp_req_2_manual.thread_type IS '针刻印要求2';
COMMENT ON COLUMN stamp_req_3_manual.thread_type IS '针刻印要求3';
COMMENT ON COLUMN stamp_req_4_manual.thread_type IS '针刻印要求4';
COMMENT ON COLUMN stamp_req_5_manual.thread_type IS '针刻印要求5';
COMMENT ON COLUMN stamp_req_6_manual.thread_type IS '针刻印要求6';
COMMENT ON COLUMN stamp_req_7_manual.thread_type IS '针刻印要求7';
COMMENT ON COLUMN stamp_req_8_manual.thread_type IS '针刻印要求8';