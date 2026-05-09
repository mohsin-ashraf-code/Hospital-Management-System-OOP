#include "Appointment.h"
#include "utility.h"

using namespace std;

// --- Default Constructor ---
Appointment::Appointment() {
    id = 0;
    patientId = 0;
    doctorId = 0;
    date = STRING("");
    timeSlot = STRING("");
    status = STRING("");
}

// --- Parameterized Constructor ---
Appointment::Appointment(int id, int patientId, int doctorId, const char* date, const char* timeSlot, const char* status) {
    this->id = id;
    this->patientId = patientId;
    this->doctorId = doctorId;
    this->date = STRING(date);
    this->timeSlot = STRING(timeSlot);
    this->status = STRING(status);
}

// --- Copy Constructor ---
Appointment::Appointment(const Appointment& other) {
    id = other.id;
    patientId = other.patientId;
    doctorId = other.doctorId;
    date = other.date;
    timeSlot = other.timeSlot;
    status = other.status;
}

// --- Assignment Operator ---
Appointment& Appointment::operator=(const Appointment& other) 
{
    if (this != &other) 
    {
        id = other.id;
        patientId = other.patientId;
        doctorId = other.doctorId;
        date = other.date;
        timeSlot = other.timeSlot;
        status = other.status;
    }
    return *this;
}
Appointment::~Appointment() {}

// --- Getters ---
int Appointment::getId() const { return id; }
int Appointment::getPatientId() const { return patientId; }
int Appointment::getDoctorId() const { return doctorId; }
const char* Appointment::getDate() const { return date.getData(); }
const char* Appointment::getTimeSlot() const { return timeSlot.getData(); }
const char* Appointment::getStatus() const { return status.getData(); }

// --- Setters ---
void Appointment::setStatus(const char* newStatus) {
    if (newStatus != nullptr) {
        status = STRING(newStatus);
    }
}

// --- Required Operator Overloads ---

// 1. == to check for scheduling conflicts[cite: 1]
bool Appointment::operator==(const Appointment& other) const {
    // Conflict exists if it's the same doctor, date, and time slot...
    bool sameDoctor = (this->doctorId == other.doctorId);
    bool sameDate = myStrEqual(this->date.getData(), other.date.getData());
    bool sameTime = myStrEqual(this->timeSlot.getData(), other.timeSlot.getData());

    // ...AND neither appointment is cancelled.
    bool thisNotCancelled = !myStrEqual(this->status.getData(), "cancelled");
    bool otherNotCancelled = !myStrEqual(other.status.getData(), "cancelled");

    return (sameDoctor && sameDate && sameTime && thisNotCancelled && otherNotCancelled);
}

bool Appointment::operator==(int searchId) const 
{
    return this->id == searchId;
}



// 2. << for formatted console output[cite: 1]
ostream& operator<<(ostream& os, const Appointment& a) 
{
    os << "Appt ID: " << a.id << " | Patient ID: " << a.patientId << " | Doctor ID: " << a.doctorId << " | Date: " << a.date.getData() << " | Time: " << a.timeSlot.getData() << " | Status: " << a.status.getData();
    return os;
}