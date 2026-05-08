#pragma once
#include <iostream> 

class Doctor {
private:
    int id;
    char name[100];
    char password[100];
    char specialization[100];
    char contact[100];
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

    // Operator overloads so Storage<T> can search for Doctors!
    bool operator==(const Doctor& other) const;
    bool operator==(int targetId) const;

    friend std::ostream& operator<<(std::ostream& os, const Doctor& doc);
};