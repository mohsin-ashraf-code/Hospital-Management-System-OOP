#include "Admin.h"

Admin::Admin() : Person() 
{
}

Admin::Admin(int i, const char* n, const char* p) : Person(i, n, "N/A", p) 
{
}

bool Admin::operator==(int targetId) const 
{
    return id == targetId;
}