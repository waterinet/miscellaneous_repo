#include "SrcIter.h"
#include "SrcFile.h"

SrcIter::SrcIter() :
	_data(0), _index(0)
{
}

SrcIter::SrcIter(SrcFile* data, size_t index) :
	_data(data), _index(index)
{
}

SrcIter::SrcIter(const SrcIter& it) :
	_data(it._data), _index(it._index)
{
}

SrcIter::~SrcIter()
{
}

SrcIter::reference SrcIter::operator*() const
{
	return (*_data)[_index];
}

SrcIter::pointer SrcIter::operator->() const
{
	return &(*_data)[_index];
}

bool SrcIter::operator==(const SrcIter& it) const
{
	return (_data == it._data) && (_index == it._index);
}

bool SrcIter::operator!=(const SrcIter& it) const
{
	return !(*this == it);
}

SrcIter& SrcIter::operator++()
{
	_index++;
	return *this;
}

SrcIter SrcIter::operator++(int)
{
	SrcIter tmp(*this);
	_index++;
	return tmp;
}

