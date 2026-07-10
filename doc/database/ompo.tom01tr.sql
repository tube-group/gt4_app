-- ompo.tom01tr 定义

-- Drop table

-- DROP TABLE ompo.tom01tr;

CREATE TABLE ompo.tom01tr (
	rec_creator varchar(24) DEFAULT ' '::character varying NOT NULL, -- 记录创建责任者
	rec_create_time varchar(14) DEFAULT ' '::character varying NOT NULL, -- 记录创建时刻
	rec_revisor varchar(24) DEFAULT ' '::character varying NOT NULL, -- 记录修改责任者
	rec_revise_time varchar(14) DEFAULT ' '::character varying NOT NULL, -- 记录修改时刻
	archive_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 归档标记
	archive_stamp_no varchar(10) DEFAULT ' '::character varying NOT NULL, -- 归档邮戳号
	order_no varchar(10) DEFAULT ' '::character varying NOT NULL, -- 合同号
	make_method_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 制造方法代码
	make_method varchar(50) DEFAULT ' '::character varying NOT NULL, -- 制造方法
	mtrl_no varchar(4) DEFAULT ' '::character varying NOT NULL, -- 材质号
	mtrl_text varchar(50) DEFAULT ' '::character varying NOT NULL, -- 材质正文
	st_no varchar(20) DEFAULT ' '::character varying NOT NULL, -- 出钢记号
	steel_making_method_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 冶炼方式代码
	steel_making_method varchar(60) DEFAULT ' '::character varying NOT NULL, -- 冶炼方式
	pipeend_type_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 管端型式代码
	pipeend_type_sign varchar(12) DEFAULT ' '::character varying NOT NULL, -- 管端型式符号
	pipeend_type varchar(30) DEFAULT ' '::character varying NOT NULL, -- 管端型式
	thread_type_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 螺纹类型代码
	thread_type_sign varchar(12) DEFAULT ' '::character varying NOT NULL, -- 螺纹类型符号
	thread_type varchar(30) DEFAULT ' '::character varying NOT NULL, -- 螺纹类型
	coup_type_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 接箍类型代码
	coup_type_sign varchar(12) DEFAULT ' '::character varying NOT NULL, -- 接箍类型符号
	coup_type varchar(30) DEFAULT ' '::character varying NOT NULL, -- 接箍类型
	coup_num int4 DEFAULT 0 NOT NULL, -- 接箍只数
	coup_wt numeric(10, 3) DEFAULT 0 NOT NULL, -- 接箍重量
	coup_wt_single numeric(6, 2) DEFAULT 0 NOT NULL, -- 接箍单只重量
	len_grade_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 长度等级代码
	len_grade varchar(20) DEFAULT ' '::character varying NOT NULL, -- 长度等级
	gradient_type_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 坡口类型代码
	gradient_type varchar(40) DEFAULT ' '::character varying NOT NULL, -- 坡口类型
	protect_sleevec_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 保护套代码
	protect_sleeve varchar(20) DEFAULT ' '::character varying NOT NULL, -- 保护套
	protect_sleeve_num int4 DEFAULT 0 NOT NULL, -- 保护套只数
	in_protect_ring_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 内保护环代码
	in_protect_ring varchar(40) DEFAULT ' '::character varying NOT NULL, -- 内保护环
	in_protect_ring_num int4 DEFAULT 0 NOT NULL, -- 内保护环只数
	out_protect_ring_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 外保护环代码
	out_protect_ring varchar(40) DEFAULT ' '::character varying NOT NULL, -- 外保护环
	out_protect_ring_num int4 DEFAULT 0 NOT NULL, -- 外保护环只数
	accu_grade_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 精度等级代码
	accu_grade varchar(30) DEFAULT ' '::character varying NOT NULL, -- 精度等级
	hot_treat_method_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 热处理方式代码
	hot_treat_method varchar(40) DEFAULT ' '::character varying NOT NULL, -- 热处理方式
	oil_kind_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 涂油种类代码
	oil_kind varchar(50) DEFAULT ' '::character varying NOT NULL, -- 涂油种类
	makeup_thread_compound_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 拧接螺纹脂代码
	makeup_thread_compound varchar(20) DEFAULT ' '::character varying NOT NULL, -- 拧接螺纹脂
	in_thread_compound_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内保螺纹脂代码
	in_thread_compound varchar(20) DEFAULT ' '::character varying NOT NULL, -- 内保螺纹脂
	out_thread_compound_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 外保螺纹脂代码
	out_thread_compound varchar(20) DEFAULT ' '::character varying NOT NULL, -- 外保螺纹脂
	thread_face_treat_mode_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 螺纹表面处理方式代码
	thread_face_treat_mode varchar(30) DEFAULT ' '::character varying NOT NULL, -- 螺纹表面处理方式
	joint_type_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 工具接头类型代码
	joint_type varchar(30) DEFAULT ' '::character varying NOT NULL, -- 工具接头类型
	joint_type_sign varchar(12) DEFAULT ' '::character varying NOT NULL, -- 工具接头类型符号
	tape_type_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 台肩类型代码
	tape_type varchar(30) DEFAULT ' '::character varying NOT NULL, -- 台肩类型
	spread_weld_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 硬质合金堆焊代码
	spread_weld varchar(30) DEFAULT ' '::character varying NOT NULL, -- 硬质合金堆焊
	cust_std_code varchar(4) DEFAULT ' '::character varying NOT NULL, -- 用户标准代码
	cust_std varchar(100) DEFAULT ' '::character varying NOT NULL, -- 用户标准
	prod_mid_code varchar(5) DEFAULT ' '::character varying NOT NULL, -- 品名中分类代码
	prod_mid_cname varchar(30) DEFAULT ' '::character varying NOT NULL, -- 品名中分类中文
	prod_wide_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 品名大分类代码
	prod_wide_cname varchar(30) DEFAULT ' '::character varying NOT NULL, -- 品名大分类中文
	stencil_position varchar(100) DEFAULT ' '::character varying NOT NULL, -- 喷印位置
	stencil_req varchar(400) DEFAULT ' '::character varying NOT NULL, -- 喷印要求
	other_stencil_req varchar(200) DEFAULT ' '::character varying NOT NULL, -- 另端喷印要求
	stamp_position varchar(50) DEFAULT ' '::character varying NOT NULL, -- 压印位置
	stamp_req varchar(300) DEFAULT ' '::character varying NOT NULL, -- 压印要求
	stamp_depth numeric(3, 2) DEFAULT 0 NOT NULL, -- 压印深度
	stamp_letter_height numeric(4, 2) DEFAULT 0 NOT NULL, -- 压印字高
	pack_mode_code varchar(4) DEFAULT ' '::character varying NOT NULL, -- 包装方式代码
	pack_mode_cname varchar(30) DEFAULT ' '::character varying NOT NULL, -- 包装方式中文
	pack_mode_ename varchar(40) DEFAULT ' '::character varying NOT NULL, -- 包装方式英文
	pack_gate int4 DEFAULT 0 NOT NULL, -- 捆扎道次
	single_bundle_tube_max int4 DEFAULT 0 NOT NULL, -- 单捆最大根数
	single_bundle_wt_max numeric(10, 3) DEFAULT 0 NOT NULL, -- 单捆最大重量
	wt_per_meter numeric(10, 5) DEFAULT 0 NOT NULL, -- 米重
	dim_specs varchar(50) DEFAULT ' '::character varying NOT NULL, -- 尺寸规格
	order_fix_wt numeric(10, 3) DEFAULT 0 NOT NULL, -- 定尺重量订货
	order_unfix_wt numeric(10, 3) DEFAULT 0 NOT NULL, -- 订货非定尺重量
	wt_l_eng numeric(6, 2) DEFAULT 0 NOT NULL, -- 名义重量英制
	len_metric_or_eng_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 长度公英制标志
	spec_metric_or_eng_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 规格公英制标志
	stencil_wt_eng_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 重量喷印单位公英制标志
	stencil_len_eng_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 长度喷印单位公英制标志
	stencil_spec_eng_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 规格喷印单位公英制标志
	certi_str_eng_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 质保书强度公英制标志
	certi_press_eng_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 质保书压力公英制标志
	lable_color varchar(20) DEFAULT ' '::character varying NOT NULL, -- 标签颜色
	lable_medium varchar(20) DEFAULT ' '::character varying NOT NULL, -- 标签介质
	lable_num int4 DEFAULT 0 NOT NULL, -- 标签数量
	lable_req_1 varchar(50) DEFAULT ' '::character varying NOT NULL, -- 标签要求1
	lable_req_2 varchar(50) DEFAULT ' '::character varying NOT NULL, -- 标签要求2
	lable_req_3 varchar(50) DEFAULT ' '::character varying NOT NULL, -- 标签要求3
	lable_req_4 varchar(50) DEFAULT ' '::character varying NOT NULL, -- 标签要求4
	lable_req_5 varchar(50) DEFAULT ' '::character varying NOT NULL, -- 标签要求5
	lable_req_6 varchar(50) DEFAULT ' '::character varying NOT NULL, -- 标签要求6
	lable_req_7 varchar(50) DEFAULT ' '::character varying NOT NULL, -- 标签要求7
	lable_req_8 varchar(50) DEFAULT ' '::character varying NOT NULL, -- 标签要求8
	special_tol_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 特殊公差标记
	end_dia_tol_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 管端外径公差标志
	wal_thick_tol_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 壁厚公差标志
	height_tol_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 高度公差标志
	in_dia_tol_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内径公差标志
	len_tol_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 长度公差标志
	tube_dia_tol_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 管体外径公差标志
	india_thick_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 订货内径壁厚标志
	pj_add_len_type_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 公接头加长类型代码
	pj_add_len_type varchar(30) DEFAULT ' '::character varying NOT NULL, -- 公接头加长类型中文
	pj_add_len_type_ename varchar(40) DEFAULT ' '::character varying NOT NULL, -- 公接头加长类型英文
	bj_add_len_type_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 母接头加长类型代码
	bj_add_len_type varchar(30) DEFAULT ' '::character varying NOT NULL, -- 母接头加长类型中文
	bj_add_len_type_ename varchar(40) DEFAULT ' '::character varying NOT NULL, -- 母接头加长类型英文
	pj_out_dia numeric(6, 2) DEFAULT 0 NOT NULL, -- 公接头外径
	pj_inner_dia numeric(5, 2) DEFAULT 0 NOT NULL, -- 公接头内径
	pj_wt numeric(10, 3) DEFAULT 0 NOT NULL, -- 公接头重量
	pj_wt_single numeric(6, 2) DEFAULT 0 NOT NULL, -- 公接头单只重量
	pj_num int4 DEFAULT 0 NOT NULL, -- 公接头只数
	bj_out_dia numeric(6, 2) DEFAULT 0 NOT NULL, -- 母接头外径
	bj_inner_dia numeric(6, 2) DEFAULT 0 NOT NULL, -- 母接头内径
	bj_wt numeric(10, 3) DEFAULT 0 NOT NULL, -- 母接头重量
	bj_wt_single numeric(6, 2) DEFAULT 0 NOT NULL, -- 母接头单只重量
	bj_num int4 DEFAULT 0 NOT NULL, -- 母接头只数
	pass_end_dia_tol_max numeric(6, 3) DEFAULT 0 NOT NULL, -- 放行管端外径公差上限
	pass_end_dia_tol_max_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行管端外径公差上限单位
	pass_end_dia_tol_min numeric(6, 3) DEFAULT 0 NOT NULL, -- 放行管端外径公差下限
	pass_end_dia_tol_min_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行管端外径公差下限单位
	pass_end_dia_from numeric(7, 3) DEFAULT 0 NOT NULL, -- 放行管端外径下限
	pass_end_dia_to numeric(7, 3) DEFAULT 0 NOT NULL, -- 放行管端外径上限
	pass_wal_thick_tol_max numeric(5, 2) DEFAULT 0 NOT NULL, -- 放行壁厚公差最大值
	pass_wal_thick_tol_max_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行壁厚公差最大值单位
	pass_wal_thick_tol_min numeric(5, 2) DEFAULT 0 NOT NULL, -- 放行壁厚公差最小值
	pass_wal_thick_tol_min_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行壁厚公差最小值单位
	pass_wal_thick_from numeric(6, 3) DEFAULT 0 NOT NULL, -- 放行壁厚起
	pass_wal_thick_to numeric(6, 3) DEFAULT 0 NOT NULL, -- 放行壁厚止
	pass_inner_dia_tol_max numeric(6, 3) DEFAULT 0 NOT NULL, -- 放行内径公差最大值
	pass_inner_dia_tol_max_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行内径公差最大值单位
	pass_inner_dia_tol_min numeric(6, 3) DEFAULT 0 NOT NULL, -- 放行内径公差最小值
	pass_inner_dia_tol_min_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行内径公差最小值单位
	pass_inner_dia_from numeric(7, 3) DEFAULT 0 NOT NULL, -- 放行内径起
	pass_inner_dia_to numeric(7, 3) DEFAULT 0 NOT NULL, -- 放行内径止
	pass_len_tol_max numeric(12, 3) DEFAULT 0 NOT NULL, -- 放行长度公差最大值
	pass_len_tol_max_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行长度公差最大值单位
	pass_len_tol_min numeric(12, 3) DEFAULT 0 NOT NULL, -- 放行长度公差最小值
	pass_len_tol_min_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行长度公差最小值单位
	pass_len_from numeric(12, 3) DEFAULT 0 NOT NULL, -- 放行长度起
	pass_len_to numeric(12, 3) DEFAULT 0 NOT NULL, -- 放行长度止
	pass_tube_dia_tol_max numeric(6, 3) DEFAULT 0 NOT NULL, -- 放行管体外径公差上限
	pass_tube_dia_tol_max_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行管体外径公差上限单位
	pass_tube_dia_tol_min numeric(6, 3) DEFAULT 0 NOT NULL, -- 放行管体外径公差下限
	pass_tube_dia_tol_min_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行管体外径公差下限单位
	pass_tube_dia_from numeric(7, 3) DEFAULT 0 NOT NULL, -- 放行管体外径下限
	pass_tube_dia_to numeric(7, 3) DEFAULT 0 NOT NULL, -- 放行管体外径上限
	ctl_end_dia_tol_max numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控管端外径公差上限
	ctl_end_dia_tol_max_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控管端外径公差上限单位
	ctl_end_dia_tol_min numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控管端外径公差下限
	ctl_end_dia_tol_min_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控管端外径公差下限单位
	ctl_end_dia_from numeric(7, 3) DEFAULT 0 NOT NULL, -- 内控管端外径起
	ctl_end_dia_to numeric(7, 3) DEFAULT 0 NOT NULL, -- 内控管端外径止
	ctl_end_dia_extreme numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控管端外径极差
	ctl_wal_thick_tol_max numeric(5, 2) DEFAULT 0 NOT NULL, -- 内控壁厚公差最大值
	ctl_wal_thick_tol_max_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控壁厚公差最大值单位
	ctl_wal_thick_tol_min numeric(5, 2) DEFAULT 0 NOT NULL, -- 内控壁厚公差最小值
	ctl_wal_thick_tol_min_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控壁厚公差最小值单位
	ctl_wal_thick_from numeric(5, 2) DEFAULT 0 NOT NULL, -- 内控壁厚起
	ctl_wal_thick_to numeric(5, 2) DEFAULT 0 NOT NULL, -- 内控壁厚止
	ctl_wal_thick_extreme numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控壁厚极差
	ctl_inner_dia_tol_max numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控内径公差最大值
	ctl_inner_dia_tol_max_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控内径公差最大值单位
	ctl_inner_dia_tol_min numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控内径公差最小值
	ctl_inner_dia_tol_min_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控内径公差最小值单位
	ctl_inner_dia_from numeric(7, 3) DEFAULT 0 NOT NULL, -- 内控内径起
	ctl_inner_dia_to numeric(7, 3) DEFAULT 0 NOT NULL, -- 内控内径止
	ctl_end_inner_dia_extreme numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控管端内径极差
	ctl_tube_inner_dia_extreme numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控管体内径极差
	ctl_len_tol_max numeric(12, 3) DEFAULT 0 NOT NULL, -- 内控长度公差最大值
	ctl_len_tol_max_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控长度公差最大值单位
	ctl_len_tol_min numeric(12, 3) DEFAULT 0 NOT NULL, -- 内控长度公差最小值
	ctl_len_tol_min_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控长度公差最小值单位
	ctl_len_from numeric(12, 3) DEFAULT 0 NOT NULL, -- 内控长度起
	ctl_len_to numeric(12, 3) DEFAULT 0 NOT NULL, -- 内控长度止
	ctl_len_var_max numeric(5, 2) DEFAULT 0 NOT NULL, -- 内控最大长度变量
	ctl_tube_dia_tol_max numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控管体外径公差上限
	ctl_tube_dia_tol_max_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控管体外径公差上限单位
	ctl_tube_dia_tol_min numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控管体外径公差下限
	ctl_tube_dia_tol_min_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控管体外径公差下限单位
	ctl_tube_dia_from numeric(7, 3) DEFAULT 0 NOT NULL, -- 内控管体外径起
	ctl_tube_dia_to numeric(7, 3) DEFAULT 0 NOT NULL, -- 内控管体外径止
	ctl_tube_dia_extreme numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控管体外径极差
	certi_notes varchar(200) DEFAULT ' '::character varying NOT NULL, -- 质保书备注
	certi_expl_cname varchar(100) DEFAULT ' '::character varying NOT NULL, -- 质保书叙述_中文
	certi_expl_ename varchar(240) DEFAULT ' '::character varying NOT NULL, -- 质保书叙述_英文
	kill_method_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 镇静方法代码
	kill_method varchar(80) DEFAULT ' '::character varying NOT NULL, -- 镇静方法
	licence_no_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 许可证编号代码
	licence_no varchar(100) DEFAULT ' '::character varying NOT NULL, -- 许可证编号
	licence_type varchar(50) DEFAULT ' '::character varying NOT NULL, -- 许可证类型
	licence_no_2 varchar(100) DEFAULT ' '::character varying NOT NULL, -- 许可证编号2
	cpc varchar(4) DEFAULT ' '::character varying NOT NULL, -- 接箍参数码
	jpc varchar(8) DEFAULT ' '::character varying NOT NULL, -- 工具接头参数码
	new_prod_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 新试产品代码
	new_prod_agree_no varchar(20) DEFAULT ' '::character varying NOT NULL, -- 新试产品加工协议号
	new_prod_desc varchar(30) DEFAULT ' '::character varying NOT NULL, -- 新试产品描述
	image_method varchar(10) DEFAULT ' '::character varying NOT NULL, -- 成像方式
	tube_detect_rate int4 DEFAULT 0 NOT NULL, -- 全管探伤比例
	in_thread_type_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内螺纹类型代码
	in_thread_type varchar(30) DEFAULT ' '::character varying NOT NULL, -- 内螺纹类型
	in_coat_std_code varchar(4) DEFAULT ' '::character varying NOT NULL, -- 内涂层标准代码
	in_coat_cust_std_code varchar(4) DEFAULT ' '::character varying NOT NULL, -- 内涂层用户标准代码
	in_coat_str_code varchar(3) DEFAULT ' '::character varying NOT NULL, -- 内涂层结构代码
	out_coat_std_code varchar(4) DEFAULT ' '::character varying NOT NULL, -- 外涂层标准代码
	out_coat_cust_std_code varchar(4) DEFAULT ' '::character varying NOT NULL, -- 外涂层用户标准代码
	out_coat_str_code varchar(3) DEFAULT ' '::character varying NOT NULL, -- 外涂层结构代码
	out_coat_temp_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 外涂层温度类型代码
	out_coat_thick_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 外涂层厚度级别代码
	stencil_req_inside varchar(400) DEFAULT ' '::character varying NOT NULL, -- 内侧喷印要求
	other_stencil_req_inside varchar(180) DEFAULT ' '::character varying NOT NULL, -- 内侧另端喷印要求
	in_stencil_position varchar(100) DEFAULT ' '::character varying NOT NULL, -- 内侧喷印位置
	produce_special_req varchar(100) DEFAULT ' '::character varying NOT NULL, -- 生产特殊要求
	current_rl_no varchar(10) DEFAULT ' '::character varying NOT NULL, -- 当前轧批号
	curr_proc_order_no varchar(10) DEFAULT ' '::character varying NOT NULL, -- 当前加工合同号
	hot_plan_date varchar(8) DEFAULT ' '::character varying NOT NULL, -- 热区计划日期
	cool_plan_date varchar(8) DEFAULT ' '::character varying NOT NULL, -- 冷区计划日期
	cool_backlog_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 冷区第一道工序
	billet_sg_sign varchar(50) DEFAULT ' '::character varying NOT NULL, -- 管坯牌号
	billet_psr varchar(20) DEFAULT ' '::character varying NOT NULL, -- 管坯PSR
	billet_apn varchar(4) DEFAULT ' '::character varying NOT NULL, -- 管坯APN
	billet_dia numeric(5, 2) DEFAULT 0 NOT NULL, -- 管坯直径
	billet_strip_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 管坯剥皮方式代码
	billet_strip varchar(20) DEFAULT ' '::character varying NOT NULL, -- 管坯剥皮方式
	billet_proc_mode_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 管坯加工方式代码
	billet_proc_mode varchar(20) DEFAULT ' '::character varying NOT NULL, -- 管坯加工方式
	sub_groove_group_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 子孔型分类代码
	steel_class varchar(2) DEFAULT ' '::character varying NOT NULL, -- 钢种分类
	steel_grade_class varchar(20) DEFAULT ' '::character varying NOT NULL, -- 钢种分类正文
	special_remark varchar(100) DEFAULT ' '::character varying NOT NULL, -- 特殊说明
	ou_lot_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 组批方法标志
	pass_height_tol_min numeric(7, 3) DEFAULT 0 NOT NULL, -- 放行高度公差最小值
	pass_height_tol_min_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行高度公差最小值单位
	pass_height_tol_max_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行高度公差最大值单位
	pass_height_tol_max numeric(7, 3) DEFAULT 0 NOT NULL, -- 放行高度公差最大值
	pass_height_to numeric(7, 3) DEFAULT 0 NOT NULL, -- 放行高度止
	pass_height_from numeric(7, 3) DEFAULT 0 NOT NULL, -- 放行高度起
	ctl_height_tol_min numeric(7, 3) DEFAULT 0 NOT NULL, -- 内控高度公差最小值
	ctl_height_tol_min_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控高度公差最小值单位
	ctl_height_tol_max numeric(7, 3) DEFAULT 0 NOT NULL, -- 内控高度公差最大值
	ctl_height_tol_max_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控高度公差最大值单位
	ctl_height_from numeric(7, 3) DEFAULT 0 NOT NULL, -- 内控高度起
	ctl_height_to numeric(7, 3) DEFAULT 0 NOT NULL, -- 内控高度止
	ctl_height_extreme numeric(7, 3) DEFAULT 0 NOT NULL, -- 内控高度极差
	ctl_angle_radius_down numeric(5, 2) DEFAULT 0 NOT NULL, -- 内控圆角半径下限
	ctl_angle_radius_up numeric(5, 2) DEFAULT 0 NOT NULL, -- 内控圆角半径上限
	ctl_angle_radius_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控圆角半径单位
	ctl_unsmooth_down numeric(5, 2) DEFAULT 0 NOT NULL, -- 内控凹凸度下限
	ctl_unsmooth_up numeric(5, 2) DEFAULT 0 NOT NULL, -- 内控凹凸度上限
	ctl_unsmooth_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控凹凸度单位
	ctl_twist_up numeric(5, 2) DEFAULT 0 NOT NULL, -- 内控扭转度上限
	ctl_lean_up numeric(5, 2) DEFAULT 0 NOT NULL, -- 内控倾斜度上限
	pass_angle_radius_down numeric(5, 2) DEFAULT 0 NOT NULL, -- 放行圆角半径下限
	pass_angle_radius_up numeric(5, 2) DEFAULT 0 NOT NULL, -- 放行圆角半径上限
	pass_angle_radius_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行圆角半径单位
	pass_unsmooth_down numeric(5, 2) DEFAULT 0 NOT NULL, -- 放行凹凸度下限
	pass_unsmooth_up numeric(5, 2) DEFAULT 0 NOT NULL, -- 放行凹凸度上限
	pass_unsmooth_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 放行凹凸度单位
	pass_twist_up numeric(5, 2) DEFAULT 0 NOT NULL, -- 放行扭转度上限
	pass_lean_up numeric(5, 2) DEFAULT 0 NOT NULL, -- 放行倾斜度上限
	ctl_two_dia_diff numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控两端外径差
	ctl_two_dia_diff_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控两端外径差单位
	ctl_two_in_dia_tol numeric(6, 3) DEFAULT 0 NOT NULL, -- 内控两端内径差
	ctl_two_in_dia_tol_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 内控两端内径差单位
	gradient_type_in varchar(40) DEFAULT ' '::character varying NOT NULL, -- 坡口类型描述2_坡口角度
	gradient_type_thick varchar(40) DEFAULT ' '::character varying NOT NULL, -- 坡口类型描述3_钝边厚度
	in_coat_cust_std varchar(80) DEFAULT ' '::character varying NOT NULL, -- 内涂层用户标准
	out_coat_cust_std varchar(80) DEFAULT ' '::character varying NOT NULL, -- 外涂层用户标准
	in_coat_std varchar(50) DEFAULT ' '::character varying NOT NULL, -- 内涂层标准
	out_coat_std varchar(50) DEFAULT ' '::character varying NOT NULL, -- 外涂层标准
	in_coat_str varchar(60) DEFAULT ' '::character varying NOT NULL, -- 内涂层结构
	out_coat_str varchar(60) DEFAULT ' '::character varying NOT NULL, -- 外涂层结构
	out_coat_temp varchar(20) DEFAULT ' '::character varying NOT NULL, -- 外涂层温度类型
	out_coat_thick varchar(20) DEFAULT ' '::character varying NOT NULL, -- 外涂层厚度级别
	in_coat_std_with_version varchar(60) DEFAULT ' '::character varying NOT NULL, -- 内涂层标准（含版本）
	out_coat_std_with_version varchar(60) DEFAULT ' '::character varying NOT NULL, -- 外涂层标准（含版本）
	coat_no_in varchar(6) DEFAULT ' '::character varying NOT NULL, -- 内涂层参数码
	coat_no_out varchar(6) DEFAULT ' '::character varying NOT NULL, -- 外涂层参数码
	order_item_no varchar(3) DEFAULT ' '::character varying NOT NULL, -- 合同项目号
	double_high_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 双高产品代码
	double_high_desc varchar(30) DEFAULT ' '::character varying NOT NULL, -- 双高产品说明
	whole_backlog_no int4 DEFAULT 0 NOT NULL, -- 制程号
	whole_backlog varchar(50) DEFAULT ' '::character varying NOT NULL, -- 全程工序途径码
	backlog varchar(17) DEFAULT ' '::character varying NOT NULL, -- 合同加工途径
	pin_jpc varchar(11) DEFAULT ' '::character varying NOT NULL, -- 工具接头产品规范码
	apn_jpc varchar(3) DEFAULT ' '::character varying NOT NULL, -- 工具接头产品最终用途码
	whole_backlog_desc varchar(200) DEFAULT ' '::character varying NOT NULL, -- 全程工序途径码说明
	plan_maker varchar(24) DEFAULT ' '::character varying NOT NULL, -- 计划责任者
	change_outer_dia numeric(6, 2) DEFAULT 0 NOT NULL, -- 转换外径
	change_wal_thick numeric(8, 3) DEFAULT 0 NOT NULL, -- 转换壁厚
	CONSTRAINT tom01tr_pk PRIMARY KEY (order_no)
)
WITH (
	orientation=row,
	compression=no,
	storage_type=ustore,
	segment=off
);
COMMENT ON TABLE ompo.tom01tr IS '合同主档表钢管子表';

-- Column comments

COMMENT ON COLUMN ompo.tom01tr.rec_creator IS '记录创建责任者';
COMMENT ON COLUMN ompo.tom01tr.rec_create_time IS '记录创建时刻';
COMMENT ON COLUMN ompo.tom01tr.rec_revisor IS '记录修改责任者';
COMMENT ON COLUMN ompo.tom01tr.rec_revise_time IS '记录修改时刻';
COMMENT ON COLUMN ompo.tom01tr.archive_flag IS '归档标记';
COMMENT ON COLUMN ompo.tom01tr.archive_stamp_no IS '归档邮戳号';
COMMENT ON COLUMN ompo.tom01tr.order_no IS '合同号';
COMMENT ON COLUMN ompo.tom01tr.make_method_code IS '制造方法代码';
COMMENT ON COLUMN ompo.tom01tr.make_method IS '制造方法';
COMMENT ON COLUMN ompo.tom01tr.mtrl_no IS '材质号';
COMMENT ON COLUMN ompo.tom01tr.mtrl_text IS '材质正文';
COMMENT ON COLUMN ompo.tom01tr.st_no IS '出钢记号';
COMMENT ON COLUMN ompo.tom01tr.steel_making_method_code IS '冶炼方式代码';
COMMENT ON COLUMN ompo.tom01tr.steel_making_method IS '冶炼方式';
COMMENT ON COLUMN ompo.tom01tr.pipeend_type_code IS '管端型式代码';
COMMENT ON COLUMN ompo.tom01tr.pipeend_type_sign IS '管端型式符号';
COMMENT ON COLUMN ompo.tom01tr.pipeend_type IS '管端型式';
COMMENT ON COLUMN ompo.tom01tr.thread_type_code IS '螺纹类型代码';
COMMENT ON COLUMN ompo.tom01tr.thread_type_sign IS '螺纹类型符号';
COMMENT ON COLUMN ompo.tom01tr.thread_type IS '螺纹类型';
COMMENT ON COLUMN ompo.tom01tr.coup_type_code IS '接箍类型代码';
COMMENT ON COLUMN ompo.tom01tr.coup_type_sign IS '接箍类型符号';
COMMENT ON COLUMN ompo.tom01tr.coup_type IS '接箍类型';
COMMENT ON COLUMN ompo.tom01tr.coup_num IS '接箍只数';
COMMENT ON COLUMN ompo.tom01tr.coup_wt IS '接箍重量';
COMMENT ON COLUMN ompo.tom01tr.coup_wt_single IS '接箍单只重量';
COMMENT ON COLUMN ompo.tom01tr.len_grade_code IS '长度等级代码';
COMMENT ON COLUMN ompo.tom01tr.len_grade IS '长度等级';
COMMENT ON COLUMN ompo.tom01tr.gradient_type_code IS '坡口类型代码';
COMMENT ON COLUMN ompo.tom01tr.gradient_type IS '坡口类型';
COMMENT ON COLUMN ompo.tom01tr.protect_sleevec_code IS '保护套代码';
COMMENT ON COLUMN ompo.tom01tr.protect_sleeve IS '保护套';
COMMENT ON COLUMN ompo.tom01tr.protect_sleeve_num IS '保护套只数';
COMMENT ON COLUMN ompo.tom01tr.in_protect_ring_code IS '内保护环代码';
COMMENT ON COLUMN ompo.tom01tr.in_protect_ring IS '内保护环';
COMMENT ON COLUMN ompo.tom01tr.in_protect_ring_num IS '内保护环只数';
COMMENT ON COLUMN ompo.tom01tr.out_protect_ring_code IS '外保护环代码';
COMMENT ON COLUMN ompo.tom01tr.out_protect_ring IS '外保护环';
COMMENT ON COLUMN ompo.tom01tr.out_protect_ring_num IS '外保护环只数';
COMMENT ON COLUMN ompo.tom01tr.accu_grade_code IS '精度等级代码';
COMMENT ON COLUMN ompo.tom01tr.accu_grade IS '精度等级';
COMMENT ON COLUMN ompo.tom01tr.hot_treat_method_code IS '热处理方式代码';
COMMENT ON COLUMN ompo.tom01tr.hot_treat_method IS '热处理方式';
COMMENT ON COLUMN ompo.tom01tr.oil_kind_code IS '涂油种类代码';
COMMENT ON COLUMN ompo.tom01tr.oil_kind IS '涂油种类';
COMMENT ON COLUMN ompo.tom01tr.makeup_thread_compound_code IS '拧接螺纹脂代码';
COMMENT ON COLUMN ompo.tom01tr.makeup_thread_compound IS '拧接螺纹脂';
COMMENT ON COLUMN ompo.tom01tr.in_thread_compound_code IS '内保螺纹脂代码';
COMMENT ON COLUMN ompo.tom01tr.in_thread_compound IS '内保螺纹脂';
COMMENT ON COLUMN ompo.tom01tr.out_thread_compound_code IS '外保螺纹脂代码';
COMMENT ON COLUMN ompo.tom01tr.out_thread_compound IS '外保螺纹脂';
COMMENT ON COLUMN ompo.tom01tr.thread_face_treat_mode_code IS '螺纹表面处理方式代码';
COMMENT ON COLUMN ompo.tom01tr.thread_face_treat_mode IS '螺纹表面处理方式';
COMMENT ON COLUMN ompo.tom01tr.joint_type_code IS '工具接头类型代码';
COMMENT ON COLUMN ompo.tom01tr.joint_type IS '工具接头类型';
COMMENT ON COLUMN ompo.tom01tr.joint_type_sign IS '工具接头类型符号';
COMMENT ON COLUMN ompo.tom01tr.tape_type_code IS '台肩类型代码';
COMMENT ON COLUMN ompo.tom01tr.tape_type IS '台肩类型';
COMMENT ON COLUMN ompo.tom01tr.spread_weld_code IS '硬质合金堆焊代码';
COMMENT ON COLUMN ompo.tom01tr.spread_weld IS '硬质合金堆焊';
COMMENT ON COLUMN ompo.tom01tr.cust_std_code IS '用户标准代码';
COMMENT ON COLUMN ompo.tom01tr.cust_std IS '用户标准';
COMMENT ON COLUMN ompo.tom01tr.prod_mid_code IS '品名中分类代码';
COMMENT ON COLUMN ompo.tom01tr.prod_mid_cname IS '品名中分类中文';
COMMENT ON COLUMN ompo.tom01tr.prod_wide_code IS '品名大分类代码';
COMMENT ON COLUMN ompo.tom01tr.prod_wide_cname IS '品名大分类中文';
COMMENT ON COLUMN ompo.tom01tr.stencil_position IS '喷印位置';
COMMENT ON COLUMN ompo.tom01tr.stencil_req IS '喷印要求';
COMMENT ON COLUMN ompo.tom01tr.other_stencil_req IS '另端喷印要求';
COMMENT ON COLUMN ompo.tom01tr.stamp_position IS '压印位置';
COMMENT ON COLUMN ompo.tom01tr.stamp_req IS '压印要求';
COMMENT ON COLUMN ompo.tom01tr.stamp_depth IS '压印深度';
COMMENT ON COLUMN ompo.tom01tr.stamp_letter_height IS '压印字高';
COMMENT ON COLUMN ompo.tom01tr.pack_mode_code IS '包装方式代码';
COMMENT ON COLUMN ompo.tom01tr.pack_mode_cname IS '包装方式中文';
COMMENT ON COLUMN ompo.tom01tr.pack_mode_ename IS '包装方式英文';
COMMENT ON COLUMN ompo.tom01tr.pack_gate IS '捆扎道次';
COMMENT ON COLUMN ompo.tom01tr.single_bundle_tube_max IS '单捆最大根数';
COMMENT ON COLUMN ompo.tom01tr.single_bundle_wt_max IS '单捆最大重量';
COMMENT ON COLUMN ompo.tom01tr.wt_per_meter IS '米重';
COMMENT ON COLUMN ompo.tom01tr.dim_specs IS '尺寸规格';
COMMENT ON COLUMN ompo.tom01tr.order_fix_wt IS '订货定尺重量';
COMMENT ON COLUMN ompo.tom01tr.order_unfix_wt IS '订货非定尺重量';
COMMENT ON COLUMN ompo.tom01tr.wt_l_eng IS '名义重量英制';
COMMENT ON COLUMN ompo.tom01tr.len_metric_or_eng_flag IS '长度公英制标志';
COMMENT ON COLUMN ompo.tom01tr.spec_metric_or_eng_flag IS '规格公英制标志';
COMMENT ON COLUMN ompo.tom01tr.stencil_wt_eng_flag IS '重量喷印单位公英制标志';
COMMENT ON COLUMN ompo.tom01tr.stencil_len_eng_flag IS '长度喷印单位公英制标志';
COMMENT ON COLUMN ompo.tom01tr.stencil_spec_eng_flag IS '规格喷印单位公英制标志';
COMMENT ON COLUMN ompo.tom01tr.certi_str_eng_flag IS '质保书强度公英制标志';
COMMENT ON COLUMN ompo.tom01tr.certi_press_eng_flag IS '质保书压力公英制标志';
COMMENT ON COLUMN ompo.tom01tr.lable_color IS '标签颜色';
COMMENT ON COLUMN ompo.tom01tr.lable_medium IS '标签介质';
COMMENT ON COLUMN ompo.tom01tr.lable_num IS '标签数量';
COMMENT ON COLUMN ompo.tom01tr.lable_req_1 IS '标签要求1';
COMMENT ON COLUMN ompo.tom01tr.lable_req_2 IS '标签要求2';
COMMENT ON COLUMN ompo.tom01tr.lable_req_3 IS '标签要求3';
COMMENT ON COLUMN ompo.tom01tr.lable_req_4 IS '标签要求4';
COMMENT ON COLUMN ompo.tom01tr.lable_req_5 IS '标签要求5';
COMMENT ON COLUMN ompo.tom01tr.lable_req_6 IS '标签要求6';
COMMENT ON COLUMN ompo.tom01tr.lable_req_7 IS '标签要求7';
COMMENT ON COLUMN ompo.tom01tr.lable_req_8 IS '标签要求8';
COMMENT ON COLUMN ompo.tom01tr.special_tol_flag IS '特殊公差标记';
COMMENT ON COLUMN ompo.tom01tr.end_dia_tol_flag IS '管端外径公差标志';
COMMENT ON COLUMN ompo.tom01tr.wal_thick_tol_flag IS '壁厚公差标志';
COMMENT ON COLUMN ompo.tom01tr.height_tol_flag IS '高度公差标志';
COMMENT ON COLUMN ompo.tom01tr.in_dia_tol_flag IS '内径公差标志';
COMMENT ON COLUMN ompo.tom01tr.len_tol_flag IS '长度公差标志';
COMMENT ON COLUMN ompo.tom01tr.tube_dia_tol_flag IS '管体外径公差标志';
COMMENT ON COLUMN ompo.tom01tr.india_thick_flag IS '订货内径壁厚标志';
COMMENT ON COLUMN ompo.tom01tr.pj_add_len_type_code IS '公接头加长类型代码';
COMMENT ON COLUMN ompo.tom01tr.pj_add_len_type IS '公接头加长类型中文';
COMMENT ON COLUMN ompo.tom01tr.pj_add_len_type_ename IS '公接头加长类型英文';
COMMENT ON COLUMN ompo.tom01tr.bj_add_len_type_code IS '母接头加长类型代码';
COMMENT ON COLUMN ompo.tom01tr.bj_add_len_type IS '母接头加长类型中文';
COMMENT ON COLUMN ompo.tom01tr.bj_add_len_type_ename IS '母接头加长类型英文';
COMMENT ON COLUMN ompo.tom01tr.pj_out_dia IS '公接头外径';
COMMENT ON COLUMN ompo.tom01tr.pj_inner_dia IS '公接头内径';
COMMENT ON COLUMN ompo.tom01tr.pj_wt IS '公接头重量';
COMMENT ON COLUMN ompo.tom01tr.pj_wt_single IS '公接头单只重量';
COMMENT ON COLUMN ompo.tom01tr.pj_num IS '公接头只数';
COMMENT ON COLUMN ompo.tom01tr.bj_out_dia IS '母接头外径';
COMMENT ON COLUMN ompo.tom01tr.bj_inner_dia IS '母接头内径';
COMMENT ON COLUMN ompo.tom01tr.bj_wt IS '母接头重量';
COMMENT ON COLUMN ompo.tom01tr.bj_wt_single IS '母接头单只重量';
COMMENT ON COLUMN ompo.tom01tr.bj_num IS '母接头只数';
COMMENT ON COLUMN ompo.tom01tr.pass_end_dia_tol_max IS '放行管端外径公差上限';
COMMENT ON COLUMN ompo.tom01tr.pass_end_dia_tol_max_unit IS '放行管端外径公差上限单位';
COMMENT ON COLUMN ompo.tom01tr.pass_end_dia_tol_min IS '放行管端外径公差下限';
COMMENT ON COLUMN ompo.tom01tr.pass_end_dia_tol_min_unit IS '放行管端外径公差下限单位';
COMMENT ON COLUMN ompo.tom01tr.pass_end_dia_from IS '放行管端外径下限';
COMMENT ON COLUMN ompo.tom01tr.pass_end_dia_to IS '放行管端外径上限';
COMMENT ON COLUMN ompo.tom01tr.pass_wal_thick_tol_max IS '放行壁厚公差最大值';
COMMENT ON COLUMN ompo.tom01tr.pass_wal_thick_tol_max_unit IS '放行壁厚公差最大值单位';
COMMENT ON COLUMN ompo.tom01tr.pass_wal_thick_tol_min IS '放行壁厚公差最小值';
COMMENT ON COLUMN ompo.tom01tr.pass_wal_thick_tol_min_unit IS '放行壁厚公差最小值单位';
COMMENT ON COLUMN ompo.tom01tr.pass_wal_thick_from IS '放行壁厚起';
COMMENT ON COLUMN ompo.tom01tr.pass_wal_thick_to IS '放行壁厚止';
COMMENT ON COLUMN ompo.tom01tr.pass_inner_dia_tol_max IS '放行内径公差最大值';
COMMENT ON COLUMN ompo.tom01tr.pass_inner_dia_tol_max_unit IS '放行内径公差最大值单位';
COMMENT ON COLUMN ompo.tom01tr.pass_inner_dia_tol_min IS '放行内径公差最小值';
COMMENT ON COLUMN ompo.tom01tr.pass_inner_dia_tol_min_unit IS '放行内径公差最小值单位';
COMMENT ON COLUMN ompo.tom01tr.pass_inner_dia_from IS '放行内径起';
COMMENT ON COLUMN ompo.tom01tr.pass_inner_dia_to IS '放行内径止';
COMMENT ON COLUMN ompo.tom01tr.pass_len_tol_max IS '放行长度公差最大值';
COMMENT ON COLUMN ompo.tom01tr.pass_len_tol_max_unit IS '放行长度公差最大值单位';
COMMENT ON COLUMN ompo.tom01tr.pass_len_tol_min IS '放行长度公差最小值';
COMMENT ON COLUMN ompo.tom01tr.pass_len_tol_min_unit IS '放行长度公差最小值单位';
COMMENT ON COLUMN ompo.tom01tr.pass_len_from IS '放行长度起';
COMMENT ON COLUMN ompo.tom01tr.pass_len_to IS '放行长度止';
COMMENT ON COLUMN ompo.tom01tr.pass_tube_dia_tol_max IS '放行管体外径公差上限';
COMMENT ON COLUMN ompo.tom01tr.pass_tube_dia_tol_max_unit IS '放行管体外径公差上限单位';
COMMENT ON COLUMN ompo.tom01tr.pass_tube_dia_tol_min IS '放行管体外径公差下限';
COMMENT ON COLUMN ompo.tom01tr.pass_tube_dia_tol_min_unit IS '放行管体外径公差下限单位';
COMMENT ON COLUMN ompo.tom01tr.pass_tube_dia_from IS '放行管体外径下限';
COMMENT ON COLUMN ompo.tom01tr.pass_tube_dia_to IS '放行管体外径上限';
COMMENT ON COLUMN ompo.tom01tr.ctl_end_dia_tol_max IS '内控管端外径公差上限';
COMMENT ON COLUMN ompo.tom01tr.ctl_end_dia_tol_max_unit IS '内控管端外径公差上限单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_end_dia_tol_min IS '内控管端外径公差下限';
COMMENT ON COLUMN ompo.tom01tr.ctl_end_dia_tol_min_unit IS '内控管端外径公差下限单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_end_dia_from IS '内控管端外径起';
COMMENT ON COLUMN ompo.tom01tr.ctl_end_dia_to IS '内控管端外径止';
COMMENT ON COLUMN ompo.tom01tr.ctl_end_dia_extreme IS '内控管端外径极差';
COMMENT ON COLUMN ompo.tom01tr.ctl_wal_thick_tol_max IS '内控壁厚公差最大值';
COMMENT ON COLUMN ompo.tom01tr.ctl_wal_thick_tol_max_unit IS '内控壁厚公差最大值单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_wal_thick_tol_min IS '内控壁厚公差最小值';
COMMENT ON COLUMN ompo.tom01tr.ctl_wal_thick_tol_min_unit IS '内控壁厚公差最小值单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_wal_thick_from IS '内控壁厚起';
COMMENT ON COLUMN ompo.tom01tr.ctl_wal_thick_to IS '内控壁厚止';
COMMENT ON COLUMN ompo.tom01tr.ctl_wal_thick_extreme IS '内控壁厚极差';
COMMENT ON COLUMN ompo.tom01tr.ctl_inner_dia_tol_max IS '内控内径公差最大值';
COMMENT ON COLUMN ompo.tom01tr.ctl_inner_dia_tol_max_unit IS '内控内径公差最大值单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_inner_dia_tol_min IS '内控内径公差最小值';
COMMENT ON COLUMN ompo.tom01tr.ctl_inner_dia_tol_min_unit IS '内控内径公差最小值单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_inner_dia_from IS '内控内径起';
COMMENT ON COLUMN ompo.tom01tr.ctl_inner_dia_to IS '内控内径止';
COMMENT ON COLUMN ompo.tom01tr.ctl_end_inner_dia_extreme IS '内控管端内径极差';
COMMENT ON COLUMN ompo.tom01tr.ctl_tube_inner_dia_extreme IS '内控管体内径极差';
COMMENT ON COLUMN ompo.tom01tr.ctl_len_tol_max IS '内控长度公差最大值';
COMMENT ON COLUMN ompo.tom01tr.ctl_len_tol_max_unit IS '内控长度公差最大值单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_len_tol_min IS '内控长度公差最小值';
COMMENT ON COLUMN ompo.tom01tr.ctl_len_tol_min_unit IS '内控长度公差最小值单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_len_from IS '内控长度起';
COMMENT ON COLUMN ompo.tom01tr.ctl_len_to IS '内控长度止';
COMMENT ON COLUMN ompo.tom01tr.ctl_len_var_max IS '内控最大长度变量';
COMMENT ON COLUMN ompo.tom01tr.ctl_tube_dia_tol_max IS '内控管体外径公差上限';
COMMENT ON COLUMN ompo.tom01tr.ctl_tube_dia_tol_max_unit IS '内控管体外径公差上限单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_tube_dia_tol_min IS '内控管体外径公差下限';
COMMENT ON COLUMN ompo.tom01tr.ctl_tube_dia_tol_min_unit IS '内控管体外径公差下限单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_tube_dia_from IS '内控管体外径起';
COMMENT ON COLUMN ompo.tom01tr.ctl_tube_dia_to IS '内控管体外径止';
COMMENT ON COLUMN ompo.tom01tr.ctl_tube_dia_extreme IS '内控管体外径极差';
COMMENT ON COLUMN ompo.tom01tr.certi_notes IS '质保书备注';
COMMENT ON COLUMN ompo.tom01tr.certi_expl_cname IS '质保书叙述_中文';
COMMENT ON COLUMN ompo.tom01tr.certi_expl_ename IS '质保书叙述_英文';
COMMENT ON COLUMN ompo.tom01tr.kill_method_code IS '镇静方法代码';
COMMENT ON COLUMN ompo.tom01tr.kill_method IS '镇静方法';
COMMENT ON COLUMN ompo.tom01tr.licence_no_code IS '许可证编号代码';
COMMENT ON COLUMN ompo.tom01tr.licence_no IS '许可证编号';
COMMENT ON COLUMN ompo.tom01tr.licence_type IS '许可证类型';
COMMENT ON COLUMN ompo.tom01tr.licence_no_2 IS '许可证编号2';
COMMENT ON COLUMN ompo.tom01tr.cpc IS '接箍参数码';
COMMENT ON COLUMN ompo.tom01tr.jpc IS '工具接头参数码';
COMMENT ON COLUMN ompo.tom01tr.new_prod_code IS '新试产品代码';
COMMENT ON COLUMN ompo.tom01tr.new_prod_agree_no IS '新试产品加工协议号';
COMMENT ON COLUMN ompo.tom01tr.new_prod_desc IS '新试产品描述';
COMMENT ON COLUMN ompo.tom01tr.image_method IS '成像方式';
COMMENT ON COLUMN ompo.tom01tr.tube_detect_rate IS '全管探伤比例';
COMMENT ON COLUMN ompo.tom01tr.in_thread_type_code IS '内螺纹类型代码';
COMMENT ON COLUMN ompo.tom01tr.in_thread_type IS '内螺纹类型';
COMMENT ON COLUMN ompo.tom01tr.in_coat_std_code IS '内涂层标准代码';
COMMENT ON COLUMN ompo.tom01tr.in_coat_cust_std_code IS '内涂层用户标准代码';
COMMENT ON COLUMN ompo.tom01tr.in_coat_str_code IS '内涂层结构代码';
COMMENT ON COLUMN ompo.tom01tr.out_coat_std_code IS '外涂层标准代码';
COMMENT ON COLUMN ompo.tom01tr.out_coat_cust_std_code IS '外涂层用户标准代码';
COMMENT ON COLUMN ompo.tom01tr.out_coat_str_code IS '外涂层结构代码';
COMMENT ON COLUMN ompo.tom01tr.out_coat_temp_code IS '外涂层温度类型代码';
COMMENT ON COLUMN ompo.tom01tr.out_coat_thick_code IS '外涂层厚度级别代码';
COMMENT ON COLUMN ompo.tom01tr.stencil_req_inside IS '内侧喷印要求';
COMMENT ON COLUMN ompo.tom01tr.other_stencil_req_inside IS '内侧另端喷印要求';
COMMENT ON COLUMN ompo.tom01tr.in_stencil_position IS '内侧喷印位置';
COMMENT ON COLUMN ompo.tom01tr.produce_special_req IS '生产特殊要求';
COMMENT ON COLUMN ompo.tom01tr.current_rl_no IS '当前轧批号';
COMMENT ON COLUMN ompo.tom01tr.curr_proc_order_no IS '当前加工合同号';
COMMENT ON COLUMN ompo.tom01tr.hot_plan_date IS '热区计划日期';
COMMENT ON COLUMN ompo.tom01tr.cool_plan_date IS '冷区计划日期';
COMMENT ON COLUMN ompo.tom01tr.cool_backlog_code IS '冷区第一道工序';
COMMENT ON COLUMN ompo.tom01tr.billet_sg_sign IS '管坯牌号';
COMMENT ON COLUMN ompo.tom01tr.billet_psr IS '管坯PSR';
COMMENT ON COLUMN ompo.tom01tr.billet_apn IS '管坯APN';
COMMENT ON COLUMN ompo.tom01tr.billet_dia IS '管坯直径';
COMMENT ON COLUMN ompo.tom01tr.billet_strip_code IS '管坯剥皮方式代码';
COMMENT ON COLUMN ompo.tom01tr.billet_strip IS '管坯剥皮方式';
COMMENT ON COLUMN ompo.tom01tr.billet_proc_mode_code IS '管坯加工方式代码';
COMMENT ON COLUMN ompo.tom01tr.billet_proc_mode IS '管坯加工方式';
COMMENT ON COLUMN ompo.tom01tr.sub_groove_group_code IS '子孔型分类代码';
COMMENT ON COLUMN ompo.tom01tr.steel_class IS '钢种分类';
COMMENT ON COLUMN ompo.tom01tr.steel_grade_class IS '钢种分类正文';
COMMENT ON COLUMN ompo.tom01tr.special_remark IS '特殊说明';
COMMENT ON COLUMN ompo.tom01tr.ou_lot_flag IS '组批方法标志';
COMMENT ON COLUMN ompo.tom01tr.pass_height_tol_min IS '放行高度公差最小值';
COMMENT ON COLUMN ompo.tom01tr.pass_height_tol_min_unit IS '放行高度公差最小值单位';
COMMENT ON COLUMN ompo.tom01tr.pass_height_tol_max_unit IS '放行高度公差最大值单位';
COMMENT ON COLUMN ompo.tom01tr.pass_height_tol_max IS '放行高度公差最大值';
COMMENT ON COLUMN ompo.tom01tr.pass_height_to IS '放行高度止';
COMMENT ON COLUMN ompo.tom01tr.pass_height_from IS '放行高度起';
COMMENT ON COLUMN ompo.tom01tr.ctl_height_tol_min IS '内控高度公差最小值';
COMMENT ON COLUMN ompo.tom01tr.ctl_height_tol_min_unit IS '内控高度公差最小值单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_height_tol_max IS '内控高度公差最大值';
COMMENT ON COLUMN ompo.tom01tr.ctl_height_tol_max_unit IS '内控高度公差最大值单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_height_from IS '内控高度起';
COMMENT ON COLUMN ompo.tom01tr.ctl_height_to IS '内控高度止';
COMMENT ON COLUMN ompo.tom01tr.ctl_height_extreme IS '内控高度极差';
COMMENT ON COLUMN ompo.tom01tr.ctl_angle_radius_down IS '内控圆角半径下限';
COMMENT ON COLUMN ompo.tom01tr.ctl_angle_radius_up IS '内控圆角半径上限';
COMMENT ON COLUMN ompo.tom01tr.ctl_angle_radius_unit IS '内控圆角半径单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_unsmooth_down IS '内控凹凸度下限';
COMMENT ON COLUMN ompo.tom01tr.ctl_unsmooth_up IS '内控凹凸度上限';
COMMENT ON COLUMN ompo.tom01tr.ctl_unsmooth_unit IS '内控凹凸度单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_twist_up IS '内控扭转度上限';
COMMENT ON COLUMN ompo.tom01tr.ctl_lean_up IS '内控倾斜度上限';
COMMENT ON COLUMN ompo.tom01tr.pass_angle_radius_down IS '放行圆角半径下限';
COMMENT ON COLUMN ompo.tom01tr.pass_angle_radius_up IS '放行圆角半径上限';
COMMENT ON COLUMN ompo.tom01tr.pass_angle_radius_unit IS '放行圆角半径单位';
COMMENT ON COLUMN ompo.tom01tr.pass_unsmooth_down IS '放行凹凸度下限';
COMMENT ON COLUMN ompo.tom01tr.pass_unsmooth_up IS '放行凹凸度上限';
COMMENT ON COLUMN ompo.tom01tr.pass_unsmooth_unit IS '放行凹凸度单位';
COMMENT ON COLUMN ompo.tom01tr.pass_twist_up IS '放行扭转度上限';
COMMENT ON COLUMN ompo.tom01tr.pass_lean_up IS '放行倾斜度上限';
COMMENT ON COLUMN ompo.tom01tr.ctl_two_dia_diff IS '内控两端外径差';
COMMENT ON COLUMN ompo.tom01tr.ctl_two_dia_diff_unit IS '内控两端外径差单位';
COMMENT ON COLUMN ompo.tom01tr.ctl_two_in_dia_tol IS '内控两端内径差';
COMMENT ON COLUMN ompo.tom01tr.ctl_two_in_dia_tol_unit IS '内控两端内径差单位';
COMMENT ON COLUMN ompo.tom01tr.gradient_type_in IS '坡口类型描述2_坡口角度';
COMMENT ON COLUMN ompo.tom01tr.gradient_type_thick IS '坡口类型描述3_钝边厚度';
COMMENT ON COLUMN ompo.tom01tr.in_coat_cust_std IS '内涂层用户标准';
COMMENT ON COLUMN ompo.tom01tr.out_coat_cust_std IS '外涂层用户标准';
COMMENT ON COLUMN ompo.tom01tr.in_coat_std IS '内涂层标准';
COMMENT ON COLUMN ompo.tom01tr.out_coat_std IS '外涂层标准';
COMMENT ON COLUMN ompo.tom01tr.in_coat_str IS '内涂层结构';
COMMENT ON COLUMN ompo.tom01tr.out_coat_str IS '外涂层结构';
COMMENT ON COLUMN ompo.tom01tr.out_coat_temp IS '外涂层温度类型';
COMMENT ON COLUMN ompo.tom01tr.out_coat_thick IS '外涂层厚度级别';
COMMENT ON COLUMN ompo.tom01tr.in_coat_std_with_version IS '内涂层标准（含版本）';
COMMENT ON COLUMN ompo.tom01tr.out_coat_std_with_version IS '外涂层标准（含版本）';
COMMENT ON COLUMN ompo.tom01tr.coat_no_in IS '内涂层参数码';
COMMENT ON COLUMN ompo.tom01tr.coat_no_out IS '外涂层参数码';
COMMENT ON COLUMN ompo.tom01tr.order_item_no IS '合同项目号';
COMMENT ON COLUMN ompo.tom01tr.double_high_code IS '双高产品代码';
COMMENT ON COLUMN ompo.tom01tr.double_high_desc IS '双高产品说明';
COMMENT ON COLUMN ompo.tom01tr.whole_backlog_no IS '制程号';
COMMENT ON COLUMN ompo.tom01tr.whole_backlog IS '全程工序途径码';
COMMENT ON COLUMN ompo.tom01tr.backlog IS '合同加工途径';
COMMENT ON COLUMN ompo.tom01tr.pin_jpc IS '工具接头产品规范码';
COMMENT ON COLUMN ompo.tom01tr.apn_jpc IS '工具接头产品最终用途码';
COMMENT ON COLUMN ompo.tom01tr.whole_backlog_desc IS '全程工序途径码说明';
COMMENT ON COLUMN ompo.tom01tr.plan_maker IS '计划责任者';
COMMENT ON COLUMN ompo.tom01tr.change_outer_dia IS '转换外径';
COMMENT ON COLUMN ompo.tom01tr.change_wal_thick IS '转换壁厚';