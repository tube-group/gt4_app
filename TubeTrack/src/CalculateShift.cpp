// Original C# reference:
/*
static class GlobalModule
{
    //三班二运转 2020-9-3开始
    static private int[] grpno = new int[12] { 11, 23, 11, 22, 13, 22, 13, 21, 12, 21, 12, 23 };
    static private string[] wkgrp1 = new string[3] { "夜", "早", "中" };
    static private string[] wkgrp2 = new string[4] { "甲", "乙", "丙", "丁" };
    static private int[,] daypm = new int[2, 11]{{31,28,31,30,31,30,31,31,30,31,30},
                                                {31,29,31,30,31,30,31,31,30,31,30}};

    static private int DtToDays(int year, int month, int day)
    {
        int mi, m1, m2, i;

        m1 = year - 2000;
        mi = 0;
        for (i = 0; i <= m1 - 1; i++)
        {
            if (i % 4 == 0)
            {
                mi += 366;
            }
            else
            {
                mi += 365;
            }
        }

        m2 = month - 2;
        for (i = 0; i <= m2; i++)
        {
            if (m1 % 4 == 0)
            {
                mi += daypm[1, i];
            }
            else
            {
                mi += daypm[0, i];
            }
        }

        mi += day;
        return mi;
    }

    static public void CalcShift(DateTime dt, out string strBh, out int nBh)
    {
        int mL, m1, tm;

        mL = DtToDays(dt.Year, dt.Month, dt.Day) - DtToDays(2020, 9, 3);
        m1 = (mL % 6) * 2;
        tm = dt.Hour * 10000 + dt.Minute * 100 + dt.Second;
        if (tm >= 74500 && tm < 194500)
        {
            m1 += 1;
        }

        if (tm >= 194500)
        {
            m1 += 2;
            if (m1 > 11)
            {
                m1 = 0;
            }
        }

        strBh = wkgrp1[(grpno[m1] / 10) - 1] + wkgrp2[(grpno[m1] % 10) - 1];
        nBh = grpno[m1];
    }
}
*/

#include "CalculateShift.h"

#include <array>

namespace {

std::tm GetLocalTimeNow()
{
    const std::time_t now = std::time(nullptr);
    std::tm localTime{};

#if defined(_WIN32)
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif

    return localTime;
}

inline constexpr std::array<int, 12> kGrpNo{11, 23, 11, 22, 13, 22, 13, 21, 12, 21, 12, 23};
inline constexpr std::array<const char*, 3> kWkgrp1{"夜", "早", "中"};
inline constexpr std::array<const char*, 4> kWkgrp2{"甲", "乙", "丙", "丁"};
inline constexpr std::array<std::array<int, 11>, 2> kDayPm{{
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30},
        {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30},
    }};

int DtToDays(int year, int month, int day)
{
    const int m1 = year - 2000;
    int mi = 0;

    for (int i = 0; i <= m1 - 1; ++i) {
        if (i % 4 == 0) {
            mi += 366;
        } else {
            mi += 365;
        }
    }

    const int m2 = month - 2;
    for (int i = 0; i <= m2; ++i) {
        if (m1 % 4 == 0) {
            mi += kDayPm[1][i];
        } else {
            mi += kDayPm[0][i];
        }
    }

    mi += day;
    return mi;
}

}  // namespace

void CalcShift(const std::tm& dt, std::string& strBh, int& nBh)
{
    const int mL = DtToDays(dt.tm_year + 1900, dt.tm_mon + 1, dt.tm_mday) - DtToDays(2020, 9, 3);
    int m1 = (mL % 6) * 2;
    const int tm = dt.tm_hour * 10000 + dt.tm_min * 100 + dt.tm_sec;

    if (tm >= 74500 && tm < 194500) {
        m1 += 1;
    }

    if (tm >= 194500) {
        m1 += 2;
        if (m1 > 11) {
            m1 = 0;
        }
    }

    strBh = std::string(kWkgrp1[(kGrpNo[m1] / 10) - 1]) + kWkgrp2[(kGrpNo[m1] % 10) - 1];
    nBh = kGrpNo[m1];
}

void CalcShiftNow(std::string& strBh, int& nBh)
{
    const std::tm localTime = GetLocalTimeNow();
    CalcShift(localTime, strBh, nBh);
}