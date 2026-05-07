#pragma once
class Person 
{
protected:
    int id;
    char* name;
    char* password;

public:
    // Constructors & Destructor
    Person();
    Person(int id, const char* name, const char* password);

    Person(const Person& other);
    Person& operator=(const Person& other);

    // Getters
    int getId() const;
    const char* getName() const;
    const char* getPassword() const;

    virtual void displayMenu() const = 0;
    virtual const char* getRole() const = 0;

    virtual ~Person();
};