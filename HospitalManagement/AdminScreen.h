#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include "AppState.h"
#include "UIComponents.h"
#include "utility.h"
#include "Validator.h"

class AdminScreen {
private:
    AppState* state;
    sf::Text* title;
    Button* buttons[8];

    InputForm addDoctorForm;
    InputForm addPatientForm;
    DataViewer dataViewer;

public:
    AdminScreen() : state(nullptr), title(nullptr) {
        for (int i = 0; i < 8; i++) buttons[i] = nullptr;
    }

    ~AdminScreen() {
        delete title;
        for (int i = 0; i < 8; i++) delete buttons[i];
    }

    void init(const sf::Font& font, AppState& appState) {
        state = &appState;

        title = new sf::Text(font);
        title->setString("Admin Dashboard - MediCore");
        title->setCharacterSize(40);
        title->setFillColor(sf::Color(0, 51, 102));
        title->setPosition({ 50.f, 50.f });

        const char* labels[] = {
            "1. Add Doctor", "2. View All Doctors", "3. Add Patient",
            "4. View All Patients", "5. View Appointments", "6. View Bills",
            "7. System Summary", "8. Logout"
        };

        for (int i = 0; i < 8; i++) {
            sf::Color idle = (i == 7) ? sf::Color(200, 50, 50) : sf::Color(0, 120, 215);
            sf::Color hover = (i == 7) ? sf::Color(150, 0, 0) : sf::Color(0, 80, 160);
            buttons[i] = new Button({ 400.f, 50.f }, { 50.f, 150.f + (i * 60.f) }, labels[i], font, idle, hover);
        }

        const char* docFields[] = { "Name", "Specialization", "Contact", "Password", "Fee (PKR)" };
        addDoctorForm.init(font, "Register New Doctor", docFields, 5);

        const char* patFields[] = { "Name", "Age", "Gender (M/F)", "Contact", "Password", "Init Balance" };
        addPatientForm.init(font, "Register New Patient", patFields, 6);

        dataViewer.init(font, "Admin Records");
    }

    void handleEvent(const sf::Event& event, sf::RenderWindow& window, AppState& appState) {

        // --- 0. DATA VIEWER OVERLAY ---
        if (dataViewer.isActive()) {
            dataViewer.handleEvent(event, window);
            return;
        }

        // --- 1. ADD DOCTOR FORM ---
        if (addDoctorForm.isActive()) {
            addDoctorForm.handleEvent(event, window, [&](const char** formData) {
                int newId = state->doctors.getSize() + 1;
                float newFee = myAtof(formData[4]);

                Doctor newDoc(newId, formData[0], formData[3], formData[1], formData[2], newFee);
                state->doctors.add(newDoc);

                std::ofstream file("doctors.txt", std::ios::app);
                if (file.is_open()) {
                    file << "\n" << newId << "," << formData[0] << "," << formData[1] << ","
                        << formData[2] << "," << formData[3] << "," << newFee;
                    file.close();
                }
                });
            return;
        }

        // --- 2. ADD PATIENT FORM ---
        if (addPatientForm.isActive()) {
            addPatientForm.handleEvent(event, window, [&](const char** formData) {
                int newId = state->patients.getSize() + 1;
                int newAge = Validator::charToInt(formData[1]);
                char newGender = formData[2][0];
                float newBal = myAtof(formData[5]);

                Patient newPat(newId, formData[0], formData[4], newAge, newGender, formData[3], newBal);
                state->patients.add(newPat);

                std::ofstream file("patients.txt", std::ios::app);
                if (file.is_open()) {
                    file << "\n" << newId << "," << formData[0] << "," << newAge << ","
                        << newGender << "," << formData[3] << "," << formData[4] << "," << newBal;
                    file.close();
                }
                });
            return;
        }

        // --- BACKGROUND BUTTON CLICKS ---
        for (int i = 0; i < 8; i++) {
            buttons[i]->handleEvent(event, window, [&, i]() {

                if (i == 0) addDoctorForm.show(); // 1. Add Doctor

                else if (i == 1) { // 2. View All Doctors (RAM)
                    char displayBuffer[8192]; displayBuffer[0] = '\0';
                    for (int j = 0; j < state->doctors.getSize(); j++) {
                        Doctor* d = state->doctors.getAt(j);
                        if (d != nullptr) {
                            char temp[256]; char feeStr[32]; char idStr[16];
                            myFloatToStr(d->getFee(), feeStr);
                            myIntToStr(d->getId(), idStr);

                            myStrCopy(temp, "Doc ID: "); myStrCopy(temp + myStrLen(temp), idStr);
                            myStrCopy(temp + myStrLen(temp), " | Name: Dr. "); myStrCopy(temp + myStrLen(temp), d->getName());
                            myStrCopy(temp + myStrLen(temp), " | Spec: "); myStrCopy(temp + myStrLen(temp), d->getSpecialization());
                            myStrCopy(temp + myStrLen(temp), " | Fee: PKR "); myStrCopy(temp + myStrLen(temp), feeStr);
                            myStrCopy(temp + myStrLen(temp), "\n");
                            myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                        }
                    }
                    if (myStrLen(displayBuffer) == 0) myStrCopy(displayBuffer, "No doctors found in RAM.");
                    dataViewer.show(displayBuffer);
                }

                else if (i == 2) addPatientForm.show(); // 3. Add Patient

                else if (i == 3) { // 4. View All Patients (RAM)
                    char displayBuffer[8192]; displayBuffer[0] = '\0';
                    for (int j = 0; j < state->patients.getSize(); j++) {
                        Patient* p = state->patients.getAt(j);
                        if (p != nullptr) {
                            char temp[256]; char balStr[32]; char idStr[16]; char ageStr[16];
                            myFloatToStr(p->getBalance(), balStr);
                            myIntToStr(p->getId(), idStr);
                            myIntToStr(p->getAge(), ageStr);

                            myStrCopy(temp, "Pat ID: "); myStrCopy(temp + myStrLen(temp), idStr);
                            myStrCopy(temp + myStrLen(temp), " | Name: "); myStrCopy(temp + myStrLen(temp), p->getName());
                            myStrCopy(temp + myStrLen(temp), " | Age: "); myStrCopy(temp + myStrLen(temp), ageStr);
                            myStrCopy(temp + myStrLen(temp), " | Balance: PKR "); myStrCopy(temp + myStrLen(temp), balStr);
                            myStrCopy(temp + myStrLen(temp), "\n");
                            myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                        }
                    }
                    if (myStrLen(displayBuffer) == 0) myStrCopy(displayBuffer, "No patients found in RAM.");
                    dataViewer.show(displayBuffer);
                }

                else if (i == 4) { // 5. View Appointments (File via Manual Parser)
                    char displayBuffer[8192]; displayBuffer[0] = '\0';
                    std::ifstream inFile("appointments.txt");
                    if (inFile.is_open()) {
                        char line[256];
                        while (inFile.getline(line, sizeof(line))) {
                            if (myStrLen(line) < 5) continue;
                            char lineCopy[256]; myStrCopy(lineCopy, line);

                            // Safe Manual Parser
                            char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                                if (lineCopy[k] == ',') { lineCopy[k] = '\0'; cols[colIdx++] = &lineCopy[k + 1]; }
                            }

                            if (colIdx >= 5) {
                                char temp[256];
                                myStrCopy(temp, "Appt ID: "); myStrCopy(temp + myStrLen(temp), cols[0]);
                                myStrCopy(temp + myStrLen(temp), " | Pat ID: "); myStrCopy(temp + myStrLen(temp), cols[1]);
                                myStrCopy(temp + myStrLen(temp), " | Doc ID: "); myStrCopy(temp + myStrLen(temp), cols[2]);
                                myStrCopy(temp + myStrLen(temp), " | Date: "); myStrCopy(temp + myStrLen(temp), cols[3]);
                                myStrCopy(temp + myStrLen(temp), " | Status: "); myStrCopy(temp + myStrLen(temp), cols[4]);
                                myStrCopy(temp + myStrLen(temp), "\n");
                                myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                            }
                        }
                        inFile.close();
                    }
                    if (myStrLen(displayBuffer) == 0) myStrCopy(displayBuffer, "No appointments in database.");
                    dataViewer.show(displayBuffer);
                }

                else if (i == 5) { // 6. View Bills (File via Manual Parser)
                    char displayBuffer[8192]; displayBuffer[0] = '\0';
                    std::ifstream inFile("bills.txt");
                    if (inFile.is_open()) {
                        char line[256];
                        while (inFile.getline(line, sizeof(line))) {
                            if (myStrLen(line) < 5) continue;
                            char lineCopy[256]; myStrCopy(lineCopy, line);

                            char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                                if (lineCopy[k] == ',') { lineCopy[k] = '\0'; cols[colIdx++] = &lineCopy[k + 1]; }
                            }

                            if (colIdx >= 5) {
                                char temp[256];
                                myStrCopy(temp, "Bill ID: "); myStrCopy(temp + myStrLen(temp), cols[0]);
                                myStrCopy(temp + myStrLen(temp), " | Pat ID: "); myStrCopy(temp + myStrLen(temp), cols[1]);
                                myStrCopy(temp + myStrLen(temp), " | Amt: PKR "); myStrCopy(temp + myStrLen(temp), cols[2]);
                                myStrCopy(temp + myStrLen(temp), " | Status: "); myStrCopy(temp + myStrLen(temp), cols[4]);
                                myStrCopy(temp + myStrLen(temp), "\n");
                                myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                            }
                        }
                        inFile.close();
                    }
                    if (myStrLen(displayBuffer) == 0) myStrCopy(displayBuffer, "No bills in database.");
                    dataViewer.show(displayBuffer);
                }

                else if (i == 6) { // 7. System Summary (Calculates Total Hospital Revenue!)
                    char displayBuffer[4096]; displayBuffer[0] = '\0';

                    int totalDocs = state->doctors.getSize();
                    int totalPats = state->patients.getSize();
                    float totalRevenue = 0.0f;

                    // Calculate revenue from paid bills
                    std::ifstream inFile("bills.txt");
                    if (inFile.is_open()) {
                        char line[256];
                        while (inFile.getline(line, sizeof(line))) {
                            if (myStrLen(line) < 5) continue;
                            char lineCopy[256]; myStrCopy(lineCopy, line);
                            char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                                if (lineCopy[k] == ',') { lineCopy[k] = '\0'; cols[colIdx++] = &lineCopy[k + 1]; }
                            }
                            // If Status is 'P' (Paid), add to revenue!
                            if (colIdx >= 5 && cols[4][0] == 'P') {
                                totalRevenue += myAtof(cols[2]);
                            }
                        }
                        inFile.close();
                    }

                    char temp[256];
                    myStrCopy(displayBuffer, "=== MEDICORE SYSTEM SUMMARY ===\n\n");

                    char docStr[16]; myIntToStr(totalDocs, docStr);
                    myStrCopy(temp, "Total Doctors Registered: "); myStrCopy(temp + myStrLen(temp), docStr); myStrCopy(temp + myStrLen(temp), "\n");
                    myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);

                    char patStr[16]; myIntToStr(totalPats, patStr);
                    myStrCopy(temp, "Total Patients Registered: "); myStrCopy(temp + myStrLen(temp), patStr); myStrCopy(temp + myStrLen(temp), "\n\n");
                    myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);

                    char revStr[32]; myFloatToStr(totalRevenue, revStr);
                    myStrCopy(temp, "Total Hospital Revenue: PKR "); myStrCopy(temp + myStrLen(temp), revStr); myStrCopy(temp + myStrLen(temp), "\n");
                    myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);

                    dataViewer.show(displayBuffer);
                }

                else if (i == 7) {
                    appState.loggedInUserId = -1;
                    appState.currentScreen = ScreenType::Login; // 8. Logout
                }
                });
        }
    }

    void update(float dt, sf::RenderWindow& window) {
        if (dataViewer.isActive()) dataViewer.update(window);
        else if (addDoctorForm.isActive()) addDoctorForm.update(window);
        else if (addPatientForm.isActive()) addPatientForm.update(window);
        else for (int i = 0; i < 8; i++) buttons[i]->update(window);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(*title);
        for (int i = 0; i < 8; i++) buttons[i]->draw(window);

        if (addDoctorForm.isActive()) addDoctorForm.draw(window);
        if (addPatientForm.isActive()) addPatientForm.draw(window);
        if (dataViewer.isActive()) dataViewer.draw(window);
    }
};