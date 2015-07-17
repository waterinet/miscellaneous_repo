#ifndef SRCCITER_H
#define SRCCITER_H

#include <string>

class SrcFile;

class SrcCIter
{
public:
    typedef std::string value_type;
    typedef std::ptrdiff_t difference_type;
    typedef const std::string& const_reference;
    typedef const std::string* const_pointer;

	SrcCIter();
	SrcCIter(const SrcFile* data, size_t index);
	SrcCIter(const SrcCIter&);
	~SrcCIter();

    const_reference operator*() const;
    const_pointer operator->() const;
    bool operator==(const SrcCIter&) const;
    bool operator!=(const SrcCIter&) const;
    SrcCIter& operator++();
    SrcCIter operator++(int);

private:
	size_t _index;
    const SrcFile* _data;
};

#endif
