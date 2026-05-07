#pragma once
#include "Person.h"
#include <iostream>

class Doctor : public Person 
{
private:
    char* specialization;
    char* contact;
    float fee;

public:
    Doctor();
    Doctor(int id, const char* name, const char* password, const char* spec, const char* contact, float fee);

    Doctor(const Doctor& other);
    Doctor& operator=(const Doctor& other);
    ~Doctor() override;

    const char* getSpecialization() const;
    const char* getContact() const;
    float getFee() const;

    void displayMenu() const override;
    const char* getRole() const override;

    bool operator==(int searchId) const;
    friend std::ostream& operator<<(std::ostream& os, const Doctor& d);
};