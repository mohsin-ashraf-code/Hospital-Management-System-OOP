#pragma once
#include "Person.h"

class Patient : public Person {
private:
    int age;
    char gender;
    float balance;

public:
    Patient();
    Patient(int i, const char* n, const char* p, int a, char g, const char* c, float b);

    int getAge() const;
    char getGender() const;
    float getBalance() const;

    void addBalance(float amount);
    void deductBalance(float amount);

    bool operator==(int targetId) const;
    void operator+=(float amount);
    void operator-=(float amount);
};