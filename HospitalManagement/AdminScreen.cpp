#define _CRT_SECURE_NO_WARNINGS
#include "AdminScreen.h"
#include "Validator.h"
#include "FileHandler.h"
#include <fstream>
#include <ctime>

// --- POINTER-SAFE STRING GENERATORS ---
sf::String intToStr(int num) {
    if (num == 0) return "0";
    sf::String res = "";
    bool neg = (num < 0);
    if (neg) num = -num;
    while (num > 0) {
        res = sf::String((char)('0' + (num % 10))) + res;
        num /= 10;
    }
    return neg ? "-" + res : res;
}

sf::String floatToStr(float num) {
    int intPart = static_cast<int>(num);
    int decPart = static_cast<int>((num - intPart) * 100);
    if (decPart < 0) decPart = -decPart;
    return intToStr(intPart) + "." + (decPart < 10 ? "0" : "") + intToStr(decPart);
}
// --------------------------------------

AdminScreen::AdminScreen() : state(nullptr), title(nullptr) {
    for (int i = 0; i < 10; i++) {
        *(buttons + i) = nullptr;
    }
}

AdminScreen::~AdminScreen() {
    delete title;
    for (int i = 0; i < 10; i++) {
        delete* (buttons + i);
    }
}

int AdminScreen::calculateDaysBetween(const char* dateStr) {
    if (!dateStr || Validator::myStrLen(dateStr) < 10) return 0;

    struct tm fileTime = { 0 };
    fileTime.tm_mday = (*(dateStr + 0) - '0') * 10 + (*(dateStr + 1) - '0');
    fileTime.tm_mon = ((*(dateStr + 3) - '0') * 10 + (*(dateStr + 4) - '0')) - 1;
    fileTime.tm_year = ((*(dateStr + 6) - '0') * 1000 + (*(dateStr + 7) - '0') * 100 + (*(dateStr + 8) - '0') * 10 + (*(dateStr + 9) - '0')) - 1900;

    time_t parsed = mktime(&fileTime);
    time_t now = time(nullptr);

    double diffSeconds = difftime(now, parsed);
    return static_cast<int>(diffSeconds / (60 * 60 * 24));
}

void AdminScreen::init(const sf::Font& font, AppState& appState) {
    state = &appState;

    title = new sf::Text(font);
    title->setString("Admin Panel - MediCore");
    title->setCharacterSize(35);
    title->setFillColor(sf::Color::Cyan);
    title->setPosition({ 50.f, 30.f });

    const char* labels[] = {
        "1. Add Doctor", "2. Remove Doctor", "3. View All Patients",
        "4. View All Doctors", "5. View All Appointments", "6. View Unpaid Bills",
        "7. Discharge Patient", "8. View Security Log", "9. Generate Daily Report", "10. Logout"
    };

    for (int i = 0; i < 10; i++) {
        sf::Color idle = (i == 9) ? sf::Color(200, 50, 50) : sf::Color(0, 102, 204);
        sf::Color hover = (i == 9) ? sf::Color(150, 0, 0) : sf::Color(0, 120, 240);

        float xPos = (i < 5) ? 50.f : 400.f;
        float yPos = 120.f + ((i % 5) * 80.f);

        *(buttons + i) = new Button({ 300.f, 50.f }, { xPos, yPos }, *(labels + i), font, idle, hover);
    }

    const char* docFields[] = { "Name", "Specialization", "Contact (11 Digits)", "Password", "Fee (PKR)" };
    addDoctorForm.init(font, "Add Doctor", docFields, 5);

    const char* remFields[] = { "Enter Doctor ID" };
    removeDoctorForm.init(font, "Remove Doctor", remFields, 1);

    const char* dischargeFields[] = { "Enter Patient ID" };
    dischargeForm.init(font, "Discharge Patient", dischargeFields, 1);

    dataViewer.init(font, "System Viewer");
}

void AdminScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window, AppState& appState) {
    if (dataViewer.isActive()) {
        dataViewer.handleEvent(event, window);
        return;
    }

    if (addDoctorForm.isActive()) {
        addDoctorForm.handleEvent(event, window, [&](const char** formData) {
            const char* name = *(formData + 0);
            const char* spec = *(formData + 1);
            const char* contact = *(formData + 2);
            const char* pass = *(formData + 3);
            const char* feeStr = *(formData + 4);

            if (!Validator::isValidContact(contact)) {
                dataViewer.show("Error: Contact must be exactly 11 numeric digits.");
                return;
            }
            if (!Validator::isValidPassword(pass)) {
                dataViewer.show("Error: Password must be minimum 6 characters.");
                return;
            }
            if (!Validator::isPositiveFloat(feeStr)) {
                dataViewer.show("Error: Fee must be a positive float.");
                return;
            }

            int newId = state->doctors.getSize() > 0 ? (*(state->doctors.getAt(state->doctors.getSize() - 1))).getId() + 1 : 1;
            Doctor d(newId, name, spec, contact, pass, Validator::charToFloat(feeStr));

            state->doctors.add(d);
            FileHandler::appendDoctor(d);

            dataViewer.show("Doctor added successfully. ID: " + intToStr(newId));
            });
        return;
    }

    if (removeDoctorForm.isActive()) {
        removeDoctorForm.handleEvent(event, window, [&](const char** formData) {
            int did = Validator::charToInt(*(formData + 0));

            bool hasPending = false;
            for (int i = 0; i < state->appointments.getSize(); i++) {
                Appointment* a = state->appointments.getAt(i);
                if (a->getDoctorId() == did && Validator::myStrEqual(a->getStatus(), "pending")) {
                    hasPending = true;
                    break;
                }
            }

            if (hasPending) {
                dataViewer.show("Cannot remove doctor with pending appointments. Cancel or reassign them first.");
                return;
            }

            if (state->doctors.removeByID(did)) {
                FileHandler::saveAllDoctors(state->doctors);
                dataViewer.show("Doctor removed successfully.");
            }
            else {
                dataViewer.show("Doctor ID not found.");
            }
            });
        return;
    }

    if (dischargeForm.isActive()) {
        dischargeForm.handleEvent(event, window, [&](const char** formData) {
            int pid = Validator::charToInt(*(formData + 0));
            Patient* p = state->patients.findByID(pid);

            if (!p) {
                dataViewer.show("Patient ID not found.");
                return;
            }

            for (int i = 0; i < state->bills.getSize(); i++) {
                Bill* b = state->bills.getAt(i);
                if (b->getPatientId() == pid && Validator::myStrEqual(b->getStatus(), "unpaid")) {
                    dataViewer.show("Cannot discharge patient with unpaid bills.");
                    return;
                }
            }

            for (int i = 0; i < state->appointments.getSize(); i++) {
                Appointment* a = state->appointments.getAt(i);
                if (a->getPatientId() == pid && Validator::myStrEqual(a->getStatus(), "pending")) {
                    dataViewer.show("Cannot discharge patient with pending appointments.");
                    return;
                }
            }

            FileHandler::appendDischargedPatient(*p);
            state->patients.removeByID(pid);
            FileHandler::saveAllPatients(state->patients);

            dataViewer.show("Patient discharged and archived successfully.");
            });
        return;
    }

    for (int i = 0; i < 10; i++) {
        (*(buttons + i))->handleEvent(event, window, [&, i]() {
            if (i == 0) {
                addDoctorForm.show();
            }
            else if (i == 1) {
                removeDoctorForm.show();
            }
            else if (i == 2) {
                sf::String display = "--- ALL PATIENTS ---\n";
                for (int j = 0; j < state->patients.getSize(); j++) {
                    Patient* p = state->patients.getAt(j);
                    int unpaidCount = 0;
                    for (int k = 0; k < state->bills.getSize(); k++) {
                        Bill* b = state->bills.getAt(k);
                        if (b->getPatientId() == p->getId() && Validator::myStrEqual(b->getStatus(), "unpaid")) unpaidCount++;
                    }
                    display += "ID: " + intToStr(p->getId()) + " | Name: " + sf::String(p->getName()) + " | Bal: " + intToStr((int)p->getBalance()) + " | Unpaid: " + intToStr(unpaidCount) + "\n";
                }
                dataViewer.show(display);
            }
            else if (i == 3) {
                sf::String display = "--- ALL DOCTORS ---\n";
                for (int j = 0; j < state->doctors.getSize(); j++) {
                    Doctor* d = state->doctors.getAt(j);
                    display += "ID: " + intToStr(d->getId()) + " | Dr. " + sf::String(d->getName()) + " | Spec: " + sf::String(d->getSpecialization()) + " | Fee: " + intToStr((int)d->getFee()) + "\n";
                }
                dataViewer.show(display);
            }
            else if (i == 4) {
                sf::String display = "--- ALL APPOINTMENTS ---\n";
                for (int j = 0; j < state->appointments.getSize(); j++) {
                    Appointment* a = state->appointments.getAt(j);
                    display += "ID: " + intToStr(a->getId()) + " | P_ID: " + intToStr(a->getPatientId()) + " | D_ID: " + intToStr(a->getDoctorId()) + " | Date: " + sf::String(a->getDate()) + " | " + sf::String(a->getStatus()) + "\n";
                }
                dataViewer.show(display);
            }
            else if (i == 5) {
                sf::String display = "--- UNPAID BILLS ---\n";
                for (int j = 0; j < state->bills.getSize(); j++) {
                    Bill* b = state->bills.getAt(j);
                    if (Validator::myStrEqual(b->getStatus(), "unpaid")) {
                        display += "Bill ID: " + intToStr(b->getId()) + " | Amt: " + intToStr((int)b->getAmount()) + " | Date: " + sf::String(b->getDate());
                        if (calculateDaysBetween(b->getDate()) > 7) {
                            display += " [OVERDUE]";
                        }
                        display += "\n";
                    }
                }
                dataViewer.show(display);
            }
            else if (i == 6) {
                dischargeForm.show();
            }
            else if (i == 7) {
                std::ifstream file("security_log.txt");
                sf::String display = "--- SECURITY LOG ---\n";
                if (file.is_open()) {
                    char c;
                    while (file.get(c) && display.getSize() < 1500) {
                        display += c;
                    }
                    file.close();
                }
                else {
                    display += "No security events logged.";
                }
                dataViewer.show(display);
            }
            else if (i == 8) {
                char timeBuf[64];
                time_t raw = time(nullptr);
                struct tm* tinfo = localtime(&raw);
                strftime(timeBuf, sizeof(timeBuf), "%d-%m-%Y", tinfo);

                int totToday = 0, pending = 0, comp = 0, noshow = 0, canc = 0;
                float rev = 0.0f;

                for (int j = 0; j < state->appointments.getSize(); j++) {
                    Appointment* a = state->appointments.getAt(j);
                    if (Validator::myStrEqual(a->getDate(), timeBuf)) {
                        totToday++;
                        const char* s = a->getStatus();
                        if (Validator::myStrEqual(s, "pending")) pending++;
                        else if (Validator::myStrEqual(s, "completed")) comp++;
                        else if (Validator::myStrEqual(s, "noshow")) noshow++;
                        else if (Validator::myStrEqual(s, "cancelled")) canc++;
                    }
                }
                for (int j = 0; j < state->bills.getSize(); j++) {
                    Bill* b = state->bills.getAt(j);
                    if (Validator::myStrEqual(b->getDate(), timeBuf) && Validator::myStrEqual(b->getStatus(), "paid")) {
                        rev += b->getAmount();
                    }
                }

                sf::String display = "--- DAILY REPORT (" + sf::String(timeBuf) + ") ---\n";
                display += "Total Appts: " + intToStr(totToday) + " (P:" + intToStr(pending) + " C:" + intToStr(comp) + " NS:" + intToStr(noshow) + " CX:" + intToStr(canc) + ")\n";
                display += "Revenue: PKR " + intToStr((int)rev) + "\n";
                dataViewer.show(display);
            }
            else if (i == 9) {
                appState.currentScreen = ScreenType::Login;
            }
            });
    }
}

void AdminScreen::update(float dt, sf::RenderWindow& window) {
    if (dataViewer.isActive()) dataViewer.update(window);
    else if (addDoctorForm.isActive()) addDoctorForm.update(window);
    else if (removeDoctorForm.isActive()) removeDoctorForm.update(window);
    else if (dischargeForm.isActive()) dischargeForm.update(window);
    else {
        for (int i = 0; i < 10; i++) (*(buttons + i))->update(window);
    }
}

void AdminScreen::draw(sf::RenderWindow& window) {
    window.draw(*title);
    for (int i = 0; i < 10; i++) (*(buttons + i))->draw(window);

    if (addDoctorForm.isActive()) addDoctorForm.draw(window);
    if (removeDoctorForm.isActive()) removeDoctorForm.draw(window);
    if (dischargeForm.isActive()) dischargeForm.draw(window);
    if (dataViewer.isActive()) dataViewer.draw(window);
}