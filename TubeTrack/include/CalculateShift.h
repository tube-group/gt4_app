#pragma once

#include <ctime>
#include <string>

void CalcShift(const std::tm& dt, std::string& strBh, int& nBh);
void CalcShiftNow(std::string& strBh, int& nBh);