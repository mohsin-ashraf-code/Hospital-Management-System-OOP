#include "Appointment.h"
#include "utility.h"

using namespace std;

// --- Default Constructor ---
Appointment::Appointment() {
    id = 0;
    patientId = 0;
    doctorId = 0;

    date = new char[1]; *date = '\0';
    timeSlot = new char[1]; *timeSlot = '\0';
    status = new char[1]; *status = '\0';
}

// --- Parameterized Constructor ---
Appointment::Appointment(int id, int patientId, int doctorId, const char* date, const char* timeSlot, const char* status) {
    this->id = id;
    this->patientId = patientId;
    this->doctorId = doctorId;

    this->date = new char[myStrLen(date) + 1];
    myStrCopy(this->date, date);

    this->timeSlot = new char[myStrLen(timeSlot) + 1];
    myStrCopy(this->timeSlot, timeSlot);

    this->status = new char[myStrLen(status) + 1];
    myStrCopy(this->status, status);
}

// --- Copy Constructor ---
Appointment::Appointment(const Appointment& other) {
    id = other.id;
    patientId = other.patientId;
    doctorId = other.doctorId;

    date = new char[myStrLen(other.date) + 1];
    myStrCopy(date, other.date);

    timeSlot = new char[myStrLen(other.timeSlot) + 1];
    myStrCopy(timeSlot, other.timeSlot);

    status = new char[myStrLen(other.status) + 1];
    myStrCopy(status, other.status);
}

// --- Assignment Operator ---
Appointment& Appointment::operator=(const Appointment& other) 
{
    if (this != &other) 
    {
        // Free old memory first
        delete[] date;
        delete[] timeSlot;
        delete[] status;

        // Copy new data
        id = other.id;
        patientId = other.patientId;
        doctorId = other.doctorId;

        date = new char[myStrLen(other.date) + 1];
        myStrCopy(date, other.date);

        timeSlot = new char[myStrLen(other.timeSlot) + 1];
        myStrCopy(timeSlot, other.timeSlot);

        status = new char[myStrLen(other.status) + 1];
        myStrCopy(status, other.status);
    }
    return *this;
}

// --- Destructor ---
Appointment::~Appointment() 
{
    delete[] date;
    delete[] timeSlot;
    delete[] status;
}

// --- Getters ---
int Appointment::getId() const { return id; }
int Appointment::getPatientId() const { return patientId; }
int Appointment::getDoctorId() const { return doctorId; }
const char* Appointment::getDate() const { return date; }
const char* Appointment::getTimeSlot() const { return timeSlot; }
const char* Appointment::getStatus() const { return status; }

// --- Setters ---
void Appointment::setStatus(const char* newStatus) {
    if (newStatus != nullptr) {
        delete[] status; // delete the old status
        status = new char[myStrLen(newStatus) + 1];
        myStrCopy(status, newStatus);
    }
}

// --- Required Operator Overloads ---

// 1. == to check for scheduling conflicts[cite: 1]
bool Appointment::operator==(const Appointment& other) const {
    // Conflict exists if it's the same doctor, date, and time slot...
    bool sameDoctor = (this->doctorId == other.doctorId);
    bool sameDate = myStrEqual(this->date, other.date);
    bool sameTime = myStrEqual(this->timeSlot, other.timeSlot);

    // ...AND neither appointment is cancelled.
    bool thisNotCancelled = !myStrEqual(this->status, "cancelled");
    bool otherNotCancelled = !myStrEqual(other.status, "cancelled");

    return (sameDoctor && sameDate && sameTime && thisNotCancelled && otherNotCancelled);
}

bool Appointment::operator==(int searchId) const 
{
    return this->id == searchId;
}



// 2. << for formatted console output[cite: 1]
ostream& operator<<(ostream& os, const Appointment& a) 
{
    os << "Appt ID: " << a.id << " | Patient ID: " << a.patientId << " | Doctor ID: " << a.doctorId << " | Date: " << a.date << " | Time: " << a.timeSlot << " | Status: " << a.status;
    return os;
}