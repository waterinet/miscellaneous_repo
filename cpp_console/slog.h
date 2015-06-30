#ifndef SLOG_H
#define SLOG_H

#include <stdio.h>
#include <string>
#include <fstream>
#include "stime.h"

namespace pgr {

class simple_logger
{
public:
    simple_logger() : rec_num(0) {}
    simple_logger(const string& path) : rec_num(0) {
        open(path);
    }
    ~simple_logger() {}
    void open(const string& path) {
        _ofs.open(path.data(), std::ios::out | std::ios::app);
    }
    void append(std::string msg, int line=0) {
        std::string time_str = stime::cur_time().to_string();
        _ofs << "[" << ++rec_num << "]" << "[" + time_str << "]"
             << "[" << line << "]" << " -> " << msg << "\n";
    }
private:
    std::ofstream _ofs;
    int rec_num;
};

#define SIMPLE_LOG(logger, msg) logger.append(msg, __LINE__)

} //namespace pgr

#endif // SLOG_H

