#pragma once
#include "boost/program_options.hpp"
#include "boost/optional.hpp"

class ProgramOptions
{
public:
	ProgramOptions();
	~ProgramOptions();
	void add_options();
	bool store(std::string);
	boost::optional<int> get_int(std::string);
	boost::optional<std::string> get_string(std::string);
private:
	std::string config_file;
	boost::program_options::options_description desc_;
	boost::program_options::variables_map map_;
};

