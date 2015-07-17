#ifndef SRCFILE_H
#define SRCFILE_H

#include "SrcLine.h"
#include "SrcCIter.h"
#include "SrcIter.h"

class SrcFile
{
public:
	typedef std::list<SrcLine>::iterator line_iterator;
	typedef std::list<SrcLine>::const_iterator const_line_iterator;
    typedef SrcCIter const_iterator;
	typedef SrcIter iterator;

	SrcFile();
	~SrcFile();
	void read(std::string);
	std::string& operator[](size_t index);
	const std::string& operator[](size_t index) const;
	size_t size() const;
	size_t line_count() const;

	line_iterator line_begin();
	line_iterator line_end();
	const_line_iterator line_begin() const;
	const_line_iterator line_end() const;
	
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;
private: 
	size_t _size;
	std::list<SrcLine> _data;
};

#endif
