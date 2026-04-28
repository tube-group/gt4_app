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

struct DeleteTubeCmd {
  int seq_no; // 序列号，确保管子的唯一性和顺序
  PodString<20> position_name; // 工位名称
};

struct SetCurrentContractCmd {
  PodString<20> order_no; // 合同号
  PodString<20> item_no; // 项目号
};


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

#endif // USERCMD_H