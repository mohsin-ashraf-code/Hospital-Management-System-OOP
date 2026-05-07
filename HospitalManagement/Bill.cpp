#include "Bill.h"
#include "utility.h"

using namespace std;

// --- Default Constructor ---
Bill::Bill() {
    id = 0;
    patientId = 0;
    appointmentId = 0;
    amount = 0.0f;

    status = new char[1]; *status = '\0';
    date = new char[1]; *date = '\0';
}

// --- Parameterized Constructor ---
Bill::Bill(int id, int patientId, int appointmentId, float amount, const char* status, const char* date) {
    this->id = id;
    this->patientId = patientId;
    this->appointmentId = appointmentId;
    this->amount = amount;

    this->status = new char[myStrLen(status) + 1];
    myStrCopy(this->status, status);

    this->date = new char[myStrLen(date) + 1];
    myStrCopy(this->date, date);
}

// --- Copy Constructor ---
Bill::Bill(const Bill& other) {
    id = other.id;
    patientId = other.patientId;
    appointmentId = other.appointmentId;
    amount = other.amount;

    status = new char[myStrLen(other.status) + 1];
    myStrCopy(status, other.status);

    date = new char[myStrLen(other.date) + 1];
    myStrCopy(date, other.date);
}

// --- Assignment Operator ---
Bill& Bill::operator=(const Bill& other) 
{
    if (this != &other) {
        // Free old memory first to avoid leaks
        delete[] status;
        delete[] date;

        // Copy new data
        id = other.id;
        patientId = other.patientId;
        appointmentId = other.appointmentId;
        amount = other.amount;

        status = new char[myStrLen(other.status) + 1];
        myStrCopy(status, other.status);

        date = new char[myStrLen(other.date) + 1];
        myStrCopy(date, other.date);
    }
    return *this;
}

// --- Destructor ---
Bill::~Bill() {
    delete[] status;
    delete[] date;
}

// --- Getters ---
int Bill::getId() const { return id; }
int Bill::getPatientId() const { return patientId; }
int Bill::getAppointmentId() const { return appointmentId; }
float Bill::getAmount() const { return amount; }
const char* Bill::getStatus() const { return status; }
const char* Bill::getDate() const { return date; }

// --- Setters ---
void Bill::setStatus(const char* newStatus) 
{
    if (newStatus != nullptr) {
        delete[] status; // delete the old status string
        status = new char[myStrLen(newStatus) + 1];
        myStrCopy(status, newStatus);
    }
}

bool Bill::operator==(int searchId) const 
{
    return this->id == searchId;
}

// --- Operator Overloads ---
std::ostream& operator<<(std::ostream& os, const Bill& b)
{
    os << "Bill ID: " << b.getId() << " | Appt ID: " << b.getAppointmentId() << " | Patient ID: " << b.getPatientId() << " | Amount: PKR " << b.getAmount() << " | Status: " << b.getStatus() << " | Date: " << b.getDate();
    return os;
}