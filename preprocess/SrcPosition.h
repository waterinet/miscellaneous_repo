#ifndef SRC_POSITION_H
#define SRC_POSITION_H

#include "SrcLine.h"

class SrcPosition
{
public:
	typedef std::string value_type;
	typedef std::ptrdiff_t difference_type;
	typedef std::string& reference;
	typedef std::string* pointer;

	SrcPosition(std::list<SrcLine>& data, size_t offset) :
		_data(data), _offset(offset) {}
	SrcPosition(const SrcPositon& sp) :
		_data(sp._data), _offset(sp._offset) {}
    ~SrcPosition() {}

	reference operator*() const
	{

	pointer operator->() const;
	bool operator==(const SrcPosition&) const;
	bool operator!=(const SrcPosition&) const;
	SrcPosition& operator++();
	SrcPosition operator++(int);
	
private:
	size_t _offset;
	std::list<SrcLine>& _data;
};

#endif
