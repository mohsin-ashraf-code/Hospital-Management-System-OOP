#include "Doctor.h" 
#include "utility.h"

// Default Constructor
Doctor::Doctor() {
    id = 0;
    name[0] = '\0';
    password[0] = '\0';
    specialization[0] = '\0';
    contact[0] = '\0';
    fee = 0.0f;
}

// Parameterized Constructor
Doctor::Doctor(int docId, const char* docName, const char* docPass, const char* docSpec, const char* docContact, float docFee) {
    id = docId;
    myStrCopy(name, docName);
    myStrCopy(password, docPass);
    myStrCopy(specialization, docSpec);
    myStrCopy(contact, docContact);
    fee = docFee;
}

// Getters
int Doctor::getId() const { return id; }
const char* Doctor::getName() const { return name; }
const char* Doctor::getPassword() const { return password; }
const char* Doctor::getSpecialization() const { return specialization; }
const char* Doctor::getContact() const { return contact; }
float Doctor::getFee() const { return fee; }

// --- Operator Overloads ---
bool Doctor::operator==(const Doctor& other) const {
    return this->id == other.id;
}

bool Doctor::operator==(int targetId) const {
    return this->id == targetId;
}

std::ostream& operator<<(std::ostream& os, const Doctor& doc) {
    os << "Dr. " << doc.name << " (" << doc.specialization << ") - Fee: PKR " << doc.fee;
    return os;
}