#include "Bill.h"
#include "utility.h"

using namespace std;

// --- Default Constructor ---
Bill::Bill() {
    id = 0;
    patientId = 0;
    appointmentId = 0;
    amount = 0.0f;

    status = STRING("");
    date = STRING("");
}

// --- Parameterized Constructor ---
Bill::Bill(int id, int patientId, int appointmentId, float amount, const char* status, const char* date) {
    this->id = id;
    this->patientId = patientId;
    this->appointmentId = appointmentId;
    this->amount = amount;

    this->status = STRING(status);
    this->date = STRING(date);
}

// --- Copy Constructor ---
Bill::Bill(const Bill& other) {
    id = other.id;
    patientId = other.patientId;
    appointmentId = other.appointmentId;
    amount = other.amount;

    status = other.status;
    date = other.date;
}

// --- Assignment Operator ---
Bill& Bill::operator=(const Bill& other) 
{
    if (this != &other) {
        id = other.id;
        patientId = other.patientId;
        appointmentId = other.appointmentId;
        amount = other.amount;
        status = other.status;
        date = other.date;
    }
    return *this;
}

// --- Destructor ---
Bill::~Bill() {}

// --- Getters ---
int Bill::getId() const { return id; }
int Bill::getPatientId() const { return patientId; }
int Bill::getAppointmentId() const { return appointmentId; }
float Bill::getAmount() const { return amount; }
const char* Bill::getStatus() const { return status.getData(); }
const char* Bill::getDate() const { return date.getData(); }

// --- Setters ---
void Bill::setStatus(const char* newStatus) 
{
    if (newStatus != nullptr) {
        status = STRING(newStatus);
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