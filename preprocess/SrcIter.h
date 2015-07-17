#ifndef SRCITER_H
#define SRCITER_H

#include <string>

class SrcFile;

class SrcIter
{
public:
    typedef std::string value_type;
    typedef std::ptrdiff_t difference_type;
    typedef std::string& reference;
    typedef std::string* pointer;

    SrcIter();
	SrcIter(SrcFile* data, size_t index);
	SrcIter(const SrcIter&);
	~SrcIter();

	reference operator*() const;
	pointer operator->() const;
	bool operator==(const SrcIter&) const;
	bool operator!=(const SrcIter&) const;
	SrcIter& operator++();
	SrcIter operator++(int);

private:
	size_t _index;
	SrcFile* _data;
};

#endif
