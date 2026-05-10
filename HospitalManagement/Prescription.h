#pragma once
#include "STRING.h"

class Prescription {
private:
    int id;
    int appointmentId;
    int patientId;
    int doctorId;
    STRING date;
    char medicines[500]; // Rubric explicit allowance
    char notes[300];     // Rubric explicit allowance

public:
    Prescription();
    Prescription(int i, int aId, int pId, int dId, const char* dt, const char* meds, const char* nts);

    int getId() const;
    int getAppointmentId() const;
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getMedicines() const;
    const char* getNotes() const;

    bool operator==(int targetId) const;
};