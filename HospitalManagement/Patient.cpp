#include "Patient.h"

Patient::Patient() : Person(), age(0), gender('M'), balance(0.0f) 
{
}

Patient::Patient(int i, const char* n, const char* p, int a, char g, const char* c, float b) : Person(i, n, c, p), age(a), gender(g), balance(b) 
{
}

int Patient::getAge() const 
{
    return age; 
}

char Patient::getGender() const 
{ 
    return gender; 
}

float Patient::getBalance() const 
{ 
    return balance; 
}

void Patient::addBalance(float amount) 
{ 
    balance += amount; 
}

void Patient::deductBalance(float amount) 
{ 
    balance -= amount; 
}


bool Patient::operator==(int targetId) const 
{
    return id == targetId;
}

void Patient::operator+=(float amount) 
{ 
    balance += amount; 
}

void Patient::operator-=(float amount) 
{ 
    balance -= amount; 
}