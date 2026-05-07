#pragma once
#include "Person.h"
#include <iostream>

class Patient : public Person {
private:
    int age;
    char gender;
    char* contact; // Dynamically allocated, no static arrays!
    float balance;

public:
    // Constructors & Destructor
    Patient();
    Patient(int id, const char* name, const char* password, int age, char gender, const char* contact, float balance);

    Patient(const Patient& other);
    Patient& operator=(const Patient& other);
    ~Patient() override;

    // Getters
    int getAge() const;
    char getGender() const;
    const char* getContact() const;
    float getBalance() const;

    // Required Pure Virtual Implementations
    void displayMenu() const override;
    const char* getRole() const override;

    // Required Operator Overloads[cite: 1]
    void operator+=(float amount);
    void operator-=(float amount);
    bool operator==(int searchId) const;
    friend std::ostream& operator<<(std::ostream& os, const Patient& p);
};
