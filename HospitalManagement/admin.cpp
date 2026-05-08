#include "Admin.h"
#include <iostream>

using namespace std;

//Constructor
Admin::Admin() : Person() 
{
}

Admin::Admin(int id, const char* name, const char* password) : Person(id, name, password)
{
}

Admin::~Admin() 
{
}

void Admin::displayMenu() const 
{
    cout << "ADMIN PANEL - MediCore\n";
}

const char* Admin::getRole() const 
{
    return "Admin";
}