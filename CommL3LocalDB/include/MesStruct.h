#pragma once

#include <string>
#include <vector>

struct FieldStruct
{
    std::string name;
    std::string description; // 字段描述
    char type;               // 数据类型
    int length;              // 长度
    int accuracy;            // 精度
};

using FieldList = std::vector<FieldStruct>;

inline FieldStruct make_field(const char* name,
                              const char* description,
                              char type,
                              int length = 0,
                              int accuracy = 0)
{
    return FieldStruct{name, description, type, length, accuracy};
}

struct GSPA13
{
    inline static const FieldList fields = {
        make_field("OPERATOR_FLAG", "增删标记", 'C', 1, 0),
        make_field("PROD_DATE", "生产日期", 'C', 8, 0),
        make_field("PROD_TM", "生产时间(6位)", 'C', 6, 0),
        make_field("PROD_SHIFT_NO", "生产班次", 'C', 1, 0),
        make_field("PROD_SHIFT_GROUP", "生产班组", 'C', 1, 0),
        make_field("PROD_JOB_POINT", "生产作业点", 'C', 4, 0),
        make_field("DEST_CODE", "去向代码", 'C', 4, 0),
        make_field("ORDER_NO", "合同号", 'C', 10, 0),
        make_field("ORDER_ITEM_NO", "合同项目号", 'C', 3, 0),
        make_field("PROD_CODE", "品名细分类代码", 'C', 1, 0),
        make_field("PROD_CNAME", "品名细分类中文", 'C', 30, 0),
        make_field("ORDER_OUTER_DIA", "外径", 'D', 6, 2),
        make_field("ORDER_WAL_THICK", "壁厚", 'D', 5, 2),
        make_field("ORDER_HEIGHT", "高度", 'D', 6, 2),
        make_field("STD_SG_CODE", "标准牌号(钢级)代码", 'C', 4, 0),
        make_field("SG_SIGN", "牌号（钢级）", 'C', 50, 0),
        make_field("SG_STD", "牌号标准", 'C', 100, 0),
        make_field("MTRL_NO", "材质号", 'C', 4, 0),
        make_field("MTRL_TEXT", "材质正文", 'C', 50, 0),
        make_field("PIPEEND_TYPE_CODE", "管端型式代码", 'C', 1, 0),
        make_field("PIPEEND_TYPE_SIGN", "管端型式符号", 'C', 12, 0),
        make_field("PIPEEND_TYPE", "管端型式", 'C', 30, 0),
        make_field("THREAD_TYPE_CODE", "螺纹类型代码", 'C', 1, 0),
        make_field("THREAD_TYPE_SIGN", "螺纹类型符号", 'C', 12, 0),
        make_field("THREAD_TYPE", "螺纹类型", 'C', 30, 0),
        make_field("OLD_ORDER_NO", "原合同号", 'C', 10, 0),
        make_field("RL_NO", "轧坯号", 'C', 10, 0),
        make_field("PONO", "制造命令号", 'C', 10, 0),
        make_field("SAMPLE_LOT_NO", "试批号", 'C', 12, 0),
        make_field("MAT_NO", "材料号", 'C', 20, 0),
        make_field("BUNDLE_TYPE", "管捆类型", 'C', 3, 0),
        make_field("MAT_TUBE", "材料根数", 'L', 7, 0),
        make_field("MAT_ACT_WT", "材料重量", 'D', 15, 6),
        make_field("MAT_ENG_WT", "材料英制重量", 'D', 15, 6),
        make_field("TOTAL_LEN", "总长度", 'D', 9, 3),
        make_field("ENG_TOTAL_LEN", "英制总长度", 'D', 9, 3),
        make_field("LEN_FROM", "长度起", 'D', 9, 3),
        make_field("LEN_TO", "长度止", 'D', 9, 3),
        make_field("MAT_THEORY_WT", "材料理论重量", 'D', 15, 6),
        make_field("THY_TOTAL_LEN", "理论总长度", 'L', 9, 0),
        make_field("ORDER_END_FLAG", "合同结束标记", 'C', 1, 0),
        make_field("CTL_TUBE_DIA_FROM", "内控管体外径起", 'D', 7, 3),
        make_field("CTL_TUBE_DIA_TO", "内控管体外径止", 'D', 7, 3),
        make_field("CTL_WAL_THICK_FROM", "内控壁厚起", 'D', 5, 2),
        make_field("CTL_WAL_THICK_TO", "内控壁厚止", 'D', 5, 2),
        make_field("CTL_HEIGHT_FROM", "内控高度起", 'D', 7, 3),
        make_field("CTL_HEIGHT_TO", "内控高度止", 'D', 7, 3),
        make_field("COUP_PONO", "接箍炉号", 'C', 10, 0),
        make_field("COUP_SAMPLE_LOT_NO", "接箍试批号", 'C', 12, 0),
        make_field("PROTECT_SIZE", "保护环规格", 'C', 12, 0),
        make_field("PROTECT_LOT_NO", "保护环批号", 'C', 7, 0),
        make_field("PROTECT_FACTORY", "保护环生产厂家", 'C', 60, 0),
        make_field("COMPOUND_SIZE", "螺纹脂规格", 'C', 12, 0),
        make_field("COMPOUND_LOT_NO", "螺纹脂批号", 'C', 7, 0),
        make_field("COMPOUND_FACTORY", "螺纹脂生产厂家", 'C', 60, 0),
        make_field("COATING_SIZE", "防腐涂层规格", 'C', 12, 0),
        make_field("COATING_LOT_NO", "防腐涂层批号", 'C', 7, 0),
        make_field("COATING_FACTORY", "防腐涂层生产厂家", 'C', 60, 0),
        make_field("STOCK_PLACE_NO", "材料库位号", 'C', 15, 0),
        make_field("TUBE_NO", "管号", 'C', 7, 0),
        make_field("TUBE_ACT_WT", "单根管子重量", 'D', 15, 6),
        make_field("TUBE_ENG_WT", "单根管子英制重量", 'D', 15, 6),
        make_field("TUBE_THEORY_WT", "单根管子理论重量", 'D', 15, 6),
        make_field("TUBE_LEN", "单根管子长度", 'L', 9, 0),
        make_field("TUBE_ENG_LEN", "单根管子英制长度", 'D', 9, 3),
        make_field("TUBE_THY_LEN", "单根管子理论长度", 'L', 9, 0),
        make_field("SEQ_ID", "流水号", 'L', 7, 0),
    };
};

struct GSPA19
{
    inline static const FieldList fields = {
        make_field("OPERATOR_FLAG", "增删标记", 'C', 1, 0),
        make_field("ORDER_NO", "合同号", 'C', 10, 0),
        make_field("MAT_NO", "材料号", 'C', 7, 0),
        make_field("RL_NO", "轧坯号", 'C', 6, 0),
        make_field("PONO", "炉号", 'C', 10, 0),
        make_field("SAMPLE_LOT_NO", "试批号", 'C', 12, 0),
        make_field("ORDER_OUTER_DIA", "外径", 'D', 6, 2),
        make_field("ORDER_WAL_THICK", "壁厚", 'D', 5, 2),
        make_field("SG_SIGN", "牌号（钢级）", 'C', 50, 0),
        make_field("SG_STD", "牌号标准", 'C', 100, 0),
        make_field("PIPEEND_TYPE_SIGN", "管端型式符号", 'C', 12, 0),
        make_field("THREAD_TYPE_SIGN", "螺纹类型符号", 'C', 12, 0),
        make_field("MAT_TUBE", "材料根数", 'L', 7, 0),
        make_field("MAT_ACT_WT", "材料重量", 'D', 15, 6),
        make_field("TOTAL_LEN", "总长度", 'D', 9, 3),
        make_field("TUBE_NO", "管号", 'C', 0, 0),
        make_field("TUBE_LEN", "单根管子长度", 'D', 12, 3),
        make_field("TUBE_ACT_WT", "单根管子重量", 'D', 15, 6),
    };
};