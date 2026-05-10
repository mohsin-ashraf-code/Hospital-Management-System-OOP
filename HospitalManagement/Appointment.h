#pragma once
#include "STRING.h"

class Appointment
{
private:
    int id;
    int patientId;
    int doctorId;
    STRING date;
    STRING timeSlot;
    STRING status;

public:
    Appointment();
    Appointment(int i, int pId, int dId, const char* dt, const char* ts, const char* st);

    int getId() const;
    int getPatientId() const;
    int getDoctorId() const;
    const char* getDate() const;
    const char* getTimeSlot() const;
    const char* getStatus() const;
    void setStatus(const char* st);

    bool operator==(int targetId) const;
};