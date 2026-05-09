#include "Prescription.h"
#include "utility.h"

using namespace std;

// --- Default Constructor ---
Prescription::Prescription() {
    id = 0;
    appointmentId = 0;
    patientId = 0;
    doctorId = 0;

    date = STRING("");
    medicines[0] = '\0';
    notes[0] = '\0';
}

// --- Parameterized Constructor ---
Prescription::Prescription(int id, int appointmentId, int patientId, int doctorId, const char* date, const char* meds, const char* nts) {
    this->id = id;
    this->appointmentId = appointmentId;
    this->patientId = patientId;
    this->doctorId = doctorId;

    this->date = STRING(date);

    int i = 0;
    if (meds != nullptr) {
        while (*(meds + i) != '\0' && i < 499) {
            medicines[i] = *(meds + i);
            i++;
        }
    }
    medicines[i] = '\0';

    int j = 0;
    if (nts != nullptr) {
        while (*(nts + j) != '\0' && j < 299) {
            notes[j] = *(nts + j);
            j++;
        }
    }
    notes[j] = '\0';
}

// --- Copy Constructor ---
Prescription::Prescription(const Prescription& other) {
    id = other.id;
    appointmentId = other.appointmentId;
    patientId = other.patientId;
    doctorId = other.doctorId;

    date = other.date;
    int i = 0;
    while (other.medicines[i] != '\0' && i < 499) {
        medicines[i] = other.medicines[i];
        i++;
    }
    medicines[i] = '\0';

    int j = 0;
    while (other.notes[j] != '\0' && j < 299) {
        notes[j] = other.notes[j];
        j++;
    }
    notes[j] = '\0';
}

// --- Assignment Operator ---
Prescription& Prescription::operator=(const Prescription& other) {
    if (this != &other) {
        id = other.id;
        appointmentId = other.appointmentId;
        patientId = other.patientId;
        doctorId = other.doctorId;
        date = other.date;

        int i = 0;
        while (other.medicines[i] != '\0' && i < 499) {
            medicines[i] = other.medicines[i];
            i++;
        }
        medicines[i] = '\0';

        int j = 0;
        while (other.notes[j] != '\0' && j < 299) {
            notes[j] = other.notes[j];
            j++;
        }
        notes[j] = '\0';
    }
    return *this;
}
bool Prescription::operator==(int searchId) const
{
    return this->id == searchId;
}
// --- Destructor ---
Prescription::~Prescription() {
}

// --- Getters ---
int Prescription::getId() const { return id; }
int Prescription::getAppointmentId() const { return appointmentId; }
int Prescription::getPatientId() const { return patientId; }
int Prescription::getDoctorId() const { return doctorId; }
const char* Prescription::getDate() const { return date.getData(); }
const char* Prescription::getMedicines() const { return medicines; }
const char* Prescription::getNotes() const { return notes; }