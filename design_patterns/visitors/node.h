#ifndef NODE_H
#define NODE_H

class visitor;
class node
{
public:
    virtual void accept(visitor&) = 0;
};

class expr : public node
{
public:
    void accept(visitor&);
};

class stmt : public node
{
public:
    void accept(visitor&);
};

#endif // NODE_H

