#pragma once
#include <iostream>

class Appointment 
{
private:
    int id;
    int patientId;
    int doctorId;
    char* date;      // Dynamically allocated (replaces date[11])
    char* timeSlot;  // Dynamically allocated (replaces timeSlot[6])
    char* status;    // Dynamically allocated (replaces status[12])

public:
    // Constructors & Destructor
    Appointment();
    Appointment(int id, int patientId, int doctorId, const char* date, const char* timeSlot, const char* status);
    ~Appointment();

    // Rule of Three (Required for safe Storage<T> usage)
    Appointment(const Appointment& other);
    Appointment& operator=(const Appointment& other);

    // Getters
    int getId() const;
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getTimeSlot() const;
    const char* getStatus() const;

    // Setter for status (Needed when admin/doctor cancels or completes it)
    void setStatus(const char* newStatus);

    // Required Operator Overloads[cite: 1]
    bool operator==(const Appointment& other) const;
    bool operator==(int searchId) const;
    friend std::ostream& operator<<(std::ostream& os, const Appointment& a);
};
