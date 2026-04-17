// AlignPosition.h 对齐工位类定义
#pragma once
#include "PositionBase.h"

class CAlignPosition : public CPositionBase
{

public:
    CAlignPosition() = default;
    ~CAlignPosition() = default;

    virtual void UpdateForm() override; // 刷新画面

private:
    const char *REDIS_KEY = "ALIGN_POS_TUBE_INFO"; // 发布的key名称
};