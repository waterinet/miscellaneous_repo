#include "SrcCIter.h"

SrcCIter::SrcCIter() :
	_data(0), _index(0)
{
}

SrcCIter::SrcCIter(const SrcFile* data, size_t index) :
	_data(data), _index(index)
{
}

SrcCIter::SrcCIter(const SrcCIter& it) :
	_data(it._data), _index(it._index)
{
}

SrcCIter::~SrcCIter()
{
}

SrcCIter::const_reference SrcCIter::operator*() const
{
    return (*_data)[_index];    
}

SrcCIter::const_pointer SrcCIter::operator->() const
{
    return &(*_data)[_index];
}

SrcCIter& SrcCIter::operator++()
{
	_index++;
	return *this;
}

SrcCIter SrcCIter::operator++(int)
{
    SrcCIter tmp(*this);
	_index++;
	return tmp;
}

bool SrcCIter::operator==(const SrcCIter& it) const
{
	return (_data == it._data) && (_index == it._index);
}

bool SrcCIter::operator!=(const SrcCIter& it) const
{
    return !(*this == it);
}
