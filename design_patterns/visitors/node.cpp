#include "node.h"
#include "visitor.h"

void expr::accept(visitor& v)
{
    v.visit(*this);
}

void stmt::accept(visitor& v)
{
    v.visit(*this);
}
