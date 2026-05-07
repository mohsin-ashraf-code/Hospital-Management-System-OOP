#include "FileHandler.h"
#include "utility.h"
#include "Validator.h"
#include <fstream>
#include <iostream>

using namespace std;

// ==========================================
// 1. HELPER PARSING FUNCTIONS
// ==========================================

// Your perfect CSV parser
static int parseCSVLine(const char* line, char fields[][200], int maxFields) 
{
    int fieldIdx = 0, charIdx = 0;
    int i = 0;
    while (*(line + i) != '\0' && *(line + i) != '\n' && *(line + i) != '\r' && fieldIdx < maxFields) 
    {
        if (*(line + i) == ',') 
        {
            fields[fieldIdx][charIdx] = '\0';
            fieldIdx++; charIdx = 0;
        }
        else 
        {
            fields[fieldIdx][charIdx++] = *(line + i);
        }
        i++;
    }
    fields[fieldIdx][charIdx] = '\0';
    return fieldIdx + 1;
}

// ==========================================
// 2. LOADERS (Reading from txt files)
// ==========================================

void FileHandler::loadPatients(Storage<Patient>& store) 
{
    ifstream file("patients.txt");
    if (!file.is_open()) 
    {
        return;
    }

    char line[1024];
    while (file.getline(line, 1024)) 
    {
        char fields[7][200];
        if (parseCSVLine(line, fields, 7) == 7) 
        {
            // Updated to Validator::
            Patient p(Validator::charToInt(fields[0]), fields[1], fields[5], Validator::charToInt(fields[2]), fields[3][0], fields[4], Validator::charToFloat(fields[6]));
            store.add(p);
        }
    }
    file.close();
}

void FileHandler::loadDoctors(Storage<Doctor>& store) 
{
    ifstream file("doctors.txt");
    if (!file.is_open()) return;

    char line[1024];
    while (file.getline(line, 1024)) 
    {
        char fields[6][200];
        if (parseCSVLine(line, fields, 6) == 6) 
        {
            // Updated to Validator::
            Doctor d(Validator::charToInt(fields[0]), fields[1], fields[4],
                fields[2], fields[3], Validator::charToFloat(fields[5]));
            store.add(d);
        }
    }
    file.close();
}

void FileHandler::loadAdmin(Admin*& admin) 
{
    ifstream file("admin.txt");
    if (!file.is_open()) 
    {
        return;
    }

    char line[512];
    if (file.getline(line, 512)) 
    {
        char fields[3][200];
        if (parseCSVLine(line, fields, 3) == 3) 
        {
            // Updated to Validator::
            admin = new Admin(Validator::charToInt(fields[0]), fields[1], fields[2]);
        }
    }
    file.close();
}

void FileHandler::loadAppointments(Storage<Appointment>& store) 
{
    ifstream file("appointments.txt");
    if (!file.is_open()) 
    {
        return;
    }

    char line[1024];
    while (file.getline(line, 1024)) 
    {
        char fields[6][200];
        if (parseCSVLine(line, fields, 6) == 6) 
        {
            // Updated to Validator::
            Appointment a(Validator::charToInt(fields[0]), Validator::charToInt(fields[1]), Validator::charToInt(fields[2]), fields[3], fields[4], fields[5]);
            store.add(a);
        }
    }
    file.close();
}

void FileHandler::loadBills(Storage<Bill>& store) 
{
    ifstream file("bills.txt");
    if (!file.is_open()) 
    {
        return;
    }

    char line[1024];
    while (file.getline(line, 1024)) 
    {
        char fields[6][200];
        if (parseCSVLine(line, fields, 6) == 6) 
        {
            // Updated to Validator::
            Bill b(Validator::charToInt(fields[0]), Validator::charToInt(fields[1]), Validator::charToInt(fields[2]), Validator::charToFloat(fields[3]), fields[4], fields[5]);
            store.add(b);
        }
    }
    file.close();
}

void FileHandler::loadPrescriptions(Storage<Prescription>& store) 
{
    ifstream file("prescriptions.txt");
    if (!file.is_open()) return;

    char line[2048];
    while (file.getline(line, 2048)) 
    {
        char fields[7][200];
        if (parseCSVLine(line, fields, 7) == 7) 
        {
            // Updated to Validator::
            Prescription p(Validator::charToInt(fields[0]), Validator::charToInt(fields[1]), Validator::charToInt(fields[2]), Validator::charToInt(fields[3]),fields[4], fields[5], fields[6]);
            store.add(p);
        }
    }
    file.close();
}
// ==========================================
// 3. APPENDERS (Writing a single new record)
// ==========================================

void FileHandler::appendPatient(const Patient& p) 
{
    ofstream file("patients.txt", ios::app); // ios::app appends to the end
    file << p.getId() << "," << p.getName() << "," << p.getAge() << "," << p.getGender() << "," << p.getContact() << "," << p.getPassword() << "," << p.getBalance() << "\n";
    file.close();
}

void FileHandler::appendDoctor(const Doctor& d) 
{
    ofstream file("doctors.txt", ios::app);
    file << d.getId() << "," << d.getName() << "," << d.getSpecialization() << "," << d.getContact() << "," << d.getPassword() << "," << d.getFee() << "\n";
    file.close();
}

void FileHandler::appendAppointment(const Appointment& a) 
{
    ofstream file("appointments.txt", ios::app);
    file << a.getId() << "," << a.getPatientId() << "," << a.getDoctorId() << "," << a.getDate() << "," << a.getTimeSlot() << "," << a.getStatus() << "\n";
    file.close();
}

void FileHandler::appendBill(const Bill& b) 
{
    ofstream file("bills.txt", ios::app);
    file << b.getId() << "," << b.getPatientId() << "," << b.getAppointmentId() << "," << b.getAmount() << "," << b.getStatus() << "," << b.getDate() << "\n";
    file.close();
}

void FileHandler::appendPrescription(const Prescription& p) 
{
    ofstream file("prescriptions.txt", ios::app);
    file << p.getId() << "," << p.getAppointmentId() << "," << p.getPatientId() << "," << p.getDoctorId() << "," << p.getDate() << "," << p.getMedicines() << "," << p.getNotes() << "\n";
    file.close();
}

// ==========================================
// 4. UPDATERS (Overwriting full file on edit)
// ==========================================

void FileHandler::saveAllPatients(Storage<Patient>& store) 
{
    ofstream file("patients.txt", ios::trunc); // ios::trunc wipes the file clean first
    Patient* arr = store.getAll();
    for (int i = 0; i < store.size(); i++) 
    {
        file << arr[i].getId() << "," << arr[i].getName() << "," << arr[i].getAge() << "," << arr[i].getGender() << "," << arr[i].getContact() << "," << arr[i].getPassword() << "," << arr[i].getBalance() << "\n";
    }
    file.close();
}

void FileHandler::saveAllDoctors(Storage<Doctor>& store) 
{
    ofstream file("doctors.txt", ios::trunc);
    Doctor* arr = store.getAll();
    for (int i = 0; i < store.size(); i++) 
    {
        file << arr[i].getId() << "," << arr[i].getName() << "," << arr[i].getSpecialization() << "," << arr[i].getContact() << "," << arr[i].getPassword() << "," << arr[i].getFee() << "\n";
    }
    file.close();
}

void FileHandler::saveAllAppointments(Storage<Appointment>& store) 
{
    ofstream file("appointments.txt", ios::trunc);
    Appointment* arr = store.getAll();
    for (int i = 0; i < store.size(); i++) 
    {
        file << arr[i].getId() << "," << arr[i].getPatientId() << "," << arr[i].getDoctorId() << "," << arr[i].getDate() << "," << arr[i].getTimeSlot() << "," << arr[i].getStatus() << "\n";
    }
    file.close();
}

void FileHandler::saveAllBills(Storage<Bill>& store) 
{
    ofstream file("bills.txt", ios::trunc);
    Bill* arr = store.getAll();
    for (int i = 0; i < store.size(); i++) 
    {
        file << arr[i].getId() << "," << arr[i].getPatientId() << "," << arr[i].getAppointmentId() << "," << arr[i].getAmount() << "," << arr[i].getStatus() << "," << arr[i].getDate() << "\n";
    }
    file.close();
}

void FileHandler::saveAllPrescriptions(Storage<Prescription>& store) 
{
    // ios::trunc wipes the file clean first so we don't duplicate data
    ofstream file("prescriptions.txt", ios::trunc);
    Prescription* arr = store.getAll();

    for (int i = 0; i < store.size(); i++) 
    {
        file << arr[i].getId() << "," << arr[i].getAppointmentId() << "," << arr[i].getPatientId() << "," << arr[i].getDoctorId() << "," << arr[i].getDate() << "," << arr[i].getMedicines() << "," << arr[i].getNotes() << "\n";
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

// ==========================================
// 5. SECURITY LOG
// ==========================================
void FileHandler::appendSecurityLog(const char* role, const char* enteredId, const char* result) {
    char ts[50];
    getCurrentTimestamp(ts, 50); // Get the real time!

    ofstream file("security_log.txt", ios::app);
    file << ts << "," << role << "," << enteredId << "," << result << "\n";
    file.close();
}