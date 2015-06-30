#ifndef _HELPER_H_
#define TIME_ACCUMULATOR_H_

#include "stime.h"
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

namespace pgr {

class time_accumulator
{
public:
    typedef std::pair<std::string, std::string> key_type;
    typedef std::pair<stime, stime> data_type;
    typedef std::vector<data_type> data_array_type;
    typedef std::map<key_type, data_array_type> data_map_type;

    typedef std::pair<size_t, stime_duration> result_type;
    typedef std::map<std::string, result_type> result_map_type;
    typedef std::map<std::string, result_map_type> group_result_type;

    void add(const key_type& key, const data_type& val) {
        _data[key].push_back(val);
    }

    void accumulate()
    {
        _result.clear();
        data_map_type::const_iterator mit = _data.begin();
        while (mit != _data.end())
        {
            std::string group_name = mit->first.first;
            std::string item_name = mit->first.second;
            data_array_type records = mit->second;
            stime_duration sum;
            data_array_type::const_iterator vit = records.begin();
            while (vit != records.end()) {
                sum += vit->second - vit->first;
                vit++;
            }
            _result[group_name][item_name] =
                    result_type(records.size(), sum);
            mit++;
        }
    }
    void print(const std::string& path)
    {
        std::ofstream ofs;
        ofs.open(path.data(), std::ios::out);
        group_result_type::const_iterator it = _result.begin();
        while (it != _result.end()) {
            do_print(ofs, it->first, it->second);
            it++;
        }
        ofs.close();
    }

private:
    void do_print(std::ofstream& ofs, std::string group_name, const result_map_type& result)
    {
        ofs << "-----------------------------"
            << group_name
            << "-----------------------------" << std::endl;
        ofs << std::setw(30) << "Item Name"
            << std::setw(20) << "Record Num"
            << std::setw(10) << "Time" << std::endl;
        size_t total_records = 0;
        size_t total_time = 0;
        result_map_type::const_iterator it = result.begin();
        while (it != result.end()) {
            long total_time_of_item =
                    (long)(it->second.second.total_milliseconds());
            ofs << std::setw(30) << it->first
                << std::setw(20) << it->second.first
                << std::setw(10) << total_time_of_item << std::endl;
            total_records += it->second.first;
            total_time += total_time_of_item;
            it++;
        }
        ofs << std::setw(50) << total_records
            << std::setw(10) << total_time << std::endl;
    }

    data_map_type _data;
    group_result_type _result;
};

} //namespace pgr

#endif // TIME_ACCUMULATOR_H_

