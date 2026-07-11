-- ompo.tom01 定义

-- Drop table

-- DROP TABLE ompo.tom01;

CREATE TABLE ompo.tom01 (
	rec_creator varchar(24) DEFAULT ' '::character varying NOT NULL, -- 记录创建责任者
	rec_create_time varchar(14) DEFAULT ' '::character varying NOT NULL, -- 记录创建时刻
	rec_revisor varchar(24) DEFAULT ' '::character varying NOT NULL, -- 记录修改责任者
	rec_revise_time varchar(14) DEFAULT ' '::character varying NOT NULL, -- 记录修改时刻
	archive_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 归档标记
	archive_stamp_no varchar(10) DEFAULT ' '::character varying NOT NULL, -- 归档邮戳号
	order_no varchar(10) DEFAULT ' '::character varying NOT NULL, -- 合同号
	order_proc_time varchar(14) DEFAULT ' '::character varying NOT NULL, -- 合同处理时间
	order_proc_oprt varchar(24) DEFAULT ' '::character varying NOT NULL, -- 合同处理责任者
	order_status varchar(2) DEFAULT ' '::character varying NOT NULL, -- 合同状态
	bargain_company_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 卖方公司代码
	order_version_no int4 DEFAULT 0 NOT NULL, -- 合同版本号
	contract_no varchar(20) DEFAULT ' '::character varying NOT NULL, -- 合约号
	contract_version_no int4 DEFAULT 0 NOT NULL, -- 合约版本号
	contract_confirm_time varchar(14) DEFAULT ' '::character varying NOT NULL, -- 合约确认时间
	order_lot_no varchar(10) DEFAULT ' '::character varying NOT NULL, -- 合同批次号
	order_lot_seq_no varchar(2) DEFAULT ' '::character varying NOT NULL, -- 合同批次子项序号
	urg_order_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 紧急合同标记
	sale_network varchar(1) DEFAULT ' '::character varying NOT NULL, -- 销售渠道
	trade_mode varchar(1) DEFAULT ' '::character varying NOT NULL, -- 贸易方式
	sale_mode varchar(2) DEFAULT ' '::character varying NOT NULL, -- 销售方式
	sale_prod_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 销售品种代码
	sale_org_code varchar(8) DEFAULT ' '::character varying NOT NULL, -- 销售组织代码
	delivy_week_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 按周交货标志
	delivery_week varchar(6) DEFAULT ' '::character varying NOT NULL, -- 厂内交货周
	order_delivery_date varchar(8) DEFAULT ' '::character varying NOT NULL, -- 合同交货期
	resource_period varchar(8) DEFAULT ' '::character varying NOT NULL, -- 资源期
	delivery_date_in varchar(8) DEFAULT ' '::character varying NOT NULL, -- 厂内交货期
	band_ord_sort varchar(1) DEFAULT ' '::character varying NOT NULL, -- 基板合同种类
	process_cut_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 加工剪切标志
	export_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 出口标记
	fin_harb_country varchar(3) DEFAULT ' '::character varying NOT NULL, -- 终到站港国别
	sale_user_sort varchar(1) DEFAULT ' '::character varying NOT NULL, -- 销售用户级别
	pre_sale_order_no varchar(10) DEFAULT ' '::character varying NOT NULL, -- 销售预合同号
	order_type_code varchar(3) DEFAULT ' '::character varying NOT NULL, -- 合同性质代码
	order_mono_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 合同买断标志
	band_prod_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 供料品种代码
	band_origin_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 供料来源代码
	deposit_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 委托控货标志
	order_wt numeric(12, 3) DEFAULT 0 NOT NULL, -- 订货重量
	order_qty numeric(10, 3) DEFAULT 0 NOT NULL, -- 订货数量
	order_tube int4 DEFAULT 0 NOT NULL, -- 订货根数
	delivy_tol_unit_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 交货公差单位代码
	delivy_tol_max numeric(11, 3) DEFAULT 0 NOT NULL, -- 交货公差上限
	delivy_tol_min numeric(11, 3) DEFAULT 0 NOT NULL, -- 交货公差下限
	requ_user_code varchar(12) DEFAULT ' '::character varying NOT NULL, -- 需方用户代码
	requ_user_name varchar(60) DEFAULT ' '::character varying NOT NULL, -- 需方用户名称
	agent_user_code varchar(12) DEFAULT ' '::character varying NOT NULL, -- 代理方用户代码
	consign_user_code varchar(12) DEFAULT ' '::character varying NOT NULL, -- 收货用户代码
	consign_user_name varchar(200) DEFAULT ' '::character varying NOT NULL, -- 收货用户名称
	cnsg_address_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 收货用户地址码
	cnsg_address_name varchar(60) DEFAULT ' '::character varying NOT NULL, -- 收货用户地址名称
	fin_cust_code varchar(12) DEFAULT ' '::character varying NOT NULL, -- 最终用户代码
	fin_user_name varchar(200) DEFAULT ' '::character varying NOT NULL, -- 最终用户名称
	fin_cust_code_qual varchar(12) DEFAULT ' '::character varying NOT NULL, -- 质量最终用户代码
	factory_id varchar(3) DEFAULT ' '::character varying NOT NULL, -- 厂别代码
	supply_mark varchar(2) DEFAULT ' '::character varying NOT NULL, -- 生产标志
	last_modi_date varchar(8) DEFAULT ' '::character varying NOT NULL, -- 末次变更日期
	order_modi_type_sale varchar(1) DEFAULT ' '::character varying NOT NULL, -- 合同变更性质(销售下发)
	order_launch_time varchar(14) DEFAULT ' '::character varying NOT NULL, -- 合同下发时间
	order_launch_person_no varchar(24) DEFAULT ' '::character varying NOT NULL, -- 合同下发人工号
	order_launch_person_name varchar(24) DEFAULT ' '::character varying NOT NULL, -- 合同下发人姓名
	order_launch_times int4 DEFAULT 0 NOT NULL, -- 合同下发次数
	balance_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 结算方式代码
	trnp_mode_code varchar(4) DEFAULT ' '::character varying NOT NULL, -- 运输方式代码
	trnp_mode_name varchar(30) DEFAULT ' '::character varying NOT NULL, -- 运输方式名称
	delivery_place_code varchar(8) DEFAULT ' '::character varying NOT NULL, -- 终到站港代码（首端交货地）
	delivery_place_name varchar(60) DEFAULT ' '::character varying NOT NULL, -- 终到站港描述（首端交货地）
	private_route_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 专用线代码
	private_route_name varchar(60) DEFAULT ' '::character varying NOT NULL, -- 专用线名称
	special_trans_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 特殊运输要求代码
	special_trans_desc varchar(100) DEFAULT ' '::character varying NOT NULL, -- 特殊运输要求描述
	rain_coat_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 加盖雨布标志
	logi_scheme_no varchar(8) DEFAULT ' '::character varying NOT NULL, -- 物流方案号
	gather_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 集批标志
	spec_store_code varchar(3) DEFAULT ' '::character varying NOT NULL, -- 指定仓库代码
	latest_ship_date varchar(8) DEFAULT ' '::character varying NOT NULL, -- 最晚装船日期
	loading_port varchar(1) DEFAULT ' '::character varying NOT NULL, -- 发货港代码
	psr varchar(20) DEFAULT ' '::character varying NOT NULL, -- 产品规范码
	apn varchar(4) DEFAULT ' '::character varying NOT NULL, -- 产品最终用途码
	sg_sign varchar(50) DEFAULT ' '::character varying NOT NULL, -- 牌号（钢级）
	sg_std varchar(100) DEFAULT ' '::character varying NOT NULL, -- 牌号标准
	std_version varchar(5) DEFAULT ' '::character varying NOT NULL, -- 标准版次号
	std_sg_code varchar(6) DEFAULT ' '::character varying NOT NULL, -- 标准牌号(钢级)代码
	sg_std_with_version varchar(100) DEFAULT ' '::character varying NOT NULL, -- 牌号标准（含版本）
	prod_class_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 产品大类代码
	prod_class_desc varchar(30) DEFAULT ' '::character varying NOT NULL, -- 产品大类码描述
	prod_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 品名细分类代码
	prod_cname_alias varchar(30) DEFAULT ' '::character varying NOT NULL, -- 品名细分类中文别名
	prod_ename_alias varchar(80) DEFAULT ' '::character varying NOT NULL, -- 品名细分类英文别名
	prod_cname varchar(30) DEFAULT ' '::character varying NOT NULL, -- 品名细分类中文
	prod_ename varchar(80) DEFAULT ' '::character varying NOT NULL, -- 品名细分类英文
	wt_method_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 计重方式代码
	cust_order_num varchar(40) DEFAULT ' '::character varying NOT NULL, -- 客户订单编号
	project_code varchar(6) DEFAULT ' '::character varying NOT NULL, -- 工程代码
	project_name varchar(40) DEFAULT ' '::character varying NOT NULL, -- 工程名称
	manu_remark varchar(100) DEFAULT ' '::character varying NOT NULL, -- 制造备注
	special_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 特殊要求标志
	order_thick numeric(6, 3) DEFAULT 0 NOT NULL, -- 订货厚度
	order_width numeric(7, 3) DEFAULT 0 NOT NULL, -- 订货宽度
	order_width_min numeric(7, 3) DEFAULT 0 NOT NULL, -- 订货宽度下限
	order_width_max numeric(7, 3) DEFAULT 0 NOT NULL, -- 订货宽度上限
	order_width_divi varchar(1) DEFAULT ' '::character varying NOT NULL, -- 宽度范围尺订货区分
	order_len numeric(12, 3) DEFAULT 0 NOT NULL, -- 订货长度
	order_len_min numeric(12, 3) DEFAULT 0 NOT NULL, -- 订货长度下限
	order_len_max numeric(12, 3) DEFAULT 0 NOT NULL, -- 订货长度上限
	order_thick_eng varchar(15) DEFAULT ' '::character varying NOT NULL, -- 英制订货厚度
	order_width_eng varchar(15) DEFAULT ' '::character varying NOT NULL, -- 英制订货宽度
	order_width_min_eng varchar(15) DEFAULT ' '::character varying NOT NULL, -- 英制订货宽度下限
	order_width_max_eng varchar(15) DEFAULT ' '::character varying NOT NULL, -- 英制订货宽度上限
	order_len_min_eng varchar(15) DEFAULT ' '::character varying NOT NULL, -- 英制订货长度下限
	order_len_max_eng varchar(15) DEFAULT ' '::character varying NOT NULL, -- 英制订货长度上限
	order_inner_dia numeric(6, 2) DEFAULT 0 NOT NULL, -- 订货内径
	order_outer_dia numeric(6, 2) DEFAULT 0 NOT NULL, -- 订货外径
	order_height numeric(6, 2) DEFAULT 0 NOT NULL, -- 订货高度
	order_inner_dia_eng numeric(6, 3) DEFAULT 0 NOT NULL, -- 英制订货内径
	order_outer_dia_eng numeric(6, 3) DEFAULT 0 NOT NULL, -- 英制订货外径
	order_wal_thick_eng numeric(6, 3) DEFAULT 0 NOT NULL, -- 英制订货壁厚
	order_high_eng numeric(7, 3) DEFAULT 0 NOT NULL, -- 英制订货高度
	order_short_rate numeric(5, 2) DEFAULT 0 NOT NULL, -- 订货短尺率
	order_short_min numeric(8, 3) DEFAULT 0 NOT NULL, -- 订货短尺长度下限
	order_short_max numeric(8, 3) DEFAULT 0 NOT NULL, -- 订货短尺长度上限
	dmin_pack_wt numeric(6, 3) DEFAULT 0 NOT NULL, -- 小卷的最小件重
	dmax_pack_wt numeric(6, 3) DEFAULT 0 NOT NULL, -- 小卷的最大件重
	order_unit_aim_wt numeric(8, 3) DEFAULT 0 NOT NULL, -- 订货重量单件目标值
	order_unit_min_wt numeric(8, 3) DEFAULT 0 NOT NULL, -- 订货重量单件最小值
	order_unit_max_wt numeric(8, 3) DEFAULT 0 NOT NULL, -- 订货重量单件最大值
	piece_wt numeric(7, 3) DEFAULT 0 NOT NULL, -- 单片重
	strip_num int4 DEFAULT 0 NOT NULL, -- 纵切条数
	strip_num_sale varchar(2) DEFAULT ' '::character varying NOT NULL, -- 销售纵切条数
	csng_n_equal_piece int8 DEFAULT 0 NOT NULL, -- 等片包装数
	inspect_unit varchar(1) DEFAULT ' '::character varying NOT NULL, -- 检验单位
	trim_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 切边标记
	certi_num int4 DEFAULT 0 NOT NULL, -- 质保书份数
	prms_welding_dot int8 DEFAULT 0 NOT NULL, -- 焊道接点
	consign_process_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 委托加工代码
	thick_method_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 计厚方式代码
	accp_auth_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 会检机关
	accp_mode_code varchar(4) DEFAULT ' '::character varying NOT NULL, -- 验收方式
	certi_type_code varchar(4) DEFAULT ' '::character varying NOT NULL, -- 证书类型代码
	better_surf_ward_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 好面朝向代码
	lace_type varchar(4) DEFAULT ' '::character varying NOT NULL, -- 花边种类
	width_length_switch varchar(1) DEFAULT ' '::character varying NOT NULL, -- 宽长转换
	stencil_req_sale int4 DEFAULT 0 NOT NULL, -- 销售喷印要求
	stagger_winding varchar(4) DEFAULT ' '::character varying NOT NULL, -- 错边卷取
	sample_req_code varchar(2) DEFAULT '0'::character varying NOT NULL, -- 取样要求代码
	mark_1 varchar(60) DEFAULT ' '::character varying NOT NULL, -- 唛头1
	mark_2 varchar(60) DEFAULT ' '::character varying NOT NULL, -- 唛头2
	special_flag_type varchar(4) DEFAULT ' '::character varying NOT NULL, -- 特殊标记
	pack_mode_code varchar(4) DEFAULT ' '::character varying NOT NULL, -- 包装方式代码
	special_pack_req varchar(4) DEFAULT ' '::character varying NOT NULL, -- 特殊包装要求
	eng_label_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 英文标签标记
	color_sign varchar(20) DEFAULT ' '::character varying NOT NULL, -- 色标
	label_pos_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 标签位置代码
	initial_ord_mark varchar(1) DEFAULT ' '::character varying NOT NULL, -- 首次供货标志
	vendor_code varchar(20) DEFAULT ' '::character varying NOT NULL, -- 供应商代码
	part_no varchar(30) DEFAULT ' '::character varying NOT NULL, -- 零部件号
	label_format_code varchar(3) DEFAULT ' '::character varying NOT NULL, -- 标签格式代码
	fumigate_req varchar(1) DEFAULT ' '::character varying NOT NULL, -- 熏蒸要求
	research_num varchar(20) DEFAULT ' '::character varying NOT NULL, -- 科研项目号
	semi_pre_order varchar(10) DEFAULT ' '::character varying NOT NULL, -- 在制品预合同号
	mend_mark varchar(1) DEFAULT ' '::character varying NOT NULL, -- 补料标记
	product_export_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 成品合同出口标记
	product_delivy_week_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 成品合同按周标记
	product_delivery_date varchar(8) DEFAULT ' '::character varying NOT NULL, -- 成品合同交货期
	prim_sheet_code varchar(20) DEFAULT ' '::character varying NOT NULL, -- 原板代码
	plant_bna varchar(4) DEFAULT ' '::character varying NOT NULL, -- BNA收料机组号
	mat_group_bna varchar(4) DEFAULT ' '::character varying NOT NULL, -- BNA材料组别及补充号
	dest_bna varchar(3) DEFAULT ' '::character varying NOT NULL, -- BNA去向
	apply_num_bna varchar(14) DEFAULT ' '::character varying NOT NULL, -- BNA申请号
	manu_feed_lot_num varchar(1) DEFAULT ' '::character varying NOT NULL, -- 本制造单元供货批次数
	other_manu_unit_feed varchar(1) DEFAULT ' '::character varying NOT NULL, -- 其它制造单元供货标识
	initial_feed_aux_flag varchar(10) DEFAULT ' '::character varying NOT NULL, -- 首次供货辅助提示
	come_proc_agree_no varchar(10) DEFAULT ' '::character varying NOT NULL, -- 来料加工协议号
	out_proc_prod_ori varchar(2) DEFAULT ' '::character varying NOT NULL, -- 委外加工原料品种
	out_proc_prod_ret varchar(2) DEFAULT ' '::character varying NOT NULL, -- 委外加工返回品种
	price_terms varchar(3) DEFAULT ' '::character varying NOT NULL, -- 价格术语
	settle_mark varchar(2) DEFAULT ' '::character varying NOT NULL, -- 开票类型
	surf_single_double_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 表面单双面代码
	manual_id varchar(30) DEFAULT ' '::character varying NOT NULL, -- 进口手册号(钢管为上锅厂大号)
	order_unit_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 订货计量单位代码
	length_divi varchar(1) DEFAULT ' '::character varying NOT NULL, -- 倍尺/厚板长度范围尺订货区分
	max_length_times int4 DEFAULT 0 NOT NULL, -- 最大长度倍数
	ingot_code varchar(4) DEFAULT ' '::character varying NOT NULL, -- 锭型代码
	chart_version varchar(5) DEFAULT ' '::character varying NOT NULL, -- 图号版本号
	measure_unit varchar(10) DEFAULT ' '::character varying NOT NULL, -- 结算计量单位
	drew_upset_code varchar(1) DEFAULT ' '::character varying NOT NULL, -- 开票点后移合同标志
	south_price_diff numeric(10, 2) DEFAULT 0 NOT NULL, -- 南分价差
	manu_eval varchar(20) DEFAULT ' '::character varying NOT NULL, -- 制造评价
	direct_order_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 直销合同标志
	carry_code varchar(6) DEFAULT ' '::character varying NOT NULL, -- 承运商代码
	carry_name varchar(100) DEFAULT ' '::character varying NOT NULL, -- 承运商名称
	order_modi_type varchar(1) DEFAULT ' '::character varying NOT NULL, -- 合同变更性质(属地处理)
	order_modi_remark varchar(100) DEFAULT ' '::character varying NOT NULL, -- 合同变更备注
	shape_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 形状代码
	special_thick_1 numeric(7, 3) DEFAULT 0 NOT NULL, -- 特殊厚度1
	special_thick_2 numeric(7, 3) DEFAULT 0 NOT NULL, -- 特殊厚度2
	special_thick_3 numeric(7, 3) DEFAULT 0 NOT NULL, -- 特殊厚度3
	special_len_1 int8 DEFAULT 0 NOT NULL, -- 特殊长度1
	special_len_2 int8 DEFAULT 0 NOT NULL, -- 特殊长度2
	special_len_3 int8 DEFAULT 0 NOT NULL, -- 特殊长度3
	special_len_4 int8 DEFAULT 0 NOT NULL, -- 特殊长度4
	special_len_5 int8 DEFAULT 0 NOT NULL, -- 特殊长度5
	d_piece int8 DEFAULT 0 NOT NULL, -- 小卷件数
	prod_config_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 产品配置码
	density numeric(5, 3) DEFAULT 0 NOT NULL, -- 密度
	mic varchar(6) DEFAULT ' '::character varying NOT NULL, -- 冶金规范码
	trim_mode varchar(2) DEFAULT ' '::character varying NOT NULL, -- 切边方式
	order_match_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 合同匹配标记
	replace_lmt_sign varchar(1) DEFAULT ' '::character varying NOT NULL, -- 充当限制标记
	plant_difficulty varchar(2) DEFAULT ' '::character varying NOT NULL, -- 难度标识
	new_test_no varchar(4) DEFAULT ' '::character varying NOT NULL, -- 新试号
	thick_meas_place varchar(3) DEFAULT ' '::character varying NOT NULL, -- 厚度测量位置
	thick_tol_minus numeric(6, 3) DEFAULT 0 NOT NULL, -- 厚度负公差
	thick_tol_plus numeric(6, 3) DEFAULT 0 NOT NULL, -- 厚度正公差
	thick_addv numeric(6, 3) DEFAULT 0 NOT NULL, -- 厚度余量
	width_tol_minus numeric(5, 2) DEFAULT 0 NOT NULL, -- 宽度负公差
	width_tol_plus numeric(5, 2) DEFAULT 0 NOT NULL, -- 宽度正公差
	len_tol_minus numeric(6, 2) DEFAULT 0 NOT NULL, -- 长度负公差
	len_tol_plus numeric(6, 2) DEFAULT 0 NOT NULL, -- 长度正公差
	tower_max int4 DEFAULT 0 NOT NULL, -- 塔形最大值
	pack_vrtc_num int4 DEFAULT 0 NOT NULL, -- 纵向/径向捆带数
	pack_cross_num int4 DEFAULT 0 NOT NULL, -- 横向/圆周捆带数
	wave_height numeric(4, 1) DEFAULT 0 NOT NULL, -- 波高
	wave_len numeric(4, 1) DEFAULT 0 NOT NULL, -- 波长
	wave_width numeric(4, 1) DEFAULT 0 NOT NULL, -- 波底宽度
	wave_angle numeric(4, 1) DEFAULT 0 NOT NULL, -- 波形角度
	inner_bend_radius numeric(3, 1) DEFAULT 0 NOT NULL, -- 内弯半径
	prod_thick numeric(6, 3) DEFAULT 0 NOT NULL, -- 成品厚度
	prod_width numeric(7, 3) DEFAULT 0 NOT NULL, -- 成品宽度
	prod_len numeric(8, 3) DEFAULT 0 NOT NULL, -- 成品长度
	plate_or_coil varchar(1) DEFAULT ' '::character varying NOT NULL, -- 板卷类型
	tol_prop numeric(4, 2) DEFAULT 0 NOT NULL, -- 公差比例
	lmt_tol_min numeric(5, 3) DEFAULT 0 NOT NULL, -- 同板差下限
	lmt_tol_max numeric(5, 3) DEFAULT 0 NOT NULL, -- 同板差上限
	pack_len numeric(6, 1) DEFAULT 0 NOT NULL, -- 包装长度
	lateral_bend_measure_len numeric(7, 2) DEFAULT 0 NOT NULL, -- 侧弯测量长度
	lateral_bend_tol numeric(7, 2) DEFAULT 0 NOT NULL, -- 侧弯公差
	flat_grade varchar(1) DEFAULT ' '::character varying NOT NULL, -- 平坦度等级
	whole_wave_tol numeric(4, 2) DEFAULT 0 NOT NULL, -- 全波平坦度公差
	center_wave_tol numeric(4, 2) DEFAULT 0 NOT NULL, -- 中波平坦度公差
	side_wave_plat_tol numeric(4, 2) DEFAULT 0 NOT NULL, -- 边波平坦度公差
	plat_measure_len numeric(6, 1) DEFAULT 0 NOT NULL, -- 平坦度测量长度
	warp_len int4 DEFAULT 0 NOT NULL, -- 翘曲长度方向
	warp_width int4 DEFAULT 0 NOT NULL, -- 翘曲宽度方向
	edge_w_degree numeric(3, 1) DEFAULT 0 NOT NULL, -- 边波急峻度
	center_w_degree numeric(3, 1) DEFAULT 0 NOT NULL, -- 中波急峻度
	edge_wave_num int4 DEFAULT 0 NOT NULL, -- 边波个数
	edge_wave_high numeric(4, 2) DEFAULT 0 NOT NULL, -- 边波浪高
	shed_rec_max numeric(4, 2) DEFAULT 0 NOT NULL, -- 脱方度最大值
	sort_grade_code_f varchar(1) DEFAULT ' '::character varying NOT NULL, -- 成品分选度代码
	hard_aim numeric(4, 1) DEFAULT 0 NOT NULL, -- 硬度目标值
	ys_stand numeric(5, 1) DEFAULT 0 NOT NULL, -- 屈服强度代表值
	ts_stand numeric(5, 1) DEFAULT 0 NOT NULL, -- 抗拉强度代表值
	temper varchar(8) DEFAULT ' '::character varying NOT NULL, -- 调质度
	test_req_ucode varchar(200) DEFAULT ' '::character varying NOT NULL, -- 试验指示组合码
	steel_grade_type varchar(2) DEFAULT ' '::character varying NOT NULL, -- 钢级大类
	si_prod_code varchar(4) DEFAULT ' '::character varying NOT NULL, -- 硅钢品种代码
	apn_desc varchar(80) DEFAULT ' '::character varying NOT NULL, -- 产品最终用途说明
	sample_pos_code varchar(3) DEFAULT ' '::character varying NOT NULL, -- 取样位置代码
	label_std varchar(50) DEFAULT ' '::character varying NOT NULL, -- 标签用标准
	prod_plate_code varchar(20) DEFAULT ' '::character varying NOT NULL, -- 产品镀层量
	prod_label_jis_flag varchar(1) DEFAULT '0'::character varying NOT NULL, -- 成品标签JIS标志
	label_special_remark varchar(50) DEFAULT ' '::character varying NOT NULL, -- 标签特殊说明
	prod_color varchar(20) DEFAULT ' '::character varying NOT NULL, -- 产品颜色
	pack_wt numeric(8, 3) DEFAULT 0 NOT NULL, -- 包皮重
	mat_strength_grade varchar(4) DEFAULT ' '::character varying NOT NULL, -- 材料强度等级
	idx_no_certi_remark varchar(4) DEFAULT ' '::character varying NOT NULL, -- 质保书注释索引号
	new_prod_class varchar(2) DEFAULT ' '::character varying NOT NULL, -- 新产品类别
	chg_prct_date varchar(8) DEFAULT ' '::character varying NOT NULL, -- 转产日期
	sign_code_1 varchar(15) DEFAULT ' '::character varying NOT NULL, -- 副钢级代码1
	sign_code_2 varchar(15) DEFAULT ' '::character varying NOT NULL, -- 副钢级代码2
	sign_code_3 varchar(15) DEFAULT ' '::character varying NOT NULL, -- 副钢级代码3
	launch_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 下发晚标记
	ord_mark_bit varchar(4) DEFAULT ' '::character varying NOT NULL, -- 合同标记位
	ord_mark_revisor varchar(24) DEFAULT ' '::character varying NOT NULL, -- 合同标记位修改者
	ord_mark_time varchar(14) DEFAULT ' '::character varying NOT NULL, -- 合同标记位修改时间
	order_ready_date varchar(8) DEFAULT ' '::character varying NOT NULL, -- 合同备妥期
	trans_mat_flag varchar(2) DEFAULT ' '::character varying NOT NULL, -- 过渡料标识
	line_design_code varchar(2) DEFAULT ' '::character varying NOT NULL, -- 产线设计代码
	hold_flag varchar(1) DEFAULT '0'::character varying NOT NULL, -- 封锁标记
	hold_time varchar(14) DEFAULT ' '::character varying NOT NULL, -- 封锁时刻
	hold_maker varchar(24) DEFAULT ' '::character varying NOT NULL, -- 封锁责任者
	order_end_date varchar(14) DEFAULT ' '::character varying NOT NULL, -- 合同结案日期
	merg_order_no varchar(10) DEFAULT ' '::character varying NOT NULL, -- 归并合同号
	old_order_status varchar(2) DEFAULT ' '::character varying NOT NULL, -- 原合同状态
	hold_flag_bna varchar(1) DEFAULT '0'::character varying NOT NULL, -- BNA封锁标记
	retmsg varchar(100) DEFAULT ' '::character varying NOT NULL, -- 返回信息
	unit_wt numeric(5, 1) DEFAULT 0 NOT NULL, -- 单位长度重量
	dgnl_tol_max numeric(5, 2) DEFAULT 0 NOT NULL, -- 对角线公差上限
	bend_degree_tot_max numeric(6, 2) DEFAULT 0 NOT NULL, -- 全长弯曲度上限
	flat_max numeric(6, 2) DEFAULT 0 NOT NULL, -- 压扁量上限
	flm_slant_width numeric(3, 1) DEFAULT 0 NOT NULL, -- 火焰切斜宽向
	flm_slant_thick numeric(3, 1) DEFAULT 0 NOT NULL, -- 火焰切斜厚向
	saw_burr numeric(2, 1) DEFAULT 0 NOT NULL, -- 锯切毛刺
	saw_slant numeric(3, 1) DEFAULT 0 NOT NULL, -- 锯切斜切
	dia_tol_min numeric(4, 2) DEFAULT 0 NOT NULL, -- 边长下限公差
	dia_tol_max numeric(4, 2) DEFAULT 0 NOT NULL, -- 边长上限公差
	length_of_part numeric(6, 1) DEFAULT 0 NOT NULL, -- 测量长度
	bend_degr_max numeric(4, 2) DEFAULT 0 NOT NULL, -- 弯曲度最大值
	label_special_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 标签特殊说明指令
	op_status varchar(1) DEFAULT ' '::character varying NOT NULL, -- 合同计划状态
	prod_auth_code varchar(30) DEFAULT ' '::character varying NOT NULL, -- 产品认证代码
	prod_licence_no varchar(20) DEFAULT ' '::character varying NOT NULL, -- 产品许可证号
	spvise_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 监造标记
	come_proc_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 来料加工标记
	std_code varchar(3) DEFAULT ' '::character varying NOT NULL, -- 标准代码
	order_confirm_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 合同确认标志
	order_confirm_maker varchar(24) DEFAULT ' '::character varying NOT NULL, -- 合同确认责任者
	order_confirm_time varchar(14) DEFAULT ' '::character varying NOT NULL, -- 合同确认时间
	big_coil_flag varchar(3) DEFAULT ' '::character varying NOT NULL, -- 大卷标志
	burr_height_max numeric(5, 3) DEFAULT 0 NOT NULL, -- 最大毛刺高度
	char_code varchar(20) DEFAULT ' '::character varying NOT NULL, -- 特征码
	urg_flag varchar(5) DEFAULT ' '::character varying NOT NULL, -- 紧急标记（生产用）
	center_wave_high numeric(4, 1) DEFAULT 0 NOT NULL, -- 中波浪高
	tol_spec_flag varchar(1) DEFAULT ' '::character varying NOT NULL, -- 同板差特殊要求标志
	od_div_code varchar(3) DEFAULT ' '::character varying NOT NULL, -- 产线设计区分代码
	hot_fix_flag varchar(20) DEFAULT ' '::character varying NOT NULL, -- 热轧固定标识
	cold_fix_flag varchar(20) DEFAULT ' '::character varying NOT NULL, -- 冷轧固定标识
	finish_fix_flag varchar(20) DEFAULT ' '::character varying NOT NULL, -- 精整固定标识
	order_priority varchar(1) DEFAULT ' '::character varying NOT NULL, -- 订单优先级
	same_kind_flag varchar(2) DEFAULT ' '::character varying NOT NULL, -- 同类标志
	fpc varchar(20) DEFAULT ' '::character varying NOT NULL, -- 最终产品码
	fur_fix_flag varchar(20) DEFAULT ' '::character varying NOT NULL, -- 退火固定标识
	hc_fix_flag varchar(20) DEFAULT ' '::character varying NOT NULL, -- 热镀固定标识
	ec_fix_flag varchar(20) DEFAULT ' '::character varying NOT NULL, -- 电镀固定标识
	spare_time_1 varchar(14) DEFAULT ' '::character varying NOT NULL, -- 备用时间_1
	spare_time_2 varchar(14) DEFAULT ' '::character varying NOT NULL, -- 备用时间_2
	spare_item_1 varchar(10) DEFAULT ' '::character varying NOT NULL, -- 备用字段_1
	spare_item_2 varchar(10) DEFAULT ' '::character varying NOT NULL, -- 备用字段_2
	spare_remark varchar(50) DEFAULT ' '::character varying NOT NULL, -- 备用备注
	vert_thick_tol_minus numeric(6, 3) DEFAULT 0 NOT NULL, -- 纵向厚度负公差
	vert_thick_tol_plus numeric(6, 3) DEFAULT 0 NOT NULL, -- 纵向厚度正公差
	CONSTRAINT tom01_pk PRIMARY KEY (order_no)
)
WITH (
	orientation=row,
	compression=no,
	storage_type=ustore,
	segment=off
);
COMMENT ON TABLE ompo.tom01 IS '合同主档表';

-- Column comments

COMMENT ON COLUMN ompo.tom01.rec_creator IS '记录创建责任者';
COMMENT ON COLUMN ompo.tom01.rec_create_time IS '记录创建时刻';
COMMENT ON COLUMN ompo.tom01.rec_revisor IS '记录修改责任者';
COMMENT ON COLUMN ompo.tom01.rec_revise_time IS '记录修改时刻';
COMMENT ON COLUMN ompo.tom01.archive_flag IS '归档标记';
COMMENT ON COLUMN ompo.tom01.archive_stamp_no IS '归档邮戳号';
COMMENT ON COLUMN ompo.tom01.order_no IS '合同号';
COMMENT ON COLUMN ompo.tom01.order_proc_time IS '合同处理时间';
COMMENT ON COLUMN ompo.tom01.order_proc_oprt IS '合同处理责任者';
COMMENT ON COLUMN ompo.tom01.order_status IS '合同状态';
COMMENT ON COLUMN ompo.tom01.bargain_company_code IS '卖方公司代码';
COMMENT ON COLUMN ompo.tom01.order_version_no IS '合同版本号';
COMMENT ON COLUMN ompo.tom01.contract_no IS '合约号';
COMMENT ON COLUMN ompo.tom01.contract_version_no IS '合约版本号';
COMMENT ON COLUMN ompo.tom01.contract_confirm_time IS '合约确认时间';
COMMENT ON COLUMN ompo.tom01.order_lot_no IS '合同批次号';
COMMENT ON COLUMN ompo.tom01.order_lot_seq_no IS '合同批次子项序号';
COMMENT ON COLUMN ompo.tom01.urg_order_flag IS '紧急合同标记';
COMMENT ON COLUMN ompo.tom01.sale_network IS '销售渠道';
COMMENT ON COLUMN ompo.tom01.trade_mode IS '贸易方式';
COMMENT ON COLUMN ompo.tom01.sale_mode IS '销售方式';
COMMENT ON COLUMN ompo.tom01.sale_prod_code IS '销售品种代码';
COMMENT ON COLUMN ompo.tom01.sale_org_code IS '销售组织代码';
COMMENT ON COLUMN ompo.tom01.delivy_week_flag IS '按周交货标志';
COMMENT ON COLUMN ompo.tom01.delivery_week IS '厂内交货周';
COMMENT ON COLUMN ompo.tom01.order_delivery_date IS '合同交货期';
COMMENT ON COLUMN ompo.tom01.resource_period IS '资源期';
COMMENT ON COLUMN ompo.tom01.delivery_date_in IS '厂内交货期';
COMMENT ON COLUMN ompo.tom01.band_ord_sort IS '基板合同种类';
COMMENT ON COLUMN ompo.tom01.process_cut_flag IS '加工剪切标志';
COMMENT ON COLUMN ompo.tom01.export_flag IS '出口标记';
COMMENT ON COLUMN ompo.tom01.fin_harb_country IS '终到站港国别';
COMMENT ON COLUMN ompo.tom01.sale_user_sort IS '销售用户级别';
COMMENT ON COLUMN ompo.tom01.pre_sale_order_no IS '销售预合同号';
COMMENT ON COLUMN ompo.tom01.order_type_code IS '合同性质代码';
COMMENT ON COLUMN ompo.tom01.order_mono_flag IS '合同买断标志';
COMMENT ON COLUMN ompo.tom01.band_prod_code IS '供料品种代码';
COMMENT ON COLUMN ompo.tom01.band_origin_code IS '供料来源代码';
COMMENT ON COLUMN ompo.tom01.deposit_flag IS '委托控货标志';
COMMENT ON COLUMN ompo.tom01.order_wt IS '订货重量';
COMMENT ON COLUMN ompo.tom01.order_qty IS '订货数量';
COMMENT ON COLUMN ompo.tom01.order_tube IS '订货根数';
COMMENT ON COLUMN ompo.tom01.delivy_tol_unit_code IS '交货公差单位代码';
COMMENT ON COLUMN ompo.tom01.delivy_tol_max IS '交货公差上限';
COMMENT ON COLUMN ompo.tom01.delivy_tol_min IS '交货公差下限';
COMMENT ON COLUMN ompo.tom01.requ_user_code IS '需方用户代码';
COMMENT ON COLUMN ompo.tom01.requ_user_name IS '需方用户名称';
COMMENT ON COLUMN ompo.tom01.agent_user_code IS '代理方用户代码';
COMMENT ON COLUMN ompo.tom01.consign_user_code IS '收货用户代码';
COMMENT ON COLUMN ompo.tom01.consign_user_name IS '收货用户名称';
COMMENT ON COLUMN ompo.tom01.cnsg_address_code IS '收货用户地址码';
COMMENT ON COLUMN ompo.tom01.cnsg_address_name IS '收货用户地址名称';
COMMENT ON COLUMN ompo.tom01.fin_cust_code IS '最终用户代码';
COMMENT ON COLUMN ompo.tom01.fin_user_name IS '最终用户名称';
COMMENT ON COLUMN ompo.tom01.fin_cust_code_qual IS '质量最终用户代码';
COMMENT ON COLUMN ompo.tom01.factory_id IS '厂别代码';
COMMENT ON COLUMN ompo.tom01.supply_mark IS '生产标志';
COMMENT ON COLUMN ompo.tom01.last_modi_date IS '末次变更日期';
COMMENT ON COLUMN ompo.tom01.order_modi_type_sale IS '合同变更性质(销售下发)';
COMMENT ON COLUMN ompo.tom01.order_launch_time IS '合同下发时间';
COMMENT ON COLUMN ompo.tom01.order_launch_person_no IS '合同下发人工号';
COMMENT ON COLUMN ompo.tom01.order_launch_person_name IS '合同下发人姓名';
COMMENT ON COLUMN ompo.tom01.order_launch_times IS '合同下发次数';
COMMENT ON COLUMN ompo.tom01.balance_code IS '结算方式代码';
COMMENT ON COLUMN ompo.tom01.trnp_mode_code IS '运输方式代码';
COMMENT ON COLUMN ompo.tom01.trnp_mode_name IS '运输方式名称';
COMMENT ON COLUMN ompo.tom01.delivery_place_code IS '终到站港代码（首端交货地）';
COMMENT ON COLUMN ompo.tom01.delivery_place_name IS '终到站港描述（首端交货地）';
COMMENT ON COLUMN ompo.tom01.private_route_code IS '专用线代码';
COMMENT ON COLUMN ompo.tom01.private_route_name IS '专用线名称';
COMMENT ON COLUMN ompo.tom01.special_trans_code IS '特殊运输要求代码';
COMMENT ON COLUMN ompo.tom01.special_trans_desc IS '特殊运输要求描述';
COMMENT ON COLUMN ompo.tom01.rain_coat_flag IS '加盖雨布标志';
COMMENT ON COLUMN ompo.tom01.logi_scheme_no IS '物流方案号';
COMMENT ON COLUMN ompo.tom01.gather_flag IS '集批标志';
COMMENT ON COLUMN ompo.tom01.spec_store_code IS '指定仓库代码';
COMMENT ON COLUMN ompo.tom01.latest_ship_date IS '最晚装船日期';
COMMENT ON COLUMN ompo.tom01.loading_port IS '发货港代码';
COMMENT ON COLUMN ompo.tom01.psr IS '产品规范码';
COMMENT ON COLUMN ompo.tom01.apn IS '产品最终用途码';
COMMENT ON COLUMN ompo.tom01.sg_sign IS '牌号（钢级）';
COMMENT ON COLUMN ompo.tom01.sg_std IS '牌号标准';
COMMENT ON COLUMN ompo.tom01.std_version IS '标准版次号';
COMMENT ON COLUMN ompo.tom01.std_sg_code IS '标准牌号(钢级)代码';
COMMENT ON COLUMN ompo.tom01.sg_std_with_version IS '牌号标准（含版本）';
COMMENT ON COLUMN ompo.tom01.prod_class_code IS '产品大类代码';
COMMENT ON COLUMN ompo.tom01.prod_class_desc IS '产品大类码描述';
COMMENT ON COLUMN ompo.tom01.prod_code IS '品名细分类代码';
COMMENT ON COLUMN ompo.tom01.prod_cname_alias IS '品名细分类中文别名';
COMMENT ON COLUMN ompo.tom01.prod_ename_alias IS '品名细分类英文别名';
COMMENT ON COLUMN ompo.tom01.prod_cname IS '品名细分类中文';
COMMENT ON COLUMN ompo.tom01.prod_ename IS '品名细分类英文';
COMMENT ON COLUMN ompo.tom01.wt_method_code IS '计重方式代码';
COMMENT ON COLUMN ompo.tom01.cust_order_num IS '客户订单编号';
COMMENT ON COLUMN ompo.tom01.project_code IS '工程代码';
COMMENT ON COLUMN ompo.tom01.project_name IS '工程名称';
COMMENT ON COLUMN ompo.tom01.manu_remark IS '制造备注';
COMMENT ON COLUMN ompo.tom01.special_flag IS '特殊要求标志';
COMMENT ON COLUMN ompo.tom01.order_thick IS '订货厚度';
COMMENT ON COLUMN ompo.tom01.order_width IS '订货宽度';
COMMENT ON COLUMN ompo.tom01.order_width_min IS '订货宽度下限';
COMMENT ON COLUMN ompo.tom01.order_width_max IS '订货宽度上限';
COMMENT ON COLUMN ompo.tom01.order_width_divi IS '宽度范围尺订货区分';
COMMENT ON COLUMN ompo.tom01.order_len IS '订货长度';
COMMENT ON COLUMN ompo.tom01.order_len_min IS '订货长度下限';
COMMENT ON COLUMN ompo.tom01.order_len_max IS '订货长度上限';
COMMENT ON COLUMN ompo.tom01.order_thick_eng IS '英制订货厚度';
COMMENT ON COLUMN ompo.tom01.order_width_eng IS '英制订货宽度';
COMMENT ON COLUMN ompo.tom01.order_width_min_eng IS '英制订货宽度下限';
COMMENT ON COLUMN ompo.tom01.order_width_max_eng IS '英制订货宽度上限';
COMMENT ON COLUMN ompo.tom01.order_len_min_eng IS '英制订货长度下限';
COMMENT ON COLUMN ompo.tom01.order_len_max_eng IS '英制订货长度上限';
COMMENT ON COLUMN ompo.tom01.order_inner_dia IS '订货内径';
COMMENT ON COLUMN ompo.tom01.order_outer_dia IS '订货外径';
COMMENT ON COLUMN ompo.tom01.order_height IS '订货高度';
COMMENT ON COLUMN ompo.tom01.order_inner_dia_eng IS '英制订货内径';
COMMENT ON COLUMN ompo.tom01.order_outer_dia_eng IS '英制订货外径';
COMMENT ON COLUMN ompo.tom01.order_wal_thick_eng IS '英制订货壁厚';
COMMENT ON COLUMN ompo.tom01.order_high_eng IS '英制订货高度';
COMMENT ON COLUMN ompo.tom01.order_short_rate IS '订货短尺率';
COMMENT ON COLUMN ompo.tom01.order_short_min IS '订货短尺长度下限';
COMMENT ON COLUMN ompo.tom01.order_short_max IS '订货短尺长度上限';
COMMENT ON COLUMN ompo.tom01.dmin_pack_wt IS '小卷的最小件重';
COMMENT ON COLUMN ompo.tom01.dmax_pack_wt IS '小卷的最大件重';
COMMENT ON COLUMN ompo.tom01.order_unit_aim_wt IS '订货重量单件目标值';
COMMENT ON COLUMN ompo.tom01.order_unit_min_wt IS '订货重量单件最小值';
COMMENT ON COLUMN ompo.tom01.order_unit_max_wt IS '订货重量单件最大值';
COMMENT ON COLUMN ompo.tom01.piece_wt IS '单片重';
COMMENT ON COLUMN ompo.tom01.strip_num IS '纵切条数';
COMMENT ON COLUMN ompo.tom01.strip_num_sale IS '销售纵切条数';
COMMENT ON COLUMN ompo.tom01.csng_n_equal_piece IS '等片包装数';
COMMENT ON COLUMN ompo.tom01.inspect_unit IS '检验单位';
COMMENT ON COLUMN ompo.tom01.trim_flag IS '切边标记';
COMMENT ON COLUMN ompo.tom01.certi_num IS '质保书份数';
COMMENT ON COLUMN ompo.tom01.prms_welding_dot IS '焊道接点';
COMMENT ON COLUMN ompo.tom01.consign_process_code IS '委托加工代码';
COMMENT ON COLUMN ompo.tom01.thick_method_code IS '计厚方式代码';
COMMENT ON COLUMN ompo.tom01.accp_auth_code IS '会检机关';
COMMENT ON COLUMN ompo.tom01.accp_mode_code IS '验收方式';
COMMENT ON COLUMN ompo.tom01.certi_type_code IS '证书类型代码';
COMMENT ON COLUMN ompo.tom01.better_surf_ward_code IS '好面朝向代码';
COMMENT ON COLUMN ompo.tom01.lace_type IS '花边种类';
COMMENT ON COLUMN ompo.tom01.width_length_switch IS '宽长转换';
COMMENT ON COLUMN ompo.tom01.stencil_req_sale IS '销售喷印要求';
COMMENT ON COLUMN ompo.tom01.stagger_winding IS '错边卷取';
COMMENT ON COLUMN ompo.tom01.sample_req_code IS '取样要求代码';
COMMENT ON COLUMN ompo.tom01.mark_1 IS '唛头1';
COMMENT ON COLUMN ompo.tom01.mark_2 IS '唛头2';
COMMENT ON COLUMN ompo.tom01.special_flag_type IS '特殊标记';
COMMENT ON COLUMN ompo.tom01.pack_mode_code IS '包装方式代码';
COMMENT ON COLUMN ompo.tom01.special_pack_req IS '特殊包装要求';
COMMENT ON COLUMN ompo.tom01.eng_label_flag IS '英文标签标记';
COMMENT ON COLUMN ompo.tom01.color_sign IS '色标';
COMMENT ON COLUMN ompo.tom01.label_pos_code IS '标签位置代码';
COMMENT ON COLUMN ompo.tom01.initial_ord_mark IS '首次供货标志';
COMMENT ON COLUMN ompo.tom01.vendor_code IS '供应商代码';
COMMENT ON COLUMN ompo.tom01.part_no IS '零部件号';
COMMENT ON COLUMN ompo.tom01.label_format_code IS '标签格式代码';
COMMENT ON COLUMN ompo.tom01.fumigate_req IS '熏蒸要求';
COMMENT ON COLUMN ompo.tom01.research_num IS '科研项目号';
COMMENT ON COLUMN ompo.tom01.semi_pre_order IS '在制品预合同号';
COMMENT ON COLUMN ompo.tom01.mend_mark IS '补料标记';
COMMENT ON COLUMN ompo.tom01.product_export_flag IS '成品合同出口标记';
COMMENT ON COLUMN ompo.tom01.product_delivy_week_flag IS '成品合同按周标记';
COMMENT ON COLUMN ompo.tom01.product_delivery_date IS '成品合同交货期';
COMMENT ON COLUMN ompo.tom01.prim_sheet_code IS '原板代码';
COMMENT ON COLUMN ompo.tom01.plant_bna IS 'BNA收料机组号';
COMMENT ON COLUMN ompo.tom01.mat_group_bna IS 'BNA材料组别及补充号';
COMMENT ON COLUMN ompo.tom01.dest_bna IS 'BNA去向';
COMMENT ON COLUMN ompo.tom01.apply_num_bna IS 'BNA申请号';
COMMENT ON COLUMN ompo.tom01.manu_feed_lot_num IS '本制造单元供货批次数';
COMMENT ON COLUMN ompo.tom01.other_manu_unit_feed IS '其它制造单元供货标识';
COMMENT ON COLUMN ompo.tom01.initial_feed_aux_flag IS '首次供货辅助提示';
COMMENT ON COLUMN ompo.tom01.come_proc_agree_no IS '来料加工协议号';
COMMENT ON COLUMN ompo.tom01.out_proc_prod_ori IS '委外加工原料品种';
COMMENT ON COLUMN ompo.tom01.out_proc_prod_ret IS '委外加工返回品种';
COMMENT ON COLUMN ompo.tom01.price_terms IS '价格术语';
COMMENT ON COLUMN ompo.tom01.settle_mark IS '开票类型';
COMMENT ON COLUMN ompo.tom01.surf_single_double_code IS '表面单双面代码';
COMMENT ON COLUMN ompo.tom01.manual_id IS '进口手册号(钢管为上锅厂大号)';
COMMENT ON COLUMN ompo.tom01.order_unit_code IS '订货计量单位代码';
COMMENT ON COLUMN ompo.tom01.length_divi IS '倍尺/厚板长度范围尺订货区分';
COMMENT ON COLUMN ompo.tom01.max_length_times IS '最大长度倍数';
COMMENT ON COLUMN ompo.tom01.ingot_code IS '锭型代码';
COMMENT ON COLUMN ompo.tom01.chart_version IS '图号版本号';
COMMENT ON COLUMN ompo.tom01.measure_unit IS '结算计量单位';
COMMENT ON COLUMN ompo.tom01.drew_upset_code IS '开票点后移合同标志';
COMMENT ON COLUMN ompo.tom01.south_price_diff IS '南分价差';
COMMENT ON COLUMN ompo.tom01.manu_eval IS '制造评价';
COMMENT ON COLUMN ompo.tom01.direct_order_flag IS '直销合同标志';
COMMENT ON COLUMN ompo.tom01.carry_code IS '承运商代码';
COMMENT ON COLUMN ompo.tom01.carry_name IS '承运商名称';
COMMENT ON COLUMN ompo.tom01.order_modi_type IS '合同变更性质(属地处理)';
COMMENT ON COLUMN ompo.tom01.order_modi_remark IS '合同变更备注';
COMMENT ON COLUMN ompo.tom01.shape_code IS '形状代码';
COMMENT ON COLUMN ompo.tom01.special_thick_1 IS '特殊厚度1';
COMMENT ON COLUMN ompo.tom01.special_thick_2 IS '特殊厚度2';
COMMENT ON COLUMN ompo.tom01.special_thick_3 IS '特殊厚度3';
COMMENT ON COLUMN ompo.tom01.special_len_1 IS '特殊长度1';
COMMENT ON COLUMN ompo.tom01.special_len_2 IS '特殊长度2';
COMMENT ON COLUMN ompo.tom01.special_len_3 IS '特殊长度3';
COMMENT ON COLUMN ompo.tom01.special_len_4 IS '特殊长度4';
COMMENT ON COLUMN ompo.tom01.special_len_5 IS '特殊长度5';
COMMENT ON COLUMN ompo.tom01.d_piece IS '小卷件数';
COMMENT ON COLUMN ompo.tom01.prod_config_code IS '产品配置码';
COMMENT ON COLUMN ompo.tom01.density IS '密度';
COMMENT ON COLUMN ompo.tom01.mic IS '冶金规范码';
COMMENT ON COLUMN ompo.tom01.trim_mode IS '切边方式';
COMMENT ON COLUMN ompo.tom01.order_match_flag IS '合同匹配标记';
COMMENT ON COLUMN ompo.tom01.replace_lmt_sign IS '充当限制标记';
COMMENT ON COLUMN ompo.tom01.plant_difficulty IS '难度标识';
COMMENT ON COLUMN ompo.tom01.new_test_no IS '新试号';
COMMENT ON COLUMN ompo.tom01.thick_meas_place IS '厚度测量位置';
COMMENT ON COLUMN ompo.tom01.thick_tol_minus IS '厚度负公差';
COMMENT ON COLUMN ompo.tom01.thick_tol_plus IS '厚度正公差';
COMMENT ON COLUMN ompo.tom01.thick_addv IS '厚度余量';
COMMENT ON COLUMN ompo.tom01.width_tol_minus IS '宽度负公差';
COMMENT ON COLUMN ompo.tom01.width_tol_plus IS '宽度正公差';
COMMENT ON COLUMN ompo.tom01.len_tol_minus IS '长度负公差';
COMMENT ON COLUMN ompo.tom01.len_tol_plus IS '长度正公差';
COMMENT ON COLUMN ompo.tom01.tower_max IS '塔形最大值';
COMMENT ON COLUMN ompo.tom01.pack_vrtc_num IS '纵向/径向捆带数';
COMMENT ON COLUMN ompo.tom01.pack_cross_num IS '横向/圆周捆带数';
COMMENT ON COLUMN ompo.tom01.wave_height IS '波高';
COMMENT ON COLUMN ompo.tom01.wave_len IS '波长';
COMMENT ON COLUMN ompo.tom01.wave_width IS '波底宽度';
COMMENT ON COLUMN ompo.tom01.wave_angle IS '波形角度';
COMMENT ON COLUMN ompo.tom01.inner_bend_radius IS '内弯半径';
COMMENT ON COLUMN ompo.tom01.prod_thick IS '成品厚度';
COMMENT ON COLUMN ompo.tom01.prod_width IS '成品宽度';
COMMENT ON COLUMN ompo.tom01.prod_len IS '成品长度';
COMMENT ON COLUMN ompo.tom01.plate_or_coil IS '板卷类型';
COMMENT ON COLUMN ompo.tom01.tol_prop IS '公差比例';
COMMENT ON COLUMN ompo.tom01.lmt_tol_min IS '同板差下限';
COMMENT ON COLUMN ompo.tom01.lmt_tol_max IS '同板差上限';
COMMENT ON COLUMN ompo.tom01.pack_len IS '包装长度';
COMMENT ON COLUMN ompo.tom01.lateral_bend_measure_len IS '侧弯测量长度';
COMMENT ON COLUMN ompo.tom01.lateral_bend_tol IS '侧弯公差';
COMMENT ON COLUMN ompo.tom01.flat_grade IS '平坦度等级';
COMMENT ON COLUMN ompo.tom01.whole_wave_tol IS '全波平坦度公差';
COMMENT ON COLUMN ompo.tom01.center_wave_tol IS '中波平坦度公差';
COMMENT ON COLUMN ompo.tom01.side_wave_plat_tol IS '边波平坦度公差';
COMMENT ON COLUMN ompo.tom01.plat_measure_len IS '平坦度测量长度';
COMMENT ON COLUMN ompo.tom01.warp_len IS '翘曲长度方向';
COMMENT ON COLUMN ompo.tom01.warp_width IS '翘曲宽度方向';
COMMENT ON COLUMN ompo.tom01.edge_w_degree IS '边波急峻度';
COMMENT ON COLUMN ompo.tom01.center_w_degree IS '中波急峻度';
COMMENT ON COLUMN ompo.tom01.edge_wave_num IS '边波个数';
COMMENT ON COLUMN ompo.tom01.edge_wave_high IS '边波浪高';
COMMENT ON COLUMN ompo.tom01.shed_rec_max IS '脱方度最大值';
COMMENT ON COLUMN ompo.tom01.sort_grade_code_f IS '成品分选度代码';
COMMENT ON COLUMN ompo.tom01.hard_aim IS '硬度目标值';
COMMENT ON COLUMN ompo.tom01.ys_stand IS '屈服强度代表值';
COMMENT ON COLUMN ompo.tom01.ts_stand IS '抗拉强度代表值';
COMMENT ON COLUMN ompo.tom01.temper IS '调质度';
COMMENT ON COLUMN ompo.tom01.test_req_ucode IS '试验指示组合码';
COMMENT ON COLUMN ompo.tom01.steel_grade_type IS '钢级大类';
COMMENT ON COLUMN ompo.tom01.si_prod_code IS '硅钢品种代码';
COMMENT ON COLUMN ompo.tom01.apn_desc IS '产品最终用途说明';
COMMENT ON COLUMN ompo.tom01.sample_pos_code IS '取样位置代码';
COMMENT ON COLUMN ompo.tom01.label_std IS '标签用标准';
COMMENT ON COLUMN ompo.tom01.prod_plate_code IS '产品镀层量';
COMMENT ON COLUMN ompo.tom01.prod_label_jis_flag IS '成品标签JIS标志';
COMMENT ON COLUMN ompo.tom01.label_special_remark IS '标签特殊说明';
COMMENT ON COLUMN ompo.tom01.prod_color IS '产品颜色';
COMMENT ON COLUMN ompo.tom01.pack_wt IS '包皮重';
COMMENT ON COLUMN ompo.tom01.mat_strength_grade IS '材料强度等级';
COMMENT ON COLUMN ompo.tom01.idx_no_certi_remark IS '质保书注释索引号';
COMMENT ON COLUMN ompo.tom01.new_prod_class IS '新产品类别';
COMMENT ON COLUMN ompo.tom01.chg_prct_date IS '转产日期';
COMMENT ON COLUMN ompo.tom01.sign_code_1 IS '副钢级代码1';
COMMENT ON COLUMN ompo.tom01.sign_code_2 IS '副钢级代码2';
COMMENT ON COLUMN ompo.tom01.sign_code_3 IS '副钢级代码3';
COMMENT ON COLUMN ompo.tom01.launch_flag IS '下发晚标记';
COMMENT ON COLUMN ompo.tom01.ord_mark_bit IS '合同标记位';
COMMENT ON COLUMN ompo.tom01.ord_mark_revisor IS '合同标记位修改者';
COMMENT ON COLUMN ompo.tom01.ord_mark_time IS '合同标记位修改时间';
COMMENT ON COLUMN ompo.tom01.order_ready_date IS '合同备妥期';
COMMENT ON COLUMN ompo.tom01.trans_mat_flag IS '过渡料标识';
COMMENT ON COLUMN ompo.tom01.line_design_code IS '产线设计代码';
COMMENT ON COLUMN ompo.tom01.hold_flag IS '封锁标记';
COMMENT ON COLUMN ompo.tom01.hold_time IS '封锁时刻';
COMMENT ON COLUMN ompo.tom01.hold_maker IS '封锁责任者';
COMMENT ON COLUMN ompo.tom01.order_end_date IS '合同结案日期';
COMMENT ON COLUMN ompo.tom01.merg_order_no IS '归并合同号';
COMMENT ON COLUMN ompo.tom01.old_order_status IS '原合同状态';
COMMENT ON COLUMN ompo.tom01.hold_flag_bna IS 'BNA封锁标记';
COMMENT ON COLUMN ompo.tom01.retmsg IS '返回信息';
COMMENT ON COLUMN ompo.tom01.unit_wt IS '单位长度重量';
COMMENT ON COLUMN ompo.tom01.dgnl_tol_max IS '对角线公差上限';
COMMENT ON COLUMN ompo.tom01.bend_degree_tot_max IS '全长弯曲度上限';
COMMENT ON COLUMN ompo.tom01.flat_max IS '压扁量上限';
COMMENT ON COLUMN ompo.tom01.flm_slant_width IS '火焰切斜宽向';
COMMENT ON COLUMN ompo.tom01.flm_slant_thick IS '火焰切斜厚向';
COMMENT ON COLUMN ompo.tom01.saw_burr IS '锯切毛刺';
COMMENT ON COLUMN ompo.tom01.saw_slant IS '锯切斜切';
COMMENT ON COLUMN ompo.tom01.dia_tol_min IS '边长下限公差';
COMMENT ON COLUMN ompo.tom01.dia_tol_max IS '边长上限公差';
COMMENT ON COLUMN ompo.tom01.length_of_part IS '测量长度';
COMMENT ON COLUMN ompo.tom01.bend_degr_max IS '弯曲度最大值';
COMMENT ON COLUMN ompo.tom01.label_special_flag IS '标签特殊说明指令';
COMMENT ON COLUMN ompo.tom01.op_status IS '合同计划状态';
COMMENT ON COLUMN ompo.tom01.prod_auth_code IS '产品认证代码';
COMMENT ON COLUMN ompo.tom01.prod_licence_no IS '产品许可证号';
COMMENT ON COLUMN ompo.tom01.spvise_flag IS '监造标记';
COMMENT ON COLUMN ompo.tom01.come_proc_flag IS '来料加工标记';
COMMENT ON COLUMN ompo.tom01.std_code IS '标准代码';
COMMENT ON COLUMN ompo.tom01.order_confirm_flag IS '合同确认标志';
COMMENT ON COLUMN ompo.tom01.order_confirm_maker IS '合同确认责任者';
COMMENT ON COLUMN ompo.tom01.order_confirm_time IS '合同确认时间';
COMMENT ON COLUMN ompo.tom01.big_coil_flag IS '大卷标志';
COMMENT ON COLUMN ompo.tom01.burr_height_max IS '最大毛刺高度';
COMMENT ON COLUMN ompo.tom01.char_code IS '特征码';
COMMENT ON COLUMN ompo.tom01.urg_flag IS '紧急标记（生产用）';
COMMENT ON COLUMN ompo.tom01.center_wave_high IS '中波浪高';
COMMENT ON COLUMN ompo.tom01.tol_spec_flag IS '同板差特殊要求标志';
COMMENT ON COLUMN ompo.tom01.od_div_code IS '产线设计区分代码';
COMMENT ON COLUMN ompo.tom01.hot_fix_flag IS '热轧固定标识';
COMMENT ON COLUMN ompo.tom01.cold_fix_flag IS '冷轧固定标识';
COMMENT ON COLUMN ompo.tom01.finish_fix_flag IS '精整固定标识';
COMMENT ON COLUMN ompo.tom01.order_priority IS '订单优先级';
COMMENT ON COLUMN ompo.tom01.same_kind_flag IS '同类标志';
COMMENT ON COLUMN ompo.tom01.fpc IS '最终产品码';
COMMENT ON COLUMN ompo.tom01.fur_fix_flag IS '退火固定标识';
COMMENT ON COLUMN ompo.tom01.hc_fix_flag IS '热镀固定标识';
COMMENT ON COLUMN ompo.tom01.ec_fix_flag IS '电镀固定标识';
COMMENT ON COLUMN ompo.tom01.spare_time_1 IS '备用时间_1';
COMMENT ON COLUMN ompo.tom01.spare_time_2 IS '备用时间_2';
COMMENT ON COLUMN ompo.tom01.spare_item_1 IS '备用字段_1';
COMMENT ON COLUMN ompo.tom01.spare_item_2 IS '备用字段_2';
COMMENT ON COLUMN ompo.tom01.spare_remark IS '备用备注';
COMMENT ON COLUMN ompo.tom01.vert_thick_tol_minus IS '纵向厚度负公差';
COMMENT ON COLUMN ompo.tom01.vert_thick_tol_plus IS '纵向厚度正公差';