#pragma once
#include <iostream> 
#include "STRING.h"

class Doctor 
{
private:
    int id;
    STRING name;
    STRING password;
    STRING specialization;
    STRING contact;
    float fee;

public:
    Doctor();
    Doctor(int docId, const char* docName, const char* docPass, const char* docSpec, const char* docContact, float docFee);

    int getId() const;
    const char* getName() const;
    const char* getPassword() const;
    const char* getSpecialization() const;
    const char* getContact() const;
    float getFee() const;

    void setPassword(const char* newPass); // ADD THIS
    void setFee(float newFee);             // ADD THIS

    // Operator overloads so Storage<T> can search for Doctors!
    bool operator==(const Doctor& other) const;
    bool operator==(int targetId) const;

    friend std::ostream& operator<<(std::ostream& os, const Doctor& doc);
};