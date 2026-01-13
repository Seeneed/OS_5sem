//для Windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <iomanip>

int main()
{
    SYSTEMTIME local_st{};
    GetLocalTime(&local_st);

    FILETIME local_ft{}, utc_ft{};
    SystemTimeToFileTime(&local_st, &local_ft);
    LocalFileTimeToFileTime(&local_ft, &utc_ft);

    long long local_ll = (static_cast<long long>(local_ft.dwHighDateTime) << 32) | local_ft.dwLowDateTime;
    long long utc_ll = (static_cast<long long>(utc_ft.dwHighDateTime) << 32) | utc_ft.dwLowDateTime;

    long long offset_hours = (local_ll - utc_ll) / (10000000LL * 3600);

    char sign = (offset_hours >= 0) ? '+' : '-';
    if (offset_hours < 0) {
        offset_hours = -offset_hours;
    }

    std::cout << std::setfill('0')
        << local_st.wYear << '-'
        << std::setw(2) << local_st.wMonth << '-'
        << std::setw(2) << local_st.wDay << 'T'
        << std::setw(2) << local_st.wHour << ':'
        << std::setw(2) << local_st.wMinute << ':'
        << std::setw(2) << local_st.wSecond
        << sign
        << std::setw(2) << offset_hours
        << std::endl;

    return 0;
}

//для Linux
//#include <iostream>
//#include <iomanip>
//#include <ctime>
//#include <cmath>
//
//int main()
//{
//    time_t raw_time;
//    time(&raw_time);
//
//    struct tm local_tm {};
//    struct tm utc_tm {};
//
//    localtime_r(&raw_time, &local_tm);
//    gmtime_r(&raw_time, &utc_tm);
//
//    struct tm utc_copy = utc_tm;
//    utc_copy.tm_isdst = local_tm.tm_isdst;
//
//    time_t utc_converted = mktime(&utc_copy);
//
//    long diff_seconds = raw_time - utc_converted;
//    long offset_hours = diff_seconds / 3600;
//
//    char sign = (offset_hours >= 0) ? '+' : '-';
//    long abs_offset = std::abs(offset_hours);
//
//    std::cout << std::setfill('0')
//        << (local_tm.tm_year + 1900) << '-'
//        << std::setw(2) << (local_tm.tm_mon + 1) << '-'
//        << std::setw(2) << local_tm.tm_mday << 'T'
//        << std::setw(2) << local_tm.tm_hour << ':'
//        << std::setw(2) << local_tm.tm_min << ':'
//        << std::setw(2) << local_tm.tm_sec
//        << sign
//        << std::setw(2) << abs_offset
//        << std::endl;
//
//    return 0;
//}