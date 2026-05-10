#pragma once
#include "STRING.h"

class Bill {
private:
    int id;
    int patientId;
    int appointmentId;
    float amount;
    STRING status;
    STRING date;

public:
    Bill();
    Bill(int i, int pId, int aId, float amt, const char* st, const char* dt);

    int getId() const;
    int getPatientId() const;
    int getAppointmentId() const;
    float getAmount() const;
    const char* getStatus() const;
    const char* getDate() const;
    void setStatus(const char* st);

    bool operator==(int targetId) const;
};