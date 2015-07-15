#include <iostream>
#include <fstream>
#include "SrcLine.h"
#include "SrcFile.h"
#include "SrcCIter.h"

using namespace std;

int main() 
{   
	/*
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
	*/
    SrcFile sf;
	sf.read("Main.cc");
	SrcFile::const_iterator cit;
	for (cit = sf.begin(); cit != sf.end(); cit++) {
		cout << *cit << " * " << cit->size() <<  endl;
	}
	/*
	SrcFile::line_iterator line_it;
	SrcLine::iterator it;
	for (line_it = sf.line_begin(); line_it != sf.line_end(); ++line_it) {
        for (it = line_it->begin(); it != line_it->end(); ++it) {
			cout << *it << " ";
		}
		cout << endl;
	}
    */
}

