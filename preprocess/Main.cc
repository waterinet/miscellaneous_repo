#include <iostream>
#include <fstream>
#include "SrcLine.h"

using namespace std;

int main() 
{
    list<SrcLine> src_data;

    fstream fs;
	fs.open("Main.cc", ios::in);
	char buf[256];
	int count = 0;
	while (fs.getline(buf, 256)) {
        SrcLine line(buf, "Main.cc", ++count);
		src_data.push_back(line);
	}

	if (!src_data.empty()) 
		cout << src_data.front().file_name() << endl;
	while (!src_data.empty()) {
		SrcLine line = src_data.front();
		cout << line.line_number() << " ";
        list<string>::const_iterator it = line.data().begin();
		while (it != line.data().end()) {
			cout << *it << " ";
			++it;
		}
		cout << endl;
		src_data.pop_front();
	}
}

