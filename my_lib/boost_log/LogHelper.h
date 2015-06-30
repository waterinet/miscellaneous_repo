#pragma once
#include "boost/log/sinks.hpp"
#include "boost/log/attributes.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "Logger.h"

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(ts, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(pid, "ProcessID", boost::log::attributes::current_process_id::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(tid, "ThreadID", boost::log::attributes::current_thread_id::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(clsname, "ClassName", std::string)

boost::log::formatting_ostream& operator<< (boost::log::formatting_ostream& strm, 
    boost::log::to_log_manip<severity_level, tag::severity> const& manip);

class LogHelper
{
public:
	typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> text_ostream_sink;
	typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> text_file_sink;
	typedef boost::shared_ptr<text_ostream_sink> ostream_sink_ptr;
	typedef boost::shared_ptr<text_file_sink> file_sink_ptr;
	void add_console_log();
	void add_file_log();
	void add_common_attributes();
	void set_file_sink_filter(int low_level);
	void set_console_sink_filter(int low_level);
private:
	ostream_sink_ptr console_sink;
	file_sink_ptr file_sink;
};