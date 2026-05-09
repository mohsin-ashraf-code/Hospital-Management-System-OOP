#include "Doctor.h" 

// Default Constructor
Doctor::Doctor() {
    id = 0;
    // MyString's default constructor automatically sets all text to "" for us!
    fee = 0.0f;
}

// Parameterized Constructor (Using initialization lists for speed & safety!)
Doctor::Doctor(int docId, const char* docName, const char* docPass, const char* docSpec, const char* docContact, float docFee)
    : name(docName), password(docPass), specialization(docSpec), contact(docContact)
{
    id = docId;
    fee = docFee;
}

// Getters (Notice how we use .getData() to return the raw char* array to the UI)
int Doctor::getId() const { return id; }
const char* Doctor::getName() const { return name.getData(); }
const char* Doctor::getPassword() const { return password.getData(); }
const char* Doctor::getSpecialization() const { return specialization.getData(); }
const char* Doctor::getContact() const { return contact.getData(); }
float Doctor::getFee() const 
{ 
    return fee; 
}

// --- Operator Overloads ---
bool Doctor::operator==(const Doctor& other) const 
{
    return this->id == other.id;
}

bool Doctor::operator==(int targetId) const 
{
    return this->id == targetId;
}

void Doctor::setPassword(const char* newPass) 
{
    password = STRING(newPass); // Your custom class automatically manages the heap memory here!
}

void Doctor::setFee(float newFee) 
{
    fee = newFee;
}

std::ostream& operator<<(std::ostream& os, const Doctor& doc) 
{
    // We must use .getData() here so the standard stream knows how to print it!
    os << "Dr. " << doc.name.getData() << " (" << doc.specialization.getData() << ") - Fee: PKR " << doc.fee;
    return os;
}