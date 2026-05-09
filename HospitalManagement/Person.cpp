#include "Person.h"

Person::Person() 
{
    id = 0;
}

Person::Person(int pId, const char* pName, const char* pPass) 
{
    id = pId;
    name.copy(pName);
    password.copy(pPass);
}

Person::~Person() {}

int Person::getID() const 
{ return id; }
const char* Person::getName() const { return name.get(); }
const char* Person::getPassword() const { return password.get(); }