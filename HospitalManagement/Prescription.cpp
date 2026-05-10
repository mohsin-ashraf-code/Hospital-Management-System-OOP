#include "Prescription.h"

Prescription::Prescription() : id(0), appointmentId(0), patientId(0), doctorId(0), date("") 
{
    *(medicines + 0) = '\0';
    *(notes + 0) = '\0';
}

Prescription::Prescription(int i, int aId, int pId, int dId, const char* dt, const char* meds, const char* nts) : id(i), appointmentId(aId), patientId(pId), doctorId(dId), date(dt) 
{

    int j = 0;
    while (*(meds + j) != '\0' && j < 499) 
    {
        *(medicines + j) = *(meds + j);
        j++;
    }
    *(medicines + j) = '\0';

    j = 0;
    while (*(nts + j) != '\0' && j < 299)
    {
        *(notes + j) = *(nts + j);
        j++;
    }
    *(notes + j) = '\0';
}

int Prescription::getId() const 
{ 
    return id; 
}

int Prescription::getAppointmentId() const 
{ 
    return appointmentId; 
}

int Prescription::getPatientId() const 
{
    return patientId; 
}

int Prescription::getDoctorId() const 
{ 
    return doctorId; 
}

const char* Prescription::getDate() const 
{
    return date.get(); 
}

const char* Prescription::getMedicines() const 
{ 
    return medicines; 
}

const char* Prescription::getNotes() const 
{ 
    return notes; 
}

bool Prescription::operator==(int targetId) const 
{
    return id == targetId;
}