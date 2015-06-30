#include "visitor.h"
#include <iostream>

void visitor1::visit(expr& e)
{
    std::cout << "1-visit expr\n";
}
void visitor1::visit(stmt& s)
{
    std::cout << "1-visit stmt\n";
}

void visitor2::visit(expr& e)
{
    std::cout << "2-visit expr\n";
}
void visitor2::visit(stmt& s)
{
    std::cout << "2-visit stmt\n";
}
