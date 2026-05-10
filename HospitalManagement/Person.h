#pragma once
#include "STRING.h"

class Person
{
protected:
    int id;
    STRING name;
    STRING contact;
    STRING password;

public:
    Person();
    Person(int i, const char* n, const char* c, const char* p);

    int getId() const;
    const char* getName() const;
    const char* getContact() const;
    const char* getPassword() const;

    virtual ~Person();
};