#pragma once
#include "STRING.h"

class Prescription 
{
private:
    int id;
    int appointmentId;
    int patientId;
    int doctorId;
    STRING date;
    char medicines[500];
    char notes[300];

public:
    // Constructors & Destructor
    Prescription();
    Prescription(int id, int appointmentId, int patientId, int doctorId, const char* date, const char* meds, const char* notes);
    ~Prescription();

    // Rule of Three (Required because 'date' is dynamic memory)
    Prescription(const Prescription& other);
    Prescription& operator=(const Prescription& other);
    bool operator==(int searchId) const;

    // Getters
    int getId() const;
    int getAppointmentId() const;
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getMedicines() const;
    const char* getNotes() const;
};
