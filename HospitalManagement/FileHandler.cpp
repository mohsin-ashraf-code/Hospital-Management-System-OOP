#define _CRT_SECURE_NO_WARNINGS
#include "FileHandler.h"
#include "Validator.h"
#include "FileNotFoundException.h" // ADDED FOR STRICT EXCEPTION HANDLING
#include <fstream>
#include <iostream>

using namespace std;


static void manualCopy(char* dest, const char* src) {
    int i = 0;
    while (*(src + i) != '\0') {
        *(dest + i) = *(src + i);
        i++;
    }
    *(dest + i) = '\0';
}
// ==========================================
// 1. HELPER PARSING FUNCTIONS (Rule XIV Compliant)
// ==========================================
static int parseCSVLine(const char* line, char fields[][200], int maxFields)
{
    int fieldIdx = 0, charIdx = 0;
    int i = 0;
    while (*(line + i) != '\0' && *(line + i) != '\n' && *(line + i) != '\r' && fieldIdx < maxFields)
    {
        if (*(line + i) == ',')
        {
            *(*(fields + fieldIdx) + charIdx) = '\0';
            fieldIdx++; charIdx = 0;
        }
        else
        {
            *(*(fields + fieldIdx) + charIdx) = *(line + i);
            charIdx++;
        }
        i++;
    }
    *(*(fields + fieldIdx) + charIdx) = '\0';
    return fieldIdx + 1;
}

// ==========================================
// 2. LOADERS (Reading from txt files)
// ==========================================
void FileHandler::loadPatients(Storage<Patient>& store)
{
    ifstream file("patients.txt");
    if (!file.is_open()) throw FileNotFoundException("Startup Error: patients.txt missing.");

    char line[1024];
    while (file.getline(line, 1024))
    {
        char fields[7][200];
        if (parseCSVLine(line, fields, 7) >= 6) // Forgiving check
        {
            Patient p(Validator::charToInt(*(fields + 0)), *(fields + 1), *(fields + 5), Validator::charToInt(*(fields + 2)), *(*(fields + 3) + 0), *(fields + 4), Validator::charToFloat(*(fields + 6)));
            store.add(p);
        }
    }
    file.close();
}

void FileHandler::loadDoctors(Storage<Doctor>& store)
{
    ifstream file("doctors.txt");
    if (!file.is_open()) throw FileNotFoundException("Startup Error: doctors.txt missing.");

    char line[1024];
    while (file.getline(line, 1024))
    {
        char fields[6][200];
        if (parseCSVLine(line, fields, 6) >= 5) // Forgiving check
        {
            Doctor d(Validator::charToInt(*(fields + 0)), *(fields + 1), *(fields + 4), *(fields + 2), *(fields + 3), Validator::charToFloat(*(fields + 5)));
            store.add(d);
        }
    }
    file.close();
}

void FileHandler::loadAdmin(Admin*& admin)
{
    ifstream file("admin.txt");
    if (!file.is_open()) throw FileNotFoundException("Startup Error: admin.txt missing.");

    char line[512];
    if (file.getline(line, 512))
    {
        char fields[3][200];
        if (parseCSVLine(line, fields, 3) == 3)
        {
            admin = new Admin(Validator::charToInt(*(fields + 0)), *(fields + 1), *(fields + 2));
        }
    }
    file.close();
}

void FileHandler::loadAppointments(Storage<Appointment>& store)
{
    ifstream file("appointments.txt");
    if (!file.is_open()) throw FileNotFoundException("Startup Error: appointments.txt missing.");

    char line[1024];
    while (file.getline(line, 1024))
    {
        char fields[6][200];
        // Now expects 6 columns because of the new 'Slot' feature!
        if (parseCSVLine(line, fields, 6) >= 5)
        {
            Appointment a(Validator::charToInt(*(fields + 0)), Validator::charToInt(*(fields + 1)), Validator::charToInt(*(fields + 2)), *(fields + 3), *(fields + 4), *(fields + 5));
            store.add(a);
        }
    }
    file.close();
}

void FileHandler::loadBills(Storage<Bill>& store)
{
    ifstream file("bills.txt");
    if (!file.is_open()) throw FileNotFoundException("Startup Error: bills.txt missing.");

    char line[1024];
    while (file.getline(line, 1024))
    {
        char fields[6][200];
        int cols = parseCSVLine(line, fields, 6);
        if (cols >= 5)
        {
            // If the UI only saved 5 columns, pad the 6th with a blank space to prevent crashes
            if (cols == 5) manualCopy(*(fields + 5), "N/A");

            Bill b(Validator::charToInt(*(fields + 0)), Validator::charToInt(*(fields + 1)), Validator::charToInt(*(fields + 2)), Validator::charToFloat(*(fields + 3)), *(fields + 4), *(fields + 5));
            store.add(b);
        }
    }
    file.close();
}

void FileHandler::loadPrescriptions(Storage<Prescription>& store)
{
    ifstream file("prescriptions.txt");
    if (!file.is_open()) throw FileNotFoundException("Startup Error: prescriptions.txt missing.");

    char line[2048];
    while (file.getline(line, 2048))
    {
        char fields[7][200];
        int cols = parseCSVLine(line, fields, 7);
        if (cols >= 6)
        {
            // If UI only saved 6 columns, pad the 7th
            if (cols == 6) manualCopy(*(fields + 6), "N/A");

            Prescription p(Validator::charToInt(*(fields + 0)), Validator::charToInt(*(fields + 1)), Validator::charToInt(*(fields + 2)), Validator::charToInt(*(fields + 3)), *(fields + 4), *(fields + 5), *(fields + 6));
            store.add(p);
        }
    }
    file.close();
}

// ==========================================
// 3. APPENDERS & UPDATERS (Rule XIV Array Access Fixes)
// ==========================================

void FileHandler::appendPatient(const Patient& p) {
    ofstream file("patients.txt", ios::app);
    file << p.getId() << "," << p.getName() << "," << p.getAge() << "," << p.getGender() << "," << p.getContact() << "," << p.getPassword() << "," << p.getBalance() << "\n";
    file.close();
}

void FileHandler::appendDoctor(const Doctor& d) {
    ofstream file("doctors.txt", ios::app);
    file << d.getId() << "," << d.getName() << "," << d.getSpecialization() << "," << d.getContact() << "," << d.getPassword() << "," << d.getFee() << "\n";
    file.close();
}

void FileHandler::appendAppointment(const Appointment& a) {
    ofstream file("appointments.txt", ios::app);
    file << a.getId() << "," << a.getPatientId() << "," << a.getDoctorId() << "," << a.getDate() << "," << a.getTimeSlot() << "," << a.getStatus() << "\n";
    file.close();
}

void FileHandler::appendBill(const Bill& b) {
    ofstream file("bills.txt", ios::app);
    file << b.getId() << "," << b.getPatientId() << "," << b.getAppointmentId() << "," << b.getAmount() << "," << b.getStatus() << "," << b.getDate() << "\n";
    file.close();
}

void FileHandler::appendPrescription(const Prescription& p) {
    ofstream file("prescriptions.txt", ios::app);
    file << p.getId() << "," << p.getAppointmentId() << "," << p.getPatientId() << "," << p.getDoctorId() << "," << p.getDate() << "," << p.getMedicines() << "," << p.getNotes() << "\n";
    file.close();
}

void FileHandler::saveAllPatients(Storage<Patient>& store) {
    ofstream file("patients.txt", ios::trunc);
    Patient* arr = store.getAll();
    for (int i = 0; i < store.getSize(); i++) {
        file << (*(arr + i)).getId() << "," << (*(arr + i)).getName() << "," << (*(arr + i)).getAge() << "," << (*(arr + i)).getGender() << "," << (*(arr + i)).getContact() << "," << (*(arr + i)).getPassword() << "," << (*(arr + i)).getBalance() << "\n";
    }
    file.close();
}

void FileHandler::saveAllDoctors(Storage<Doctor>& store) {
    ofstream file("doctors.txt", ios::trunc);
    Doctor* arr = store.getAll();
    for (int i = 0; i < store.getSize(); i++) {
        file << (*(arr + i)).getId() << "," << (*(arr + i)).getName() << "," << (*(arr + i)).getSpecialization() << "," << (*(arr + i)).getContact() << "," << (*(arr + i)).getPassword() << "," << (*(arr + i)).getFee() << "\n";
    }
    file.close();
}

void FileHandler::saveAllAppointments(Storage<Appointment>& store) {
    ofstream file("appointments.txt", ios::trunc);
    Appointment* arr = store.getAll();
    for (int i = 0; i < store.getSize(); i++) {
        file << (*(arr + i)).getId() << "," << (*(arr + i)).getPatientId() << "," << (*(arr + i)).getDoctorId() << "," << (*(arr + i)).getDate() << "," << (*(arr + i)).getTimeSlot() << "," << (*(arr + i)).getStatus() << "\n";
    }
    file.close();
}

void FileHandler::saveAllBills(Storage<Bill>& store) {
    ofstream file("bills.txt", ios::trunc);
    Bill* arr = store.getAll();
    for (int i = 0; i < store.getSize(); i++) {
        file << (*(arr + i)).getId() << "," << (*(arr + i)).getPatientId() << "," << (*(arr + i)).getAppointmentId() << "," << (*(arr + i)).getAmount() << "," << (*(arr + i)).getStatus() << "," << (*(arr + i)).getDate() << "\n";
    }
    file.close();
}

void FileHandler::saveAllPrescriptions(Storage<Prescription>& store) {
    ofstream file("prescriptions.txt", ios::trunc);
    Prescription* arr = store.getAll();
    for (int i = 0; i < store.getSize(); i++) {
        file << (*(arr + i)).getId() << "," << (*(arr + i)).getAppointmentId() << "," << (*(arr + i)).getPatientId() << "," << (*(arr + i)).getDoctorId() << "," << (*(arr + i)).getDate() << "," << (*(arr + i)).getMedicines() << "," << (*(arr + i)).getNotes() << "\n";
    }
    file.close();
}

void FileHandler::appendDischargedPatient(const Patient& p) {
    ofstream file("discharged.txt", ios::app);
    file << p.getId() << "," << p.getName() << "," << p.getAge() << ","
        << p.getGender() << "," << p.getContact() << ","
        << p.getPassword() << "," << p.getBalance() << "\n";
    file.close();
}

void FileHandler::appendSecurityLog(const char* role, const char* enteredId, const char* result) {
    char ts[64];
    time_t raw = time(nullptr);
    struct tm* tinfo = localtime(&raw);
    strftime(ts, sizeof(ts), "%d-%m-%Y %H:%M:%S", tinfo); // Native timestamp generation

    ofstream file("security_log.txt", ios::app);
    file << ts << "," << role << "," << enteredId << "," << result << "\n";
    file.close();
}