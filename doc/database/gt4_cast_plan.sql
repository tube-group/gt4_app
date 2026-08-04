CREATE TABLE gt4_cast_plan (
    ORDER_NO           CHAR(10)      NOT NULL,    -- 合同号
    BUNDLE_NO          CHAR(7)       NOT NULL,    -- 材料号
    ROLL_NO            CHAR(6),                   -- 轧坯号
    MELT_NO            CHAR(6),                   -- 炉号
    LOT_NO             CHAR(12),                  -- 试批号
    ORDER_OUTER_DIA    NUMERIC(6,2),              -- 外径
    ORDER_WAL_THICK    NUMERIC(5,2),              -- 壁厚
    SG_SIGN            VARCHAR(50),               -- 牌号（钢级）
    SG_STD             VARCHAR(100),              -- 牌号标准
    PIPEEND_TYPE_SIGN  CHAR(12),                  -- 管端型式符号
    THREAD_TYPE_SIGN   CHAR(12),                  -- 螺纹类型符号
    MAT_TUBE           INTEGER,                   -- 材料根数
    MAT_ACT_WT         NUMERIC(15,6),             -- 材料重量
    TOTAL_LEN          NUMERIC(9,3),              -- 总长度
    CONSTRAINT pk_gt4_cast_plan PRIMARY KEY (order_no, bundle_no)
);

COMMENT ON TABLE gt4_cast_plan IS '投料计划表';
COMMENT ON COLUMN gt4_cast_plan.ORDER_NO IS '合同号';
COMMENT ON COLUMN gt4_cast_plan.BUNDLE_NO IS '管捆号';
COMMENT ON COLUMN gt4_cast_plan.ROLL_NO IS '轧批号';
COMMENT ON COLUMN gt4_cast_plan.MELT_NO IS '炉号';
COMMENT ON COLUMN gt4_cast_plan.LOT_NO IS '试批号';
COMMENT ON COLUMN gt4_cast_plan.ORDER_OUTER_DIA IS '外径';
COMMENT ON COLUMN gt4_cast_plan.ORDER_WAL_THICK IS '壁厚';
COMMENT ON COLUMN gt4_cast_plan.SG_SIGN IS '牌号（钢级）';
COMMENT ON COLUMN gt4_cast_plan.SG_STD IS '牌号标准';
COMMENT ON COLUMN gt4_cast_plan.PIPEEND_TYPE_SIGN IS '管端型式符号';
COMMENT ON COLUMN gt4_cast_plan.THREAD_TYPE_SIGN IS '螺纹类型符号';
COMMENT ON COLUMN gt4_cast_plan.MAT_TUBE IS '材料根数';
COMMENT ON COLUMN gt4_cast_plan.MAT_ACT_WT IS '材料重量';
COMMENT ON COLUMN gt4_cast_plan.TOTAL_LEN IS '总长度';