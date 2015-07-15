#ifndef CGENERIC_H
#define CGENERIC_H

#include <vector>
#include <string>
#include "SrcPosition.h"

class CGeneric
{
public:
	typedef std::string TypeName;
	typedef std::string Expression;
	typedef pair<TypeName, Expression> AssociationType;
	CGeneric();
	CGeneric(SrcPositon beg, SrcPosition end);
	~CGeneric();
	AssociationType first_or_default();
private:
	std::string ctrl_exp;
    std::vector<AssociationType> association_list;
	std::vector<AssociationType>::const_iterator _default;
};
#endif
