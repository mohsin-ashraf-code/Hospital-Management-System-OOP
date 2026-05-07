#include "Person.h"
#include "utility.h"

// Default Constructor
Person::Person() 
{
    id = 0;

    name = new char[1];
    *name = '\0';

    password = new char[1];
    *password = '\0';
}

// Parameterized Constructor
Person::Person(int id, const char* name, const char* password) {
    this->id = id;

    // Deep copy for name using utils
    int nameLen = myStrLen(name);
    this->name = new char[nameLen + 1];
    myStrCopy(this->name, name); // Clean!

    // Deep copy for password using utils
    int passLen = myStrLen(password);
    this->password = new char[passLen + 1];
    myStrCopy(this->password, password); // Clean!
}

Person::Person(const Person& other) {
    id = other.id;
    name = new char[myStrLen(other.name) + 1];
    myStrCopy(name, other.name);

    password = new char[myStrLen(other.password) + 1];
    myStrCopy(password, other.password);
}

// --- Assignment Operator ---
Person& Person::operator=(const Person& other) {
    if (this != &other) { // Prevent self-assignment
        id = other.id;

        delete[] name; // Clean up old memory first
        delete[] password;

        name = new char[myStrLen(other.name) + 1];
        myStrCopy(name, other.name);

        password = new char[myStrLen(other.password) + 1];
        myStrCopy(password, other.password);
    }
    return *this;
}

// Getters
int Person::getId() const 
{
    return id;
}

const char* Person::getName() const 
{
    return name;
}

const char* Person::getPassword() const 
{
    return password;
}

// Destructor (Frees the dynamic memory to avoid leaks)
Person::~Person()
{
    delete[] name;
    delete[] password;
}