#pragma once
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/sources/record_ostream.hpp"
#include "boost/log/attributes/constant.hpp"
#include "boost/lexical_cast.hpp"

enum severity_level
{
	trace = 0,
	debug,
	notification,
	warning,
	error
};

class BaseLogger
{
public:
	virtual void add_attribute(std::string attr, std::string val) = 0;
	virtual void write(severity_level lvl, std::string msg) = 0;
	void debug(std::string msg) {
		write(severity_level::debug, msg);
	}
	void info(std::string msg) {
		write(severity_level::notification, msg);
	}
	void error(std::string msg) {
		write(severity_level::error, msg);
	}
	template<typename... Args>
	void debug_fmt(std::string fmt, Args... args) {
		std::string msg = format(fmt.begin(), fmt.end(), args...);
		debug(msg);
	}
	template<typename... Args>
	void info_fmt(std::string fmt, Args... args) {
		std::string msg = format(fmt.begin(), fmt.end(), args...);
		info(msg);
	}
	template<typename... Args>
	void error_fmt(std::string fmt, Args... args) {
		std::string msg = format(fmt.begin(), fmt.end(), args...);
		error(msg);
	}

private:
	std::string format(std::string::iterator beg, std::string::iterator end)
	{
		std::string str;
		while (beg != end)
		{
			if (*beg == '%') {
				if (beg < end - 1 && *(beg + 1) == '%') {
					++beg;
				}
				else {
					throw std::runtime_error(
						"invalid format string: missing arguments");
				}
			}
			str.push_back(*beg++);
		}
		return str;
	}

	template<typename T, typename... Args>
	std::string format(std::string::iterator beg, std::string::iterator end, T val, Args... args)
	{
		std::string str;
		while (beg != end)
		{
			if (*beg == '%') {
				if (beg < end - 1 && *(beg + 1) == '%') {
					++beg;
				}
				else {
					str.append(
						boost::lexical_cast<std::string>(val));
					//参数列表为空时调用log_format的重载函数
					return str.append(format(beg + 1, end, args...));
				}
			}
			str.push_back(*beg++);
		}
		throw std::logic_error("extra arguments provided");
	}
};

class Logger : public BaseLogger
{
	typedef boost::log::sources::severity_logger<severity_level> logger;
public:
	void add_attribute(std::string attr, std::string val) override {
		log.add_attribute(attr, boost::log::attributes::constant<std::string>(val));
	}
	void write(severity_level lvl, std::string msg) override {
		BOOST_LOG_SEV(log, lvl) << msg;
	}
private:
	logger log;
};

class LoggerMT : public BaseLogger
{
	typedef boost::log::sources::severity_logger_mt<severity_level> logger_mt;
public:
	void add_attribute(std::string attr, std::string val) override {
		log.add_attribute(attr, boost::log::attributes::constant<std::string>(val));
	}
	void write(severity_level lvl, std::string msg) override {
		BOOST_LOG_SEV(log, lvl) << msg;
	}
private:
	logger_mt log;
};