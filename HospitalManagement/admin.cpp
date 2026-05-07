#include "Admin.h"
#include <iostream>

using namespace std; // Added exactly as requested!

// --- Constructors & Destructor ---

// The Person default constructor automatically handles setting up the empty dynamic arrays
Admin::Admin() : Person() {
}

// We pass everything directly up to Person, which handles the dynamic copying using utils.h!
Admin::Admin(int id, const char* name, const char* password) : Person(id, name, password)
{

}

// The Person destructor will automatically be called to delete name and password.
// We don't have any extra pointers in Admin, so this stays empty.
Admin::~Admin() {
}

// --- Virtual Overrides ---

void Admin::displayMenu() const {
    cout << "--- Admin Panel - MediCore ---\n";
}

const char* Admin::getRole() const {
    return "Admin";
}