#include "CGeneric.h"

using namespace std;

CGeneric::CGeneric()
{
}

CGeneric::CGeneric(SrcPosition beg, SrcPosition end) :
	ctrl_exp(), association_list(),
	_default(association_list.end())
{
}

CGeneric::~CGeneric()
{
}

AssociationType CGeneric::first_or_default()
{
	if (association_list.empty())
		return AssociationType("", "");
	if (_default == association_list.end()) {
        return association_list.front();
	}
	else {
		return *_default;
	}
}
