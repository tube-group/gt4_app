#include <thread>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <atomic>
#include <cstring>
#include <chrono>
#include <unistd.h>
#include <csignal>
#include <utility>

#include "logging.h"
#include "higplat.h"
#include "TubeTrackContext.h"

#include "podstring.h"

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

void DoStatistics(TubeTrackContext &ctx);
