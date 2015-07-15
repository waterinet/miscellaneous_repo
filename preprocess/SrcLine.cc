#include "SrcLine.h"
#include "sstring.h"
#include <vector>

using namespace std;

SrcLine::SrcLine()
{
}

SrcLine::SrcLine(const string& str, string file, int line) :
	_file(file), _line(line)
{
	vector<string> parts = pgr::split(str, " ", true);
    _data = list<string>(parts.begin(), parts.end());
}

SrcLine::~SrcLine()
{
}

const list<string>& SrcLine::data() const
{
    return _data;
}

string SrcLine::file_name() const
{
	return _file;
}

int SrcLine::line_number() const
{
	return _line;
}

size_t SrcLine::size() const
{
	return _data.size();
}

SrcLine::iterator SrcLine::begin()
{
	return _data.begin();
}

SrcLine::iterator SrcLine::end()
{
	return _data.end();
}

SrcLine::const_iterator SrcLine::begin() const
{
	return _data.begin();
}

SrcLine::const_iterator SrcLine::end() const
{
	return _data.end();
}
