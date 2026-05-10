#include "Doctor.h"

Doctor::Doctor() : Person(), specialization(""), fee(0.0f) 
{
}

Doctor::Doctor(int i, const char* n, const char* s, const char* c, const char* p, float f) : Person(i, n, c, p), specialization(s), fee(f) 
{
}

const char* Doctor::getSpecialization() const 
{ 
    return specialization.get(); 
}

float Doctor::getFee() const 
{ 
    return fee; 
}

bool Doctor::operator==(int targetId) const 
{
    return id == targetId;
}