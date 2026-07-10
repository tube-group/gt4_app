#ifndef USER_TYPES_H_
#define USER_TYPES_H_

#include "struct_reflect.h"
#include "podstring.h"

// ============================================================
// 用户自定义 struct 定义
// 使用方法：
//   1. 用 #pragma pack(push, 8) 定义 struct
//   2. 用 REGISTER_STRUCT 注册元数据
//   3. 在 struct_registry.h 的 GetStructRegistry() 中添加 REG(XXX)
// ============================================================

struct SensorData {
	int32_t       temperature;
	int32_t       humidity;
	double        pressure;
	bool          alarm;
	PodString<20> location;
};

REGISTER_STRUCT(SensorData,
	FIELD_DESC(Int32,   SensorData, temperature),
	FIELD_DESC(Int32,   SensorData, humidity),
	FIELD_DESC(Double,  SensorData, pressure),
	FIELD_DESC(Boolean, SensorData, alarm),
	FIELD_DESC_STRING(SensorData, location)
)

struct MotorStatus {
	float         speed[3];
	float         current;
	int32_t       error_code;
	uint32_t      run_count;
	PodString<16> motor_name[3];
};

REGISTER_STRUCT(MotorStatus,
	FIELD_DESC_ARRAY(Single,  MotorStatus, speed, 3),
	FIELD_DESC(Single,  MotorStatus, current),
	FIELD_DESC(Int32,   MotorStatus, error_code),
	FIELD_DESC(UInt32,  MotorStatus, run_count),
	FIELD_DESC_STRING_ARRAY(MotorStatus, motor_name, 3)
)

// 嵌套 struct 示例（内层：仅含基本类型，供外层引用）

struct GPSPosition {
	double latitude;
	double longitude;
};

REGISTER_STRUCT(GPSPosition,
	FIELD_DESC(Double, GPSPosition, latitude),
	FIELD_DESC(Double, GPSPosition, longitude)
)

struct Vehicle {
	int32_t       id;
	GPSPosition   pos;
	GPSPosition   history[3];
	PodString<16> plate;
};

REGISTER_STRUCT(Vehicle,
	FIELD_DESC(Int32, Vehicle, id),
	FIELD_DESC_STRUCT(Vehicle, pos, GPSPosition),
	FIELD_DESC_STRUCT_ARRAY(Vehicle, history, GPSPosition, 3),
	FIELD_DESC_STRING(Vehicle, plate)
)

//下面是GT4_APP项目的用户自定义struct定义

//设定生产计划命令
struct SetFeedNumCmd
{
	int feed_num; // 投料支数
};

REGISTER_STRUCT(SetFeedNumCmd,
	FIELD_DESC(Int32, SetFeedNumCmd, feed_num)
)

//移动管子命令
struct MoveTubeCmd
{
	PodString<20> from; // 来源工位
	PodString<20> to;   // 目标工位
};

REGISTER_STRUCT(MoveTubeCmd,
	FIELD_DESC_STRING(MoveTubeCmd, from),
	FIELD_DESC_STRING(MoveTubeCmd, to)
)

struct ModifyTubeCmd {
	int seq_no; // 序列号，确保管子的唯一性和顺序
	PodString<20> position_name; // 工位名称
	PodString<20> order_no; // 合同号
	PodString<20> item_no; // 项目号
	PodString<20> roll_no; // 轧批号
	PodString<20> melt_no; // 炉号
	PodString<20> lot_no; // 试批号
	int tube_no; // 管号
	int flow_no; // 流水号
	double length; // 长度(米)
	double weight; // 重量(KG)
	bool length_ok; // 长度合格
	bool weight_ok; // 重量合格
	PodString<20> lotno_coupling; // 接箍批号
	PodString<20> meltno_coupling; // 接箍炉号
};

REGISTER_STRUCT(ModifyTubeCmd,
	FIELD_DESC(Int32, ModifyTubeCmd, seq_no),
	FIELD_DESC_STRING(ModifyTubeCmd, position_name),
	FIELD_DESC_STRING(ModifyTubeCmd, order_no),
	FIELD_DESC_STRING(ModifyTubeCmd, item_no),
	FIELD_DESC_STRING(ModifyTubeCmd, roll_no),
	FIELD_DESC_STRING(ModifyTubeCmd, melt_no),
	FIELD_DESC_STRING(ModifyTubeCmd, lot_no),
	FIELD_DESC(Int32, ModifyTubeCmd, tube_no),
	FIELD_DESC(Int32, ModifyTubeCmd, flow_no),
	FIELD_DESC(Double, ModifyTubeCmd, length),
	FIELD_DESC(Double, ModifyTubeCmd, weight),
	FIELD_DESC(Boolean, ModifyTubeCmd, length_ok),
	FIELD_DESC(Boolean, ModifyTubeCmd, weight_ok),
	FIELD_DESC_STRING(ModifyTubeCmd, lotno_coupling),
	FIELD_DESC_STRING(ModifyTubeCmd, meltno_coupling)
)

struct DeleteTubeCmd {
	int seq_no; // 序列号，确保管子的唯一性和顺序
	PodString<20> position_name; // 工位名称
};

REGISTER_STRUCT(DeleteTubeCmd,
	FIELD_DESC(Int32, DeleteTubeCmd, seq_no),
	FIELD_DESC_STRING(DeleteTubeCmd, position_name)
)


struct SetCurrentContractCmd {
  PodString<20> order_no; // 合同号
  PodString<20> item_no; // 项目号
};

REGISTER_STRUCT(SetCurrentContractCmd,
	FIELD_DESC_STRING(SetCurrentContractCmd, order_no),
	FIELD_DESC_STRING(SetCurrentContractCmd, item_no)
)

struct StartSprayEvent
{
    PodString<20> order_no; // 合同号
    PodString<20> item_no; // 项目号
    int tube_no; // 管号
    int flow_no; // 流水号
    PodString<20> melt_no; // 炉号
    PodString<20> lot_no; // 试批号
    double length; // 长度(米)
    double weight; // 重量(KG)
    double theory_weight; // 理论重量
    bool length_ok; // 长度合格
    bool weight_ok; // 重量合格
};

REGISTER_STRUCT(StartSprayEvent,
	FIELD_DESC_STRING(StartSprayEvent, order_no),
	FIELD_DESC_STRING(StartSprayEvent, item_no),
	FIELD_DESC(Int32, StartSprayEvent, tube_no),
	FIELD_DESC(Int32, StartSprayEvent, flow_no),
	FIELD_DESC_STRING(StartSprayEvent, melt_no),
	FIELD_DESC_STRING(StartSprayEvent, lot_no),
	FIELD_DESC(Double, StartSprayEvent, length),
	FIELD_DESC(Double, StartSprayEvent, weight),
	FIELD_DESC(Double, StartSprayEvent, theory_weight),
	FIELD_DESC(Boolean, StartSprayEvent, length_ok),
	FIELD_DESC(Boolean, StartSprayEvent, weight_ok)
)


struct AddTubeCmd
{
  int seq_no; // 在此位置前插入管子，0表示第一个位置，1表示第二个位置，以此类推，-1代表在末尾添加
  PodString<20> position_name; // 工位名称
};

REGISTER_STRUCT(AddTubeCmd,
	FIELD_DESC(Int32, AddTubeCmd, seq_no),
	FIELD_DESC_STRING(AddTubeCmd, position_name)
)

struct TagPrintEvent
{
  PodString<20> order_no; // 合同号
  PodString<20> item_no;  // 项目号
  PodString<20> bundle_no; // 捆号
  int count;
};

REGISTER_STRUCT(TagPrintEvent,
	FIELD_DESC_STRING(TagPrintEvent, order_no),
	FIELD_DESC_STRING(TagPrintEvent, item_no),
	FIELD_DESC_STRING(TagPrintEvent, bundle_no),
	FIELD_DESC(Int32, TagPrintEvent, count)
)

struct YieldStatistics
{
	PodString<16> order_no; // 合同号
	PodString<8> item_no; // 项目号
	PodString<16> melt_no; // 炉号
	PodString<8> lot_no; // 试批号
	float diameter;
	float thickness;
	float order_weight;
	float order_length;
	int order_count;
	float order_weight_correct;
	float order_length_correct;
	int order_count_correct;
	float lot_weight;
	float lot_length;
	int lot_count;
	float shift_weight;
	float shift_length;
	int shift_count;
};

REGISTER_STRUCT(YieldStatistics,
	FIELD_DESC_STRING(YieldStatistics, order_no),
	FIELD_DESC_STRING(YieldStatistics, item_no),
	FIELD_DESC_STRING(YieldStatistics, melt_no),
	FIELD_DESC_STRING(YieldStatistics, lot_no),
	FIELD_DESC(Single, YieldStatistics, diameter),
	FIELD_DESC(Single, YieldStatistics, thickness),
	FIELD_DESC(Single, YieldStatistics, order_weight),
	FIELD_DESC(Single, YieldStatistics, order_length),
	FIELD_DESC(Int32, YieldStatistics, order_count),
	FIELD_DESC(Single, YieldStatistics, order_weight_correct),
	FIELD_DESC(Single, YieldStatistics, order_length_correct),
	FIELD_DESC(Int32, YieldStatistics, order_count_correct),
	FIELD_DESC(Single, YieldStatistics, lot_weight),
	FIELD_DESC(Single, YieldStatistics, lot_length),
	FIELD_DESC(Int32, YieldStatistics, lot_count),
	FIELD_DESC(Single, YieldStatistics, shift_weight),
	FIELD_DESC(Single, YieldStatistics, shift_length),
	FIELD_DESC(Int32, YieldStatistics, shift_count)
)

struct RequestOrderDataCmd
{
	PodString<10> order_no; // 合同号
	PodString<3> item_no; // 项目号
};

REGISTER_STRUCT(RequestOrderDataCmd,
	FIELD_DESC_STRING(RequestOrderDataCmd, order_no),
	FIELD_DESC_STRING(RequestOrderDataCmd, item_no)
)

struct OrderData {
	PodString<10> order_no; // 合同号
	PodString<3> item_no; // 项目号
	PodString<6> roll_no; // 轧批号
	double diameter; // 外径
	double wall_thickness; // 壁厚
	PodString<1> prod_code; // 品名细分类代码
	PodString<30> prod_cname; // 品名细分类
	PodString<1> heat_treat_code; // 热处理方式代码
	PodString<40> heat_treat_text; // 热处理方式正文
	PodString<4> std_sg_code; // 标准钢级代码
	PodString<100> std_text; // 标准正文
	PodString<50> sg_text; // 钢级正文
	PodString<4> mat_no; // 材质号
	PodString<20> mat_text; // 材质正文
	PodString<1> thread_type_code; // 螺纹类型代码
	PodString<12> thread_type_sign; // 螺纹类型符号
	PodString<1> end_type_code; // 管端类型代码
	PodString<6> end_type_sign; // 管端类型符号
	PodString<1> coupling_type_code; // 接箍类型代码
	PodString<12> coupling_type_sign; // 接箍类型符号
	PodString<1> thread_face_treat_mode_code; // 螺纹表面处理方式代码
	PodString<20> thread_face_treat_mode; // 螺纹表面处理方式
	double length_from; // 订货长度起
	double length_to; // 订货长度止
	PodString<2> order_unit_code; // 订货计量单位代码
	PodString<4> order_unit; // 订货计量单位
	double order_qty; // 订货数量
	int order_tube; // 订货根数
	double order_weight; // 订货重量
	double fixed_order_weight; // 定尺订货重量
	double unfixed_order_weight; // 非定尺订货重量
	PodString<2> delivery_tolerance_code; // 提货公差单位代码
	PodString<4> delivery_tolerance_unit; // 提货公差单位
	int delivery_tolerance_from; // 提货公差起
	int delivery_tolerance_to; // 提货公差至
	int short_rate; // 短尺率
	double short_from; // 短尺长度起
	double short_to; // 短尺长度至
	int single_bundle_weight_max; // 最大单捆重量
	int single_bundle_tube_max; // 最大单捆根数
	PodString<1> oil_code; // 涂油代码
	PodString<50> oil_type; // 涂油正文
	PodString<300> stamp_req; // 压印要求
	PodString<400> stencil_req; // 喷印要求
	PodString<50> label_req_1; // 标签要求1
	PodString<50> label_req_2; // 标签要求2
	PodString<50> label_req_3; // 标签要求3
	PodString<50> label_req_4; // 标签要求4
	PodString<50> label_req_5; // 标签要求5
	PodString<50> label_req_6; // 标签要求6
	PodString<50> label_req_7; // 标签要求7
	PodString<50> label_req_8; // 标签要求8
	PodString<100> qual_special_req; // 质量特殊要求
	PodString<100> produce_special_req; // 生产特殊要求
	double std_pressure_mpa; // 标准水压压力（MPA)
	double std_pressure_psi; // 标准水压压力 (PSI)
	int stabilivolt_time_min; // 最小稳压时间
	PodString<1> anneal_flag; // 退火标志
	double weight_per_meter; // 米重
	double weight_ew; // EW值
	double theory_weight_eng; // 名义重量
	PodString<10> order_no_old; // 原合同号
	PodString<50> color_circle; // 色环
	PodString<20> color_circle_pos; // 色环位置
	int finish_number; // 完成根数
	double finish_length; // 完成长度
	double finish_weight; // 完成重量
	int finish_number_sh; // 短尺完成根数
	double finish_length_sh; // 短尺完成长度
	double finish_weight_sh; // 短尺完成重量
	int last_flow_no; // 最后流水号
	PodString<400> stencil_req_manual; // 人工喷印要求
	PodString<50> label_req_1_manual; // 人工自由格式打印要求1
	PodString<50> label_req_2_manual; // 人工自由格式打印要求2
	PodString<50> label_req_3_manual; // 人工自由格式打印要求3
	PodString<50> label_req_4_manual; // 人工自由格式打印要求4
	PodString<50> label_req_5_manual; // 人工自由格式打印要求5
	PodString<50> label_req_6_manual; // 人工自由格式打印要求6
	PodString<50> label_req_7_manual; // 人工自由格式打印要求7
	PodString<50> label_req_8_manual; // 人工自由格式打印要求8
	PodString<4> colour; // 标签颜色
	PodString<19> toc; // 修改时间
	PodString<1> select_flag; // 选择标志: 0-未选, 1-已选
	double order_qty_l2; // L2订货数量
	double rolling_diameter; // 轧制外径
	double rolling_thickness; // 轧制壁厚
	PodString<1> weight_mode_code; // 计重方式代码
	PodString<4> weight_mode_text; // 计重方式正文
	double p_weight_tolerance_ul; // 单根重量允差上限
	double p_weight_tolerance_ll; // 单根重量允差下限
	double weight_req_flatside_wpe; // 称重要求平端WPE
	double control_lenght_ul; // 内控长度上限
	double control_lenght_ll; // 内控长度下限
	int label_length_type; // 实验室长度类型: 0-无, 1-有
	int label_weight_type; // 实验室重量类型: 0-无, 1-有
	int label_type; // EMF标签类型: 0-无, 1-有
	double height; // 高度 (mm)
	PodString<12> end_type; // 管端类型
	PodString<30> coupling_type; // 接箍类型
	PodString<6> mic_no; // MIC编号
	double diameter_down_ctrl; // 外径下限_内控
	double diameter_up_ctrl; // 外径上限_内控
	double wal_thick_down_ctrl; // 壁厚下限_内控
	double wal_thick_up_ctrl; // 壁厚上限_内控
	double height_down_ctrl; // 高度下限_内控
	double height_up_ctrl; // 高度上限_内控
	PodString<1> length_grade_code; // 长度等级编码
	PodString<20> length_grade; // 长度等级
	PodString<30> thread_type; // 螺纹类型
	PodString<80> stamp_req_1_manual; // 针刻印要求1
	PodString<80> stamp_req_2_manual; // 针刻印要求2
	PodString<80> stamp_req_3_manual; // 针刻印要求3
	PodString<80> stamp_req_4_manual; // 针刻印要求4
	PodString<80> stamp_req_5_manual; // 针刻印要求5
	PodString<80> stamp_req_6_manual; // 针刻印要求6
	PodString<80> stamp_req_7_manual; // 针刻印要求7
	PodString<80> stamp_req_8_manual; // 针刻印要求8
};

REGISTER_STRUCT(OrderData,
	FIELD_DESC_STRING(OrderData, order_no),
	FIELD_DESC_STRING(OrderData, item_no),
	FIELD_DESC_STRING(OrderData, roll_no),
	FIELD_DESC(Double, OrderData, diameter),
	FIELD_DESC(Double, OrderData, wall_thickness),
	FIELD_DESC_STRING(OrderData, prod_code),
	FIELD_DESC_STRING(OrderData, prod_cname),
	FIELD_DESC_STRING(OrderData, heat_treat_code),
	FIELD_DESC_STRING(OrderData, heat_treat_text),
	FIELD_DESC_STRING(OrderData, std_sg_code),
	FIELD_DESC_STRING(OrderData, std_text),
	FIELD_DESC_STRING(OrderData, sg_text),
	FIELD_DESC_STRING(OrderData, mat_no),
	FIELD_DESC_STRING(OrderData, mat_text),
	FIELD_DESC_STRING(OrderData, thread_type_code),
	FIELD_DESC_STRING(OrderData, thread_type_sign),
	FIELD_DESC_STRING(OrderData, end_type_code),
	FIELD_DESC_STRING(OrderData, end_type_sign),
	FIELD_DESC_STRING(OrderData, coupling_type_code),
	FIELD_DESC_STRING(OrderData, coupling_type_sign),
	FIELD_DESC_STRING(OrderData, thread_face_treat_mode_code),
	FIELD_DESC_STRING(OrderData, thread_face_treat_mode),
	FIELD_DESC(Double, OrderData, length_from),
	FIELD_DESC(Double, OrderData, length_to),
	FIELD_DESC_STRING(OrderData, order_unit_code),
	FIELD_DESC_STRING(OrderData, order_unit),
	FIELD_DESC(Double, OrderData, order_qty),
	FIELD_DESC(Int32, OrderData, order_tube),
	FIELD_DESC(Double, OrderData, order_weight),
	FIELD_DESC(Double, OrderData, fixed_order_weight),
	FIELD_DESC(Double, OrderData, unfixed_order_weight),
	FIELD_DESC_STRING(OrderData, delivery_tolerance_code),
	FIELD_DESC_STRING(OrderData, delivery_tolerance_unit),
	FIELD_DESC(Int32, OrderData, delivery_tolerance_from),
	FIELD_DESC(Int32, OrderData, delivery_tolerance_to),
	FIELD_DESC(Int32, OrderData, short_rate),
	FIELD_DESC(Double, OrderData, short_from),
	FIELD_DESC(Double, OrderData, short_to),
	FIELD_DESC(Int32, OrderData, single_bundle_weight_max),
	FIELD_DESC(Int32, OrderData, single_bundle_tube_max),
	FIELD_DESC_STRING(OrderData, oil_code),
	FIELD_DESC_STRING(OrderData, oil_type),
	FIELD_DESC_STRING(OrderData, stamp_req),
	FIELD_DESC_STRING(OrderData, stencil_req),
	FIELD_DESC_STRING(OrderData, label_req_1),
	FIELD_DESC_STRING(OrderData, label_req_2),
	FIELD_DESC_STRING(OrderData, label_req_3),
	FIELD_DESC_STRING(OrderData, label_req_4),
	FIELD_DESC_STRING(OrderData, label_req_5),
	FIELD_DESC_STRING(OrderData, label_req_6),
	FIELD_DESC_STRING(OrderData, label_req_7),
	FIELD_DESC_STRING(OrderData, label_req_8),
	FIELD_DESC_STRING(OrderData, qual_special_req),
	FIELD_DESC_STRING(OrderData, produce_special_req),
	FIELD_DESC(Double, OrderData, std_pressure_mpa),
	FIELD_DESC(Double, OrderData, std_pressure_psi),
	FIELD_DESC(Int32, OrderData, stabilivolt_time_min),
	FIELD_DESC_STRING(OrderData, anneal_flag),
	FIELD_DESC(Double, OrderData, weight_per_meter),
	FIELD_DESC(Double, OrderData, weight_ew),
	FIELD_DESC(Double, OrderData, theory_weight_eng),
	FIELD_DESC_STRING(OrderData, order_no_old),
	FIELD_DESC_STRING(OrderData, color_circle),
	FIELD_DESC_STRING(OrderData, color_circle_pos),
	FIELD_DESC(Int32, OrderData, finish_number),
	FIELD_DESC(Double, OrderData, finish_length),
	FIELD_DESC(Double, OrderData, finish_weight),
	FIELD_DESC(Int32, OrderData, finish_number_sh),
	FIELD_DESC(Double, OrderData, finish_length_sh),
	FIELD_DESC(Double, OrderData, finish_weight_sh),
	FIELD_DESC(Int32, OrderData, last_flow_no),
	FIELD_DESC_STRING(OrderData, stencil_req_manual),
	FIELD_DESC_STRING(OrderData, label_req_1_manual),
	FIELD_DESC_STRING(OrderData, label_req_2_manual),
	FIELD_DESC_STRING(OrderData, label_req_3_manual),
	FIELD_DESC_STRING(OrderData, label_req_4_manual),
	FIELD_DESC_STRING(OrderData, label_req_5_manual),
	FIELD_DESC_STRING(OrderData, label_req_6_manual),
	FIELD_DESC_STRING(OrderData, label_req_7_manual),
	FIELD_DESC_STRING(OrderData, label_req_8_manual),
	FIELD_DESC_STRING(OrderData, colour),
	FIELD_DESC_STRING(OrderData, toc),
	FIELD_DESC_STRING(OrderData, select_flag),
	FIELD_DESC(Double, OrderData, order_qty_l2),
	FIELD_DESC(Double, OrderData, rolling_diameter),
	FIELD_DESC(Double, OrderData, rolling_thickness),
	FIELD_DESC_STRING(OrderData, weight_mode_code),
	FIELD_DESC_STRING(OrderData, weight_mode_text),
	FIELD_DESC(Double, OrderData, p_weight_tolerance_ul),
	FIELD_DESC(Double, OrderData, p_weight_tolerance_ll),
	FIELD_DESC(Double, OrderData, weight_req_flatside_wpe),
	FIELD_DESC(Double, OrderData, control_lenght_ul),
	FIELD_DESC(Double, OrderData, control_lenght_ll),
	FIELD_DESC(Int32, OrderData, label_length_type),
	FIELD_DESC(Int32, OrderData, label_weight_type),
	FIELD_DESC(Int32, OrderData, label_type),
	FIELD_DESC(Double, OrderData, height),
	FIELD_DESC_STRING(OrderData, end_type),
	FIELD_DESC_STRING(OrderData, coupling_type),
	FIELD_DESC_STRING(OrderData, mic_no),
	FIELD_DESC(Double, OrderData, diameter_down_ctrl),
	FIELD_DESC(Double, OrderData, diameter_up_ctrl),
	FIELD_DESC(Double, OrderData, wal_thick_down_ctrl),
	FIELD_DESC(Double, OrderData, wal_thick_up_ctrl),
	FIELD_DESC(Double, OrderData, height_down_ctrl),
	FIELD_DESC(Double, OrderData, height_up_ctrl),
	FIELD_DESC_STRING(OrderData, length_grade_code),
	FIELD_DESC_STRING(OrderData, length_grade),
	FIELD_DESC_STRING(OrderData, thread_type),
	FIELD_DESC_STRING(OrderData, stamp_req_1_manual),
	FIELD_DESC_STRING(OrderData, stamp_req_2_manual),
	FIELD_DESC_STRING(OrderData, stamp_req_3_manual),
	FIELD_DESC_STRING(OrderData, stamp_req_4_manual),
	FIELD_DESC_STRING(OrderData, stamp_req_5_manual),
	FIELD_DESC_STRING(OrderData, stamp_req_6_manual),
	FIELD_DESC_STRING(OrderData, stamp_req_7_manual),
	FIELD_DESC_STRING(OrderData, stamp_req_8_manual)
)
#endif // USER_TYPES_H_
