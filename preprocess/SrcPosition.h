#ifndef SRC_POSITION_H
#define SRC_POSITION_H

#include "SrcLine.h"

struct SrcPosition
{
    std::list<SrcLine>::const_iterator row; // line 
    SrcLine::const_iterator col;            // token of line
};

#endif
