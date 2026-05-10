#pragma once
#include "Person.h"

class Admin : public Person {
public:
    Admin();
    Admin(int i, const char* n, const char* p);
    bool operator==(int targetId) const;
};