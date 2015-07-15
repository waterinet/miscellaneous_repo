#ifndef SRCFILE_H
#define SRCFILE_H

#include "SrcLine.h"

class SrcFile
{
public:
	SrcFile();
	~SrcFile();
	void read(std::string);
	std::string& operator[](size_t index);
	size_t size() const;
private: 
	size_t _size;
	std::list<SrcLine> _data;
};

#endif
