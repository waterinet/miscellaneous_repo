#ifndef VISITOR
#define VISITOR

#include "node.h"

class visitor
{
public:
    virtual void visit(expr&) = 0;
    virtual void visit(stmt&) = 0;
};

class visitor1 : public visitor
{
    void visit(expr& e);
    void visit(stmt& s);
};

class visitor2 : public visitor
{
    void visit(expr& e);
    void visit(stmt& s);
};

#endif // VISITOR

