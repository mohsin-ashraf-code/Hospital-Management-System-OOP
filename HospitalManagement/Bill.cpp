#include "Bill.h"

Bill::Bill() : id(0), patientId(0), appointmentId(0), amount(0.0f), status("unpaid"), date("") {}

Bill::Bill(int i, int pId, int aId, float amt, const char* st, const char* dt)
    : id(i), patientId(pId), appointmentId(aId), amount(amt), status(st), date(dt) {
}

int Bill::getId() const { return id; }
int Bill::getPatientId() const { return patientId; }
int Bill::getAppointmentId() const { return appointmentId; }
float Bill::getAmount() const { return amount; }
const char* Bill::getStatus() const { return status.get(); }
const char* Bill::getDate() const { return date.get(); }

void Bill::setStatus(const char* st) { status.copy(st); }

bool Bill::operator==(int targetId) const {
    return id == targetId;
}