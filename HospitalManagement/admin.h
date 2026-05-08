#pragma once
#include "Person.h"

#pragma once
class Admin : public Person
{
public:
    Admin();
    Admin(int id, const char* name, const char* password);
    ~Admin() override;

    void displayMenu() const override;
    const char* getRole() const override;
};