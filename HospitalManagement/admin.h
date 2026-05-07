#pragma once
#include "Person.h"

class Admin : public Person 
{
public:
    // Constructors & Destructor
    Admin();
    Admin(int id, const char* name, const char* password);
    ~Admin() override;

    // Required Pure Virtual Implementations
    void displayMenu() const override;
    const char* getRole() const override;
};