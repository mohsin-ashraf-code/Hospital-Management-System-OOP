#pragma once
#include "Person.h"

class Doctor : public Person {
private:
    STRING specialization;
    float fee;

public:
    Doctor();
    Doctor(int i, const char* n, const char* s, const char* c, const char* p, float f);
    const char* getSpecialization() const;
    float getFee() const;
    bool operator==(int targetId) const;
};