#pragma once
#include "STRING.h"

class Person 
{
protected:
    int id;
    STRING name;
    STRING password;

public:
    Person();
    Person(int pId, const char* pName, const char* pPass);
    virtual ~Person();

    int getID() const;
    const char* getName() const;
    const char* getPassword() const;

    virtual const char* getRole() const = 0;
    virtual void displayDetails() const = 0;
};