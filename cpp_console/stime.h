#ifndef STIME_H
#define STIME_H

#include <string>
#include <stdio.h>
#include "sstring.h"
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <time.h>
#endif

namespace pgr {

class stime_duration
{
public:
    stime_duration() : _milliseconds(0) {}
    stime_duration(long ms) : _milliseconds(ms) {}
    stime_duration(int h, int m, int s, int ms) {
        _milliseconds =
                ((h * 60 + m) * 60 + s) * 1000 + ms;
    }
    int hours() const {
        return _milliseconds / 3600000;
    }
    int minutes() const {
        return (_milliseconds % 3600000) / 60000;
    }
    int seconds() const {
        return ((_milliseconds % 3600000) % 60000) / 1000;
    }
    int milliseconds() const {
        return ((_milliseconds % 3600000) % 60000) % 1000;
    }
    double total_milliseconds() const {
        return _milliseconds;
    }

    stime_duration& operator+=(const stime_duration& st) {
        _milliseconds += st._milliseconds;
        return *this;
    }

    stime_duration& operator-=(const stime_duration& st) {
        _milliseconds -= st._milliseconds;
        return *this;
    }

    stime_duration operator+(const stime_duration& st) const {
        return stime_duration(_milliseconds + st._milliseconds);
    }

    stime_duration operator-(const stime_duration& st) const {
        return stime_duration(_milliseconds - st._milliseconds);
    }

private:
    long _milliseconds;
};

class stime
{
public:
    stime() :
        hour(0),
        minute(0),
        second(0),
        millisecond(0)
    {}
    stime(int h, int m, int s, int ms) :
        hour(h),
        minute(m),
        second(s),
        millisecond(ms)
    {}
    stime(const std::string& time_str) :
        hour(0),
        minute(0),
        second(0),
        millisecond(0)
    {
        parse(time_str);
    }

    void parse(const std::string& time_str)
    {
        //hh::mm::ss.fff
        std::vector<std::string> parts =
                split(time_str, ":.", true);
        hour = str_to_int(parts[0], 10).second;
        minute = str_to_int(parts[1], 10).second;
        second = str_to_int(parts[2], 10).second;
        millisecond = str_to_int(parts[3], 10).second;
    }

    std::string to_string() {
        char buf[16];
        sprintf(buf, "%02d:%02d:%02d.%03d",
                hour, minute, second, millisecond);
        return buf;
    }

    static stime cur_time()
    {
    #ifdef WIN32
        SYSTEMTIME st;
        GetLocalTime(&st);
        return stime(st.wHour, st.wMinute,
                     st.wSecond, st.wMilliseconds);
    #else
        struct timeval tv;
        struct tm* ts;
        gettimeofday(&tv, NULL);
        ts = localtime(&tv.tv_sec);
        return stime(ts->tm_hour, ts->tm_min,
                     ts->tm_sec, tv.tv_usec / 1000);
    #endif
    }

    stime_duration time_of_day() const {
        return stime_duration(hour, minute, second, millisecond);
    }

    stime_duration operator-(const stime& st) const {
        return time_of_day() - st.time_of_day();
    }

private:
    int hour;
    int minute;
    int second;
    int millisecond;
};

} //namespace pgr

#endif // STIME_H

