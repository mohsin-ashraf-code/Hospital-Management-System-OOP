#pragma once
#include <iostream>
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
    // Constructors & Destructor
    Bill();
    Bill(int id, int patientId, int appointmentId, float amount, const char* status, const char* date);
    ~Bill();

    // Rule of Three (Required for safe Storage<T> usage)
    Bill(const Bill& other);
    Bill& operator=(const Bill& other);

    // Getters
    int getId() const;
    int getPatientId() const;
    int getAppointmentId() const;
    float getAmount() const;
    const char* getStatus() const;
    const char* getDate() const;

    // Setters
    void setStatus(const char* newStatus); // Crucial for "Pay Bill" menu

    // Operator Overloads
    bool operator==(int searchId) const;
    friend std::ostream& operator<<(std::ostream& os, const Bill& b);
};