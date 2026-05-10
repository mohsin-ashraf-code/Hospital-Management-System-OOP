#include "Person.h"

Person::Person() : id(0), name(""), contact(""), password("") {}

Person::Person(int i, const char* n, const char* c, const char* p) : id(i), name(n), contact(c), password(p) {}

Person::~Person() {}

int Person::getId() const { return id; }
const char* Person::getName() const { return name.get(); }
const char* Person::getContact() const { return contact.get(); }
const char* Person::getPassword() const { return password.get(); }