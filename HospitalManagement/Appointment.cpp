#include "Appointment.h"

Appointment::Appointment() : id(0), patientId(0), doctorId(0), date(""), timeSlot(""), status("pending") 
{
}

Appointment::Appointment(int i, int pId, int dId, const char* dt, const char* ts, const char* st) : id(i), patientId(pId), doctorId(dId), date(dt), timeSlot(ts), status(st) 
{
}

int Appointment::getId() const 
{ 
    return id; 
}

int Appointment::getPatientId() const 
{ 
    return patientId; 
}

int Appointment::getDoctorId() const 
{
    return doctorId; 
}

const char* Appointment::getDate() const 
{ 
    return date.get(); 
}

const char* Appointment::getTimeSlot() const 
{ 
    return timeSlot.get(); 
}

const char* Appointment::getStatus() const 
{
    return status.get(); 
}

void Appointment::setStatus(const char* st) 
{ 
    status.copy(st); 
}

bool Appointment::operator==(int targetId) const 
{
    return id == targetId;
}