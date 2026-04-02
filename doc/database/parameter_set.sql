-- parameter_set definition

-- DROP TABLE parameter_set;

CREATE TABLE parameter_set (
    order_no varchar(10) DEFAULT '',
    item_no varchar(3) DEFAULT '',
	diameter numeric(5, 2) DEFAULT 0,
	wall_thickness numeric(4, 2) DEFAULT 0,
    direction_code varchar(8) DEFAULT '',
	bundle_type varchar(8) DEFAULT '',
	bundle_number int DEFAULT 0,
    produce_job_point varchar(8) DEFAULT '',
	order_weight int DEFAULT 0,	
    lot_no varchar(8) DEFAULT '',
    roll_no varchar(8) DEFAULT '',
    melt_no varchar(8) DEFAULT '',
    melt_no_coupling varchar(8) DEFAULT '',
    lot_no_coupling varchar(8) DEFAULT '',
    flow_no int DEFAULT 0,
    feed_number int DEFAULT 0,
    length_coupling real DEFAULT 0,
    weight_coupling real DEFAULT 0,
	weight_packaging real DEFAULT 0,
    length_enable int DEFAULT 0,
    weight_enable int DEFAULT 0,
	circle_enable int DEFAULT 0,
    carve_enable int DEFAULT 0,
    spray_enable int DEFAULT 0,
    waste_length_enable int DEFAULT 0,
    waste_weight_enable int DEFAULT 0,
    gun1 int DEFAULT 0,
    gun2 int DEFAULT 0,
    gun3 int DEFAULT 0,
    gun4 int DEFAULT 0,
    gun5 int DEFAULT 0,
    gun_clear int DEFAULT 0,
    circle_time int DEFAULT 0,
    spray_length_type int DEFAULT 0,
    spray_weight_type int DEFAULT 0,
    spray_length_precision int DEFAULT 0,
    spray_weight_precision int DEFAULT 0,	
    weight_limit_max real DEFAULT 0,
    weight_limit_min real DEFAULT 0,
    bundle_first_type int DEFAULT 0,
	bundle_flow_no int DEFAULT 0,
	spray_year_count int DEFAULT 0,
	label_count int DEFAULT 0,
    length_limit_max real DEFAULT 0,
    length_limit_min real DEFAULT 0,
    label_length_type int DEFAULT 0,
    label_weight_type int DEFAULT 0,	
	label_type int DEFAULT 0,
    tube_no int DEFAULT 0,
    qrcode_spray_enable int DEFAULT 0,
	weight_per_meter real DEFAULT 0,
    weight_ew real DEFAULT 0,
);

-- 为各个字段添加注释
COMMENT ON COLUMN parameter_set.order_no IS '当前合同号';
COMMENT ON COLUMN parameter_set.item_no IS '当前项目号';
COMMENT ON COLUMN parameter_set.diameter IS '外径';
COMMENT ON COLUMN parameter_set.wall_thickness IS '壁厚';
COMMENT ON COLUMN parameter_set.direction_code IS '去向';
COMMENT ON COLUMN parameter_set.bundle_type IS '管捆类型';
COMMENT ON COLUMN parameter_set.bundle_number IS '打捆根数';
COMMENT ON COLUMN parameter_set.produce_job_point IS '机组代码';
COMMENT ON COLUMN parameter_set.order_weight IS '订货总量';	
COMMENT ON COLUMN parameter_set.lot_no IS '上料试批号';	
COMMENT ON COLUMN parameter_set.roll_no IS '当前轧批号';
COMMENT ON COLUMN parameter_set.melt_no IS '上料炉号';
COMMENT ON COLUMN parameter_set.melt_no_coupling IS '接箍炉号';
COMMENT ON COLUMN parameter_set.lot_no_coupling IS '接箍批号';
COMMENT ON COLUMN parameter_set.flow_no IS '喷印工位下一根管子流水号';
COMMENT ON COLUMN parameter_set.feed_number IS '上料总根数';
COMMENT ON COLUMN parameter_set.length_coupling IS '保护环长度';
COMMENT ON COLUMN parameter_set.weight_coupling IS '保护环重量';
COMMENT ON COLUMN parameter_set.weight_packging IS '包装材料重量';
COMMENT ON COLUMN parameter_set.length_enable IS '测长允许';
COMMENT ON COLUMN parameter_set.weight_enable IS '称重允许';
COMMENT ON COLUMN parameter_set.circle_enable IS '色环允许';
COMMENT ON COLUMN parameter_set.carve_enable IS '针刻印允许';
COMMENT ON COLUMN parameter_set.spray_enable IS '喷印允许';
COMMENT ON COLUMN parameter_set.waste_length_enable IS '长度判废';
COMMENT ON COLUMN parameter_set.waste_weight_enable IS '重量判废';
COMMENT ON COLUMN parameter_set.gun1 IS '色环喷枪1';
COMMENT ON COLUMN parameter_set.gun2 IS '色环喷枪2';
COMMENT ON COLUMN parameter_set.gun3 IS '色环喷枪3';
COMMENT ON COLUMN parameter_set.gun4 IS '色环喷枪4';
COMMENT ON COLUMN parameter_set.gun5 IS '色环喷枪5';
COMMENT ON COLUMN parameter_set.gun_clear IS '喷枪清洗';
COMMENT ON COLUMN parameter_set.circle_time IS '色环时间';	
COMMENT ON COLUMN parameter_set.spray_length_type IS '喷涂长度格式';
COMMENT ON COLUMN parameter_set.spray_weight_type IS '喷涂重量格式';	
COMMENT ON COLUMN parameter_set.spray_length_precision IS '喷涂长度小数位数';
COMMENT ON COLUMN parameter_set.spray_weight_precision IS '喷涂重量小数位数';	
COMMENT ON COLUMN parameter_set.weight_limit_max IS '管重偏差上限';  
COMMENT ON COLUMN parameter_set.weight_limit_min IS '管重偏差下限'; 
COMMENT ON COLUMN parameter_set.bundle_first_type IS '管捆号首位';	
COMMENT ON COLUMN parameter_set.bundle_flow_no IS '管捆流水号';	
COMMENT ON COLUMN parameter_set.spray_year_count IS '喷印刻印<年>';
COMMENT ON COLUMN parameter_set.label_count IS '管捆标签张数';
COMMENT ON COLUMN parameter_set.length_limit_max IS '判废管长止';  
COMMENT ON COLUMN parameter_set.length_limit_min IS '判废管长起';	
COMMENT ON COLUMN parameter_set.label_length_type IS '标签长度格式';
COMMENT ON COLUMN parameter_set.label_weight_type IS '标签重量格式';  	
COMMENT ON COLUMN parameter_set.label_type IS '标签格式';
COMMENT ON COLUMN parameter_set.tube_no IS '测长工位下一根管号';
COMMENT ON COLUMN parameter_set.qrcode_spray_enable IS '二维码喷印';
COMMENT ON COLUMN parameter_set.weight_per_meter IS '米重';
COMMENT ON COLUMN parameter_set.weight_ew IS 'EW值';
