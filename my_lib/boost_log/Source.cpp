#include <iostream>
#include "LogHelper.h"
using namespace std;

void main()
{
	LogHelper helper;
	helper.add_console_log();
	helper.add_common_attributes();
	helper.set_console_sink_filter(1);

	Logger log;
	log.add_attribute("ClassName", "main");
	log.info("this is log");
	log.info_fmt("this is %", "log");
}