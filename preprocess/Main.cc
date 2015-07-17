#include <iostream>
#include <fstream>
#include "SrcLine.h"
#include "SrcFile.h"

using namespace std;

int main() 
{   
    SrcFile sf;
	sf.read("Main.cc");
	SrcFile::iterator it;
	for (it = sf.begin(); it != sf.end(); it++) {
		cout << *it << " * " << it->size() <<  endl;
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

