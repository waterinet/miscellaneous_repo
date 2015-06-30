#ifndef AUTO_PRINTER
#define AUTO_PRINTER

#include <vector>
#include <string>
#include "sstring.h"
#include <iostream>

class auto_printer
{
public:
    ~auto_printer() {
        for (int i = 0; i < _data.size(); i++) {
            std::cout << "[" << i + 1 << "]"
                      << _data[i] << std::endl;
        }
    }
    void append(std::string str) {
        _data.push_back(str);
    }

private:
    std::vector<std::string> _data;
};

class log_printer
{
public:
    static void add(std::string key, long val) {
        std::string str = key + ": " + ::to_string(val);
        _printer.append(str);
    }

private:
    static auto_printer _printer;
};

auto_printer log_printer::_printer;

#endif // AUTO_PRINTER

