#include "Patient.h"
#include "utility.h" // Utilizing our global toolkit!
using namespace std;

// --- Constructors & Destructor ---
Patient::Patient() : Person() 
{
    age = 0;
    gender = 'U'; // Unknown by default
    balance = 0.0f;

    contact = new char[1];
    *contact = '\0';
}

// Note: We pass id, name, and password straight up to the Person constructor
Patient::Patient(int id, const char* name, const char* password, int age, char gender, const char* contact, float balance) : Person(id, name, password)
{
    this->age = age;
    this->gender = gender;
    this->balance = balance;

    // Dynamically allocate and copy contact using utils.h
    int contactLen = myStrLen(contact);
    this->contact = new char[contactLen + 1];
    myStrCopy(this->contact, contact);
}

// --- Copy Constructor ---
Patient::Patient(const Patient& other) : Person(other) {
    age = other.age;
    gender = other.gender;
    balance = other.balance;

    contact = new char[myStrLen(other.contact) + 1];
    myStrCopy(contact, other.contact);
}

// --- Assignment Operator ---
Patient& Patient::operator=(const Patient& other) {
    if (this != &other) {
        Person::operator=(other); // Call base class assignment

        age = other.age;
        gender = other.gender;
        balance = other.balance;

        delete[] contact; // Clean up old memory
        contact = new char[myStrLen(other.contact) + 1];
        myStrCopy(contact, other.contact);
    }
    return *this;
}

Patient::~Patient() 
{
    delete[] contact; // Free memory to avoid leaks
}

// --- Getters ---
int Patient::getAge() const 
{ 
    return age; 
}

char Patient::getGender() const 
{ 
    return gender; 
}

const char* Patient::getContact() const 
{ 
    return contact; 
}

float Patient::getBalance() const 
{ 
    return balance; 
}

// --- Virtual Overrides ---
void Patient::displayMenu() const 
{
    cout << "--- Patient Menu ---\n";
}

const char* Patient::getRole() const 
{
    return "Patient";
}

// --- Required Operator Overloads[cite: 1] ---

// 1. += to top up balance
void Patient::operator+=(float amount) 
{
    if (amount > 0) {
        balance += amount;
    }
}

// 2. -= to deduct from balance (e.g., paying a bill)
void Patient::operator-=(float amount) 
{
    // Removed the safe-guard. The Menu logic handles the exception check!
    this->balance -= amount;
}

// 3. == to compare patients by ID
bool Patient::operator==(int searchId) const 
{
    return this->id == searchId; // 'id' inherited from Person
}

// 4. << for formatted console output
ostream& operator<<(ostream& os, const Patient& p) 
{
    os << "ID: " << p.id << " | Name: " << p.name << " | Age: " << p.age << " | Gender: " << p.gender << " | Contact: " << p.contact << " | Balance: PKR " << p.balance;
    return os;
}