//这个头文件定义了用户命令结构体和相关函数，每个结构体和一个TypeScript接口对应
#ifndef USERCMD_H
#define USERCMD_H

#include "podstring.h"

//设定投料支数命令
// export interface SetFeedNumCmd {
//   feed_num: number; // 投料支数
// }

//设定生产计划命令
struct SetFeedNumCmd
{
    int feed_num; // 投料支数
};

//移动管子命令
//plan:投料虚拟工位
//align:对齐工位
//weight：称重工位
//carve:刻印工位
//spray:喷码工位
//circle:色环工位
//scraptroller:出废辊道工位
//scrapt:废料台架工位
//backbuffer:打包前缓冲区工位
//basket:打包区工位
// export interface MoveTubeCmd {
//   from: string;
//   to: string;
// }
struct MoveTubeCmd
{    
    PodString<20> from; // 来源工位
    PodString<20> to;   // 目标工位
};




#endif // USERCMD_H