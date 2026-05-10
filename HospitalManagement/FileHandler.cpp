#define _CRT_SECURE_NO_WARNINGS
#include "FileHandler.h"
#include "utility.h"
#include "Validator.h"
#include <fstream>
#include <ctime>

// --- RULE XIV COMPLIANT PARSER (NO STATIC ARRAY SUBSCRIPTS []) ---
static int parseCSVLine(const char* line, char fields[10][200], int maxCols) {
    int col = 0;
    int charIdx = 0;
    int i = 0;

    // Initialize all string buffers to empty
    for (int c = 0; c < maxCols; c++) {
        *(*(fields + c) + 0) = '\0';
    }

    while (*(line + i) != '\0' && col < maxCols) {
        if (*(line + i) == ',') {
            *(*(fields + col) + charIdx) = '\0';
            col++;
            charIdx = 0;
        }
        else if (*(line + i) != '\r' && *(line + i) != '\n') {
            if (charIdx < 199) {
                *(*(fields + col) + charIdx) = *(line + i);
                charIdx++;
            }
        }
        i++;
    }

    if (col < maxCols) {
        *(*(fields + col) + charIdx) = '\0';
        col++;
    }
    return col;
}

// ==========================================================
// DATA LOADERS
// ==========================================================

void FileHandler::loadPatients(Storage<Patient>& store) {
    std::ifstream file("patients.txt");
    if (!file.is_open()) {
        std::ofstream create("patients.txt");
        create.close();
        return;
    }

    char line[1024];
    while (file.getline(line, 1024)) {
        char fields[10][200];
        if (parseCSVLine(line, fields, 7) >= 7) {
            // patient_id,name,age,gender,contact,password,balance
            Patient p(
                Validator::charToInt(*(fields + 0)),
                *(fields + 1),
                *(fields + 5),
                Validator::charToInt(*(fields + 2)),
                *(*(fields + 3) + 0),
                *(fields + 4),
                Validator::charToFloat(*(fields + 6))
            );
            store.add(p);
        }
    }
    file.close();
}

void FileHandler::loadDoctors(Storage<Doctor>& store) {
    std::ifstream file("doctors.txt");
    if (!file.is_open()) {
        std::ofstream create("doctors.txt");
        create.close();
        return;
    }

    char line[1024];
    while (file.getline(line, 1024)) {
        char fields[10][200];
        if (parseCSVLine(line, fields, 6) >= 6) {
            // doctor_id,name,specialization,contact,password,fee
            Doctor d(
                Validator::charToInt(*(fields + 0)),
                *(fields + 1),
                *(fields + 2),
                *(fields + 3),
                *(fields + 4),
                Validator::charToFloat(*(fields + 5))
            );
            store.add(d);
        }
    }
    file.close();
}

void FileHandler::loadAdmin(Admin*& admin) {
    std::ifstream file("admin.txt");
    if (!file.is_open()) {
        // Create default admin credentials if missing
        std::ofstream create("admin.txt");
        create << "1,Admin,admin123\n";
        create.close();
        admin = new Admin(1, "Admin", "admin123");
        return;
    }

    char line[1024];
    if (file.getline(line, 1024)) {
        char fields[10][200];
        if (parseCSVLine(line, fields, 3) >= 3) {
            admin = new Admin(
                Validator::charToInt(*(fields + 0)),
                *(fields + 1),
                *(fields + 2)
            );
        }
    }
    file.close();
}

void FileHandler::loadAppointments(Storage<Appointment>& store) {
    std::ifstream file("appointments.txt");
    if (!file.is_open()) {
        std::ofstream create("appointments.txt");
        create.close();
        return;
    }

    char line[1024];
    while (file.getline(line, 1024)) {
        char fields[10][200];
        if (parseCSVLine(line, fields, 6) >= 6) {
            // appointment_id,patient_id,doctor_id,date,time_slot,status
            Appointment a(
                Validator::charToInt(*(fields + 0)),
                Validator::charToInt(*(fields + 1)),
                Validator::charToInt(*(fields + 2)),
                *(fields + 3),
                *(fields + 4),
                *(fields + 5)
            );
            store.add(a);
        }
    }
    file.close();
}

void FileHandler::loadBills(Storage<Bill>& store) {
    std::ifstream file("bills.txt");
    if (!file.is_open()) {
        std::ofstream create("bills.txt");
        create.close();
        return;
    }

    char line[1024];
    while (file.getline(line, 1024)) {
        char fields[10][200];
        if (parseCSVLine(line, fields, 6) >= 6) {
            // bill_id,patient_id,appointment_id,amount,status,date
            Bill b(
                Validator::charToInt(*(fields + 0)),
                Validator::charToInt(*(fields + 1)),
                Validator::charToInt(*(fields + 2)),
                Validator::charToFloat(*(fields + 3)),
                *(fields + 4),
                *(fields + 5)
            );
            store.add(b);
        }
    }
    file.close();
}

void FileHandler::loadPrescriptions(Storage<Prescription>& store) {
    std::ifstream file("prescriptions.txt");
    if (!file.is_open()) {
        std::ofstream create("prescriptions.txt");
        create.close();
        return;
    }

    char line[1024];
    while (file.getline(line, 1024)) {
        char fields[10][200];
        if (parseCSVLine(line, fields, 7) >= 7) {
            // prescription_id,appointment_id,patient_id,doctor_id,date,medicines,notes
            Prescription pr(
                Validator::charToInt(*(fields + 0)),
                Validator::charToInt(*(fields + 1)),
                Validator::charToInt(*(fields + 2)),
                Validator::charToInt(*(fields + 3)),
                *(fields + 4),
                *(fields + 5),
                *(fields + 6)
            );
            store.add(pr);
        }
    }
    file.close();
}

// ==========================================================
// DATA SAVERS & APPENDS (CLEAN WRITES WITHOUT CORRUPT DUPLICATES)
// ==========================================================

void FileHandler::saveAllPatients(Storage<Patient>& store) {
    std::ofstream file("patients.txt");
    if (file.is_open()) {
        for (int i = 0; i < store.getSize(); i++) {
            Patient* p = store.getAt(i);
            file << p->getId() << "," << p->getName() << "," << p->getAge() << ","
                << p->getGender() << "," << p->getContact() << "," << p->getPassword() << ","
                << p->getBalance() << "\n";
        }
        file.close();
    }
}

void FileHandler::appendPatient(const Patient& p) {
    std::ofstream file("patients.txt", std::ios::app);
    if (file.is_open()) {
        file << p.getId() << "," << p.getName() << "," << p.getAge() << ","
            << p.getGender() << "," << p.getContact() << "," << p.getPassword() << ","
            << p.getBalance() << "\n";
        file.close();
    }
}

void FileHandler::saveAllDoctors(Storage<Doctor>& store) {
    std::ofstream file("doctors.txt");
    if (file.is_open()) {
        for (int i = 0; i < store.getSize(); i++) {
            Doctor* d = store.getAt(i);
            file << d->getId() << "," << d->getName() << "," << d->getSpecialization() << ","
                << d->getContact() << "," << d->getPassword() << "," << d->getFee() << "\n";
        }
        file.close();
    }
}

void FileHandler::appendDoctor(const Doctor& d) {
    std::ofstream file("doctors.txt", std::ios::app);
    if (file.is_open()) {
        file << d.getId() << "," << d.getName() << "," << d.getSpecialization() << ","
            << d.getContact() << "," << d.getPassword() << "," << d.getFee() << "\n";
        file.close();
    }
}

void FileHandler::appendDischargedPatient(const Patient& p) {
    std::ofstream file("discharged.txt", std::ios::app);
    if (file.is_open()) {
        file << p.getId() << "," << p.getName() << "," << p.getAge() << ","
            << p.getGender() << "," << p.getContact() << "," << p.getPassword() << ","
            << p.getBalance() << "\n";
        file.close();
    }
}

void FileHandler::appendAppointment(const Appointment& a) {
    std::ofstream file("appointments.txt", std::ios::app);
    if (file.is_open()) {
        file << a.getId() << "," << a.getPatientId() << "," << a.getDoctorId() << ","
            << a.getDate() << "," << a.getTimeSlot() << "," << a.getStatus() << "\n";
        file.close();
    }
}

void FileHandler::saveAllAppointments(Storage<Appointment>& store) {
    std::ofstream file("appointments.txt");
    if (file.is_open()) {
        for (int i = 0; i < store.getSize(); i++) {
            Appointment* a = store.getAt(i);
            file << a->getId() << "," << a->getPatientId() << "," << a->getDoctorId() << ","
                << a->getDate() << "," << a->getTimeSlot() << "," << a->getStatus() << "\n";
        }
        file.close();
    }
}

void FileHandler::appendBill(const Bill& b) {
    std::ofstream file("bills.txt", std::ios::app);
    if (file.is_open()) {
        file << b.getId() << "," << b.getPatientId() << "," << b.getAppointmentId() << ","
            << b.getAmount() << "," << b.getStatus() << "," << b.getDate() << "\n";
        file.close();
    }
}

void FileHandler::saveAllBills(Storage<Bill>& store) {
    std::ofstream file("bills.txt");
    if (file.is_open()) {
        for (int i = 0; i < store.getSize(); i++) {
            Bill* b = store.getAt(i);
            file << b->getId() << "," << b->getPatientId() << "," << b->getAppointmentId() << ","
                << b->getAmount() << "," << b->getStatus() << "," << b->getDate() << "\n";
        }
        file.close();
    }
}

void FileHandler::appendPrescription(const Prescription& pr) {
    std::ofstream file("prescriptions.txt", std::ios::app);
    if (file.is_open()) {
        file << pr.getId() << "," << pr.getAppointmentId() << "," << pr.getPatientId() << ","
            << pr.getDoctorId() << "," << pr.getDate() << "," << pr.getMedicines() << ","
            << pr.getNotes() << "\n";
        file.close();
    }
}

void FileHandler::saveAllPrescriptions(Storage<Prescription>& store) {
    std::ofstream file("prescriptions.txt");
    if (file.is_open()) {
        for (int i = 0; i < store.getSize(); i++) {
            Prescription* pr = store.getAt(i);
            file << pr->getId() << "," << pr->getAppointmentId() << "," << pr->getPatientId() << ","
                << pr->getDoctorId() << "," << pr->getDate() << "," << pr->getMedicines() << ","
                << pr->getNotes() << "\n";
        }
        file.close();
    }
}

void FileHandler::appendSecurityLog(const char* role, const char* enteredId, const char* result) {
    char ts[64];
    time_t raw = time(nullptr);
    struct tm* tinfo = localtime(&raw);
    strftime(ts, sizeof(ts), "%d-%m-%Y %H:%M:%S", tinfo);

    std::ofstream file("security_log.txt", std::ios::app);
    if (file.is_open()) {
        file << ts << "," << role << "," << enteredId << "," << result << "\n";
        file.close();
    }
}