#ifndef SRCPROCESSOR_H
#define SRCPROCESSOR_H

#include "SrcFile.h"

class SrcProcessor
{
public:
	SrcProcessor(SrcFile&);
	~SrcProcessor();

protected:
    

private:
	SrcFile& _data;
};

#endif
