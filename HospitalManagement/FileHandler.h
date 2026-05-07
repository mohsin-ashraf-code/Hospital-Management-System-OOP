#pragma once

#include "Storage.h"
#include "Patient.h"
#include "Doctor.h"
#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "Prescription.h"

class FileHandler {
public:
    // Loaders — called once on startup
    static void loadPatients(Storage<Patient>& store);
    static void loadDoctors(Storage<Doctor>& store);
    static void loadAdmin(Admin*& admin);          // only 1 admin
    static void loadAppointments(Storage<Appointment>& store);
    static void loadBills(Storage<Bill>& store);
    static void loadPrescriptions(Storage<Prescription>& store);

    // Appenders — called when new record is created
    static void appendPatient(const Patient& p);
    static void appendDoctor(const Doctor& d);
    static void appendAppointment(const Appointment& a);
    static void appendBill(const Bill& b);
    static void appendPrescription(const Prescription& p);
    static void appendDischargedPatient(const Patient& p);

    // Updaters — rewrite entire file after in-memory change
    static void saveAllPatients(Storage<Patient>& store);
    static void saveAllDoctors(Storage<Doctor>& store);
    static void saveAllAppointments(Storage<Appointment>& store);
    static void saveAllBills(Storage<Bill>& store);
    static void saveAllPrescriptions(Storage<Prescription>& store);

    // Security log
    static void appendSecurityLog(const char* role, const char* enteredId, const char* result);
};