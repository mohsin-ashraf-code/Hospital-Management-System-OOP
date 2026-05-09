#include "Patient.h"
#include "utility.h"
using namespace std;

// Constructors
Patient::Patient() : Person() 
{
    age = 0;
    gender = 'U'; // Unknown by default
    balance = 0.0f;

    contact = new char[1];
    *contact = '\0';
}

Patient::Patient(int id, const char* name, const char* password, int age, char gender, const char* contact, float balance) : Person(id, name, password)
{
    this->age = age;
    this->gender = gender;
    this->balance = balance;

    int contactLen = myStrLen(contact);
    this->contact = new char[contactLen + 1];
    myStrCopy(this->contact, contact);
}

//Copy Constructor
Patient::Patient(const Patient& other) : Person(other) 
{
    age = other.age;
    gender = other.gender;
    balance = other.balance;

    contact = new char[myStrLen(other.contact) + 1];
    myStrCopy(contact, other.contact);
}

// = overloading
Patient& Patient::operator=(const Patient& other) 
{
    if (this != &other) 
    {
        Person::operator=(other);

        age = other.age;
        gender = other.gender;
        balance = other.balance;

        delete[] contact;
        contact = new char[myStrLen(other.contact) + 1];
        myStrCopy(contact, other.contact);
    }
    return *this;
}

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

void Patient::displayMenu() const 
{
    cout << "Patient Menu\n";
}

const char* Patient::getRole() const 
{
    return "Patient";
}

// += overlaoding adds to Patient balance
void Patient::operator+=(float amount) 
{
    if (amount > 0) 
    {
        balance += amount;
    }
}

// -= overlaoding minuses from patient balance
void Patient::operator-=(float amount) 
{
    this->balance -= amount;
}

// == overloading checks if givenID == realID
bool Patient::operator==(int searchId) const 
{
    return this->id == searchId;
}

// << overloading
ostream& operator<<(ostream& os, const Patient& p) 
{
    os << "ID: " << p.id << " | Name: " << p.name << " | Age: " << p.age << " | Gender: " << p.gender << " | Contact: " << p.contact << " | Balance: PKR " << p.balance;
    return os;
}

Patient::~Patient()
{
    delete[] contact; // Free memory to avoid leaks
}