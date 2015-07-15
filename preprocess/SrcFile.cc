#include "SrcFile.h"
#include "SrcCIter.h"
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

const string& SrcFile::operator[](size_t index) const
{
    list<SrcLine>::const_iterator line_it;
	for (line_it = _data.begin(); line_it != _data.end(); line_it++) {
		if (index < line_it->size()) {
			break;
		}
		else {
			index -= line_it->size();
		}
	}
	SrcLine::const_iterator it = line_it->begin();
    while (index-- > 0) it++;
	return *it;
}

size_t SrcFile::size() const
{
	return _size;
}

size_t SrcFile::line_count() const
{
	return _data.size();
}

SrcFile::line_iterator SrcFile::line_begin()
{
	return _data.begin();
}

SrcFile::line_iterator SrcFile::line_end()
{
	return _data.end();
}

SrcFile::const_line_iterator SrcFile::line_begin() const
{
	return _data.begin();
}

SrcFile::const_line_iterator SrcFile::line_end() const
{
	return _data.end();
}

SrcFile::const_iterator SrcFile::begin() const
{
	return SrcCIter(this, 0);
}

SrcFile::const_iterator SrcFile::end() const
{
	return SrcCIter(this, _size);
}
