#include "ProgramOptions.h"
#include <iostream>

namespace po = boost::program_options;

ProgramOptions::ProgramOptions()
{
}


ProgramOptions::~ProgramOptions()
{
}
void ProgramOptions::add_options()
{
	desc_.add_options()
		("num_threads", po::value<int>()->default_value(4), "number of work threads")
		("max_interval", po::value<int>()->default_value(10), "use to filter records")
		("datasource.database", po::value<std::string>()->required(), "oracle database name")
		("datasource.username", po::value<std::string>()->required(), "username")
		("datasource.password", po::value<std::string>()->required(), "password")
		("http_server.ip", po::value<std::string>()->required(), "http server ip address")
		("http_server.port", po::value<int>()->required(), "http server port")
		("http_server.path", po::value<std::string>()->required(), "http url path")
		("http_server.connection_timeout", po::value<int>()->default_value(5), "connection timeout")
		("http_server.transfer_timeout", po::value<int>()->default_value(10), "transfer timeout")
		("http_server.max_connections", po::value<int>()->default_value(20), "max connections with server")
		("log.console.low_level", po::value<int>()->default_value(0), "console logger filter")
		("log.file.low_level", po::value<int>()->default_value(0), "file logger filter");
}
bool ProgramOptions::store(std::string cfg)
{
	try {
		po::store(po::parse_config_file<char>(cfg.c_str(), desc_), map_);
		po::notify(map_);
		return true;
	}
	catch (std::exception& e) {
		std::cout << e.what() << "\n";
		return false;
	}
}
boost::optional<int> ProgramOptions::get_int(std::string key)
{
	try {
		if (map_.count(key)) {
			int val = map_[key].as<int>();
			return boost::optional<int>(val);
		}
	}
	catch (boost::bad_any_cast& e) {
		std::cout << e.what() << "\n";
	}
	return boost::optional<int>();
}
boost::optional<std::string> ProgramOptions::get_string(std::string key)
{
	try {
		if (map_.count(key)) {
			std::string val = map_[key].as<std::string>();
			return boost::optional<std::string>(val);
		}
	}
	catch (boost::bad_any_cast& e) {
		std::cout << e.what() << "\n";
	}
	return boost::optional<std::string>();
}

