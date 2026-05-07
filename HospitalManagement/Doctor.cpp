#include "Doctor.h"
#include "utility.h"

using namespace std;

// --- Default Constructor ---
Doctor::Doctor() : Person()
{
    specialization = new char[1];
    *specialization = '\0';
    contact = new char[1];
    *contact = '\0';
    fee = 0.0f;
}

// --- Parameterized Constructor ---
Doctor::Doctor(int id, const char* name, const char* password, const char* spec, const char* contact, float fee) : Person(id, name, password)
{
    this->fee = fee;

    int specLen = myStrLen(spec);
    this->specialization = new char[specLen + 1];
    myStrCopy(this->specialization, spec);

    int contactLen = myStrLen(contact);
    this->contact = new char[contactLen + 1];
    myStrCopy(this->contact, contact);
}

// --- Copy Constructor ---
Doctor::Doctor(const Doctor& other) : Person(other) {
    fee = other.fee;

    specialization = new char[myStrLen(other.specialization) + 1];
    myStrCopy(specialization, other.specialization);

    contact = new char[myStrLen(other.contact) + 1];
    myStrCopy(contact, other.contact);
}

// --- Assignment Operator ---
Doctor& Doctor::operator=(const Doctor& other) {
    if (this != &other) {
        Person::operator=(other); // Call base class assignment

        fee = other.fee;

        delete[] specialization; // Clean up old memory
        delete[] contact;

        specialization = new char[myStrLen(other.specialization) + 1];
        myStrCopy(specialization, other.specialization);

        contact = new char[myStrLen(other.contact) + 1];
        myStrCopy(contact, other.contact);
    }
    return *this;
}

// --- Destructor ---
Doctor::~Doctor()
{
    delete[] specialization;
    delete[] contact;
}

// --- Getters ---
const char* Doctor::getSpecialization() const
{
    return specialization;
}

const char* Doctor::getContact() const
{
    return contact;
}

float Doctor::getFee() const
{
    return fee;
}

// --- Virtual Overrides ---
void Doctor::displayMenu() const
{
    cout << "--- Doctor Menu ---\n";
}

const char* Doctor::getRole() const
{
    return "Doctor";
}

// --- Operator Overloads ---
bool Doctor::operator==(int searchId) const
{
    return this->id == searchId;
}

ostream& operator<<(ostream& os, const Doctor& d)
{
    // Note: We use d.getId() and d.getName() here because id and name are protected in Person
    os << "ID: " << d.getId() << " | Name: " << d.getName() << " | Specialization: " << d.specialization << " | Contact: " << d.contact << " | Fee: PKR " << d.fee;
    return os;
}