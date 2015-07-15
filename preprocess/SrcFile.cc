#include "SrcFile.h"
#include <fstream>

using namespace std;

SrcFile::SrcFile() :
	_size(0)
{
}

SrcFile::~SrcFile()
{
}

void SrcFile::read(string filename)
{
    fstream fs;
    fs.open(filename.c_str(), ios::in);
    char buf[256];
    int count = 0;
    while (fs.getline(buf, 256)) {
        SrcLine line(buf, filename, ++count);
        _data.push_back(line);
		_size += line.size();
    }
}

// index >= 0 && index <= size - 1
string& SrcFile::operator[](size_t index)
{
	size_t count = 0;
	list<SrcLine>::iterator line_it;
	for (line_it = _data.begin(); line_it != _data.end(); line_it++) {
		if (index < line_it->size()) {
			break;
		}
		else {
		    index -= line_it->size();
		}
	}
    SrcLine::iterator it = line_it->begin();
	while (index-- > 0) it++;
	return *it;
}

size_t SrcFile::size() const
{
	return _size;
}
