#ifndef SSTRING_H
#define SSTRING_H

#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include <cmath>
#include "numeric_traits.h"

namespace pgr {

std::vector<std::string> split(const std::string& str, const std::string& delims,
                               bool remove_empty=false)
{
    using namespace std;
    vector<string> result;
    if (str.empty())
        return result;
    string sub;
    string::const_iterator it;
    for (it = str.begin(); it != str.end(); it++)
    {
        if (delims.find(*it) == string::npos) {
            sub.push_back(*it);
            continue;
        }
        if (!(remove_empty && sub.empty())) {
            result.push_back(sub);
            sub.clear();
        }
    }
    //last part
    if (!(remove_empty && sub.empty())) {
        result.push_back(sub);
        sub.clear();
    }
    return result;
}

std::string trim(const std::string& src)
{
    typedef std::string::const_iterator it_type;
    it_type beg = src.begin();
    it_type end = src.end();
    it_type it;
    for (it = beg; it != end && isspace(*it); it++);
        //skip leading whitespaces
    if (it == end)
        return std::string();
    beg = it;
    for (it = end - 1; it != beg && isspace(*it); it--);
        //skip trailing whitespaces
    end = it + 1;
    return std::string(beg, end);
}

template<typename T>
std::pair<bool, T> str_to_integer(const std::string& s, int base=0)
{
    typedef std::pair<bool, T> result_type;
    std::string str = trim(s);
    if (str.empty())
        return result_type(false, 0);

    //0 - defualt(positive), 1 - positive, -1 - negative
    int sign = 0;
    if (str[0] == '+')
        sign = 1;
    if (str[0] == '-')
        sign = -1;
    //auto detect base
    if (base == 0) {
        base = 10;
        int off = std::abs(sign);
        if (str.size() - off >= 2) {
            if (str[off] == '0') {
                base = 8;
                if (str[off + 1] == 'x' || str[off + 1] == 'X')
                    base = 16;
            }
        }
    }
    T val;
    std::istringstream iss(str);
    if (base == 16) {
        iss >> std::hex >> val;
    }
    else if (base == 8) {
        iss >> std::oct >> val;
    }
    else {
        iss >> std::dec >> val;
    }

    if (!iss.fail() && iss.eof()) {
        //fix bug of stlport when convert numeric_limits<T>::max()+n, n>0
        if (val < 0 && sign != -1) {
            return result_type(false, 0);
        }
        return result_type(true, val);
    }
    else {
        return result_type(false, 0);
    }
}

std::pair<bool, int> str_to_int(const std::string& s, int base=0)
{
   return str_to_integer<int>(s, base);
}

std::pair<bool, long long> str_to_ll(const std::string& s, int base=0)
{
    return str_to_integer<long long>(s, base);
}

template<typename T>
std::string to_string(T t)
{
    char buf[numeric_traits<T>::max_length];
    sprintf(buf, numeric_traits<T>::fmt_str(), t);
    return buf;
}

//wikipedia - English numerals
std::string to_ordinal(unsigned int num)
{
    std::string str = to_string(num);
    int tens_digit = (num % 100) / 10;
    if (tens_digit == 1) {
        str += "th";
    }
    else {
        int units_digit = num % 10;
        char* table[] = { "th", "st", "nd", "rd",
              "th", "th", "th", "th", "th", "th" };
        str += table[units_digit];
    }
    return str;
}

} //namespace pgr

#endif // SSTRING_H

