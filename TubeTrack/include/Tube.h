#pragma once

#include <iostream>
#include <string>
#include <nlohmann/json.hpp>

using namespace std;

class CTube
{
public:
	CTube() = default;
	CTube(const CTube &) = default;
	~CTube() = default; // 也可以省略这一行，默认析构就是空的
	CTube &operator=(const CTube &) = default;

	std::string convertToJson() const {
		nlohmann::json j;
		j["order_no"] = order_no;
		j["item_no"] = item_no;
		j["roll_no"] = roll_no;
		j["melt_no"] = melt_no;
		j["lot_no"] = lot_no;
		j["tube_no"] = tube_no;
		j["flow_no"] = flow_no;
		j["lotno_coupling"] = lotno_coupling;
		j["meltno_coupling"] = meltno_coupling;
		j["length"] = length;
		j["weight"] = weight;
		j["lengthOk"] = lengthOk;
		j["weightOk"] = weightOk;
		j["bSprayed"] = bSprayed;

		return j.dump(4);
	}

	bool   calib_tube=false;           //样管
	string order_no;    	        //合同号
	string item_no;				//项目号
	string roll_no;				//轧批号
	string melt_no;				//炉号
	string lot_no;				//试批号
	int    tube_no=0;				//管号
	int    flow_no=0;				//流水号
	string lotno_coupling;		//接箍批号
	string meltno_coupling;		//接箍炉号
	float  length=0;          	    //长度(米)
	float  weight=0;				//重量(KG)
	bool   lengthOk=true;             //长度合格
	bool   weightOk=true;             //重量合格
	bool   bSprayed=false;             //是否喷印过
};

std::string trimString(std::string str);
