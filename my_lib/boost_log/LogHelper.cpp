#include "LogHelper.h"
#include "boost/log/expressions.hpp"
#include "boost/log/utility/setup/console.hpp"
#include "boost/log/utility/setup/file.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"

namespace expr = boost::log::expressions;
namespace attr = boost::log::attributes;
namespace keyw = boost::log::keywords;

void LogHelper::add_console_log()
{
	console_sink = boost::log::add_console_log();
	console_sink->set_formatter
	(
	    expr::stream
	    << "[" << line_id << "] "
	    << "[" << ts << "] "
	    << "<" << severity << "> "
	    << "[" << pid << "] "
	    << "[" << tid << "] "
	    << "[" << clsname << "] "
	    << expr::message
	);
}

void LogHelper::add_file_log()
{
	file_sink = boost::log::add_file_log
	(
	    keyw::target = "logs",
		keyw::file_name = "logs/bus_reader_%N.log",
		keyw::rotation_size = 1024 * 1024,  //rotate files every 5M
		keyw::max_size = 1024 * 1024 * 10,
		keyw::format =
		(
		    expr::stream
		    << "[" << line_id << "] "
		    << "[" << ts << "] "
		    << "<" << severity << "> "
		    << "[" << pid << "] "
		    << "[" << tid << "] "
		    << "[" << clsname << "] "
		    << expr::message
		)
    );
}

void LogHelper::add_common_attributes()
{
	boost::log::add_common_attributes();
}

void LogHelper::set_console_sink_filter(int low_level)
{
	if (console_sink) {
		console_sink->set_filter(severity >= low_level);
	}
}

void LogHelper::set_file_sink_filter(int low_level)
{
	if (file_sink) {
		file_sink->set_filter(severity >= low_level);
	}
}

boost::log::formatting_ostream& operator<<
(
    boost::log::formatting_ostream& strm,
	boost::log::to_log_manip<severity_level, tag::severity> const& manip
)
{
	static const char* strings[] =
	{
		"TRACE",
		"DEBUG",
		"INFOM",
		"WARNN",
		"ERROR"
	};
	severity_level level = manip.get();
	//check index
	if (static_cast<size_t>(level) < sizeof(strings) / sizeof(*strings)) {
		strm << strings[level];
	}
	else {
		strm << static_cast<int>(level);
	}
	return strm;
}