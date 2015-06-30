#include <iostream>
#include "ProgramOptions.h"

void main()
{
	ProgramOptions po;
	po.add_options();
	try {
		po.store("config.txt");
	}
	catch (std::exception& e) {
		std::cout << e.what() << "\n";
	}
	boost::optional<int> num_threads = po.get_int("num_threads");
	if (num_threads) {
		std::cout << "num threads: " << num_threads.get() << "\n";
	}
	boost::optional<std::string> db = po.get_string("datasource.database");
	if (db) {
		std::cout << "database: " << db.get() << "\n";
	}
}