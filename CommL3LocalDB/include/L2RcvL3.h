#pragma once

#include "CommL3Context.h"

class L2RcvL3
{
public:
    explicit L2RcvL3(CommL3Context &ctx);
    void Run();

private:
    CommL3Context &ctx;
};
