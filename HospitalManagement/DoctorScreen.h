#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cstdio>
#include "AppState.h"
#include "UIComponents.h"
#include "utility.h"
#include "Validator.h"

class DoctorScreen {
private:
    AppState* state;
    sf::Text* title;
    sf::Text* subtitle;
    Button* buttons[8];

    InputForm attendPatientForm;
    DataViewer dataViewer;

public:
    DoctorScreen() : state(nullptr), title(nullptr), subtitle(nullptr) {
        for (int i = 0; i < 8; i++) buttons[i] = nullptr;
    }

    ~DoctorScreen() {
        delete title; delete subtitle;
        for (int i = 0; i < 8; i++) delete buttons[i];
    }

    void init(const sf::Font& font, AppState& appState) {
        state = &appState;

        title = new sf::Text(font);
        title->setCharacterSize(40);
        title->setFillColor(sf::Color(0, 51, 102));
        title->setPosition({ 50.f, 50.f });

        subtitle = new sf::Text(font);
        subtitle->setCharacterSize(30);
        subtitle->setFillColor(sf::Color(0, 100, 0));
        subtitle->setPosition({ 50.f, 100.f });

        const char* labels[] = {
            "1. View Pending Appts", "2. Attend Patient", "3. View Completed Appts",
            "4. View Prescriptions", "5. My Earnings", "6. ---",
            "7. ---", "8. Logout"
        };

        for (int i = 0; i < 8; i++) {
            sf::Color idle = (i == 7) ? sf::Color(200, 50, 50) : sf::Color(0, 120, 215);
            sf::Color hover = (i == 7) ? sf::Color(150, 0, 0) : sf::Color(0, 80, 160);
            buttons[i] = new Button({ 400.f, 50.f }, { 50.f, 180.f + (i * 60.f) }, labels[i], font, idle, hover);
        }

        const char* attendFields[] = { "Appt ID", "Diagnosis", "Medicines", "Generate Bill? (Y/N)" };
        attendPatientForm.init(font, "Attend Patient & Prescribe", attendFields, 4);

        dataViewer.init(font, "Doctor Dashboard");
    }

    void handleEvent(const sf::Event& event, sf::RenderWindow& window, AppState& appState) {

        if (dataViewer.isActive()) {
            dataViewer.handleEvent(event, window);
            return;
        }

        // --- 1. ATTEND PATIENT ---
        if (attendPatientForm.isActive()) {
            attendPatientForm.handleEvent(event, window, [&](const char** formData) {
                int targetAppt = Validator::charToInt(formData[0]);
                const char* diagnosis = formData[1];
                const char* meds = formData[2];
                bool generateBill = (formData[3][0] == 'Y' || formData[3][0] == 'y');

                Doctor* d = state->doctors.findByID(state->loggedInUserId);
                if (d == nullptr) return;

                std::ifstream inFile("appointments.txt");
                std::ofstream outFile("temp_appts.txt");

                if (inFile.is_open() && outFile.is_open()) {
                    char line[256];
                    while (inFile.getline(line, sizeof(line))) {
                        if (myStrLen(line) < 5) continue;
                        char lineCopy[256]; myStrCopy(lineCopy, line);

                        char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                        for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                            if (lineCopy[k] == ',') { lineCopy[k] = '\0'; cols[colIdx++] = &lineCopy[k + 1]; }
                        }

                        if (colIdx >= 5) {
                            int aId = Validator::charToInt(cols[0]);
                            int pId = Validator::charToInt(cols[1]);
                            int docId = Validator::charToInt(cols[2]);

                            // If this is the correct appointment, and it's currently Pending
                            if (aId == targetAppt && docId == d->getId() && cols[4][0] == 'P') {

                                // Write Prescription
                                int newPrescId = 1;
                                std::ifstream pIn("prescriptions.txt");
                                char tempLine[256];
                                while (pIn.getline(tempLine, sizeof(tempLine))) newPrescId++;
                                pIn.close();

                                std::ofstream pOut("prescriptions.txt", std::ios::app);
                                if (pOut.is_open()) {
                                    pOut << "\n" << newPrescId << "," << pId << "," << docId << "," << cols[3] << "," << diagnosis << "," << meds;
                                    pOut.close();
                                }

                                // Generate Bill
                                if (generateBill) {
                                    int newBillId = 1;
                                    std::ifstream bIn("bills.txt");
                                    while (bIn.getline(tempLine, sizeof(tempLine))) newBillId++;
                                    bIn.close();

                                    std::ofstream bOut("bills.txt", std::ios::app);
                                    if (bOut.is_open()) {
                                        bOut << "\n" << newBillId << "," << pId << "," << d->getFee() << ",Consultation Fee,Unpaid";
                                        bOut.close();
                                    }
                                }

                                // Mark Appointment Completed
                                outFile << cols[0] << "," << cols[1] << "," << cols[2] << "," << cols[3] << ",Completed\n";
                                continue;
                            }
                        }
                        outFile << line << "\n";
                    }
                    inFile.close(); outFile.close();

                    (void)remove("appointments.txt");
                    (void)rename("temp_appts.txt", "appointments.txt");
                }
                });
            return;
        }

        // --- BACKGROUND BUTTON CLICKS ---
        for (int i = 0; i < 8; i++) {
            buttons[i]->handleEvent(event, window, [&, i]() {

                if (i == 0 || i == 2) {
                    char displayBuffer[8192]; displayBuffer[0] = '\0';
                    char targetStatus = (i == 0) ? 'P' : 'C';

                    std::ifstream inFile("appointments.txt");
                    if (inFile.is_open()) {
                        char line[256];
                        while (inFile.getline(line, sizeof(line))) {
                            if (myStrLen(line) < 5) continue;
                            char lineCopy[256]; myStrCopy(lineCopy, line);

                            char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                                if (lineCopy[k] == ',') { lineCopy[k] = '\0'; cols[colIdx++] = &lineCopy[k + 1]; }
                            }

                            if (colIdx >= 5 && Validator::charToInt(cols[2]) == state->loggedInUserId && cols[4][0] == targetStatus) {
                                char temp[256];
                                myStrCopy(temp, "Appt ID: "); myStrCopy(temp + myStrLen(temp), cols[0]);
                                myStrCopy(temp + myStrLen(temp), " | Pat ID: "); myStrCopy(temp + myStrLen(temp), cols[1]);
                                myStrCopy(temp + myStrLen(temp), " | Date: "); myStrCopy(temp + myStrLen(temp), cols[3]);
                                myStrCopy(temp + myStrLen(temp), " | Status: "); myStrCopy(temp + myStrLen(temp), cols[4]);
                                myStrCopy(temp + myStrLen(temp), "\n");
                                myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                            }
                        }
                        inFile.close();
                    }
                    if (myStrLen(displayBuffer) == 0) myStrCopy(displayBuffer, "No appointments found.");
                    dataViewer.show(displayBuffer);
                }

                else if (i == 1) attendPatientForm.show();

                else if (i == 3) {
                    char displayBuffer[8192]; displayBuffer[0] = '\0';
                    std::ifstream inFile("prescriptions.txt");
                    if (inFile.is_open()) {
                        char line[256];
                        while (inFile.getline(line, sizeof(line))) {
                            if (myStrLen(line) < 5) continue;
                            char lineCopy[256]; myStrCopy(lineCopy, line);

                            char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                                if (lineCopy[k] == ',') { lineCopy[k] = '\0'; cols[colIdx++] = &lineCopy[k + 1]; }
                            }

                            if (colIdx >= 6 && Validator::charToInt(cols[2]) == state->loggedInUserId) {
                                char temp[256];
                                myStrCopy(temp, "Rec ID: "); myStrCopy(temp + myStrLen(temp), cols[0]);
                                myStrCopy(temp + myStrLen(temp), " | Pat ID: "); myStrCopy(temp + myStrLen(temp), cols[1]);
                                myStrCopy(temp + myStrLen(temp), " | Diag: "); myStrCopy(temp + myStrLen(temp), cols[4]);
                                myStrCopy(temp + myStrLen(temp), " | Meds: "); myStrCopy(temp + myStrLen(temp), cols[5]);
                                myStrCopy(temp + myStrLen(temp), "\n");
                                myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                            }
                        }
                        inFile.close();
                    }
                    if (myStrLen(displayBuffer) == 0) myStrCopy(displayBuffer, "No prescriptions found.");
                    dataViewer.show(displayBuffer);
                }

                else if (i == 4) {
                    int completedCount = 0;
                    Doctor* d = state->doctors.findByID(state->loggedInUserId);
                    if (d != nullptr) {
                        std::ifstream inFile("appointments.txt");
                        if (inFile.is_open()) {
                            char line[256];
                            while (inFile.getline(line, sizeof(line))) {
                                if (myStrLen(line) < 5) continue;
                                char lineCopy[256]; myStrCopy(lineCopy, line);

                                char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                                for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                                    if (lineCopy[k] == ',') { lineCopy[k] = '\0'; cols[colIdx++] = &lineCopy[k + 1]; }
                                }
                                if (colIdx >= 5 && Validator::charToInt(cols[2]) == d->getId() && cols[4][0] == 'C') {
                                    completedCount++;
                                }
                            }
                            inFile.close();
                        }

                        float earnings = completedCount * d->getFee();

                        char displayBuffer[512]; char countStr[16]; char earnStr[32];
                        myIntToStr(completedCount, countStr);
                        myFloatToStr(earnings, earnStr);

                        myStrCopy(displayBuffer, "Total Patients Treated: ");
                        myStrCopy(displayBuffer + myStrLen(displayBuffer), countStr);
                        myStrCopy(displayBuffer + myStrLen(displayBuffer), "\n\nTotal Estimated Earnings: PKR ");
                        myStrCopy(displayBuffer + myStrLen(displayBuffer), earnStr);

                        dataViewer.show(displayBuffer);
                    }
                }

                else if (i == 7) {
                    appState.loggedInUserId = -1;
                    appState.currentScreen = ScreenType::Login;
                }
                });
        }
    }

    void update(float dt, sf::RenderWindow& window) {
        if (state && state->loggedInUserId != -1 && state->currentScreen == ScreenType::Doctor) {
            Doctor* d = state->doctors.findByID(state->loggedInUserId);
            if (d != nullptr) {
                char welcomeMsg[150];
                myStrCopy(welcomeMsg, "Welcome, Dr. ");
                myStrCopy(welcomeMsg + myStrLen(welcomeMsg), d->getName());
                title->setString(welcomeMsg);

                char specStr[100];
                myStrCopy(specStr, "Department: ");
                myStrCopy(specStr + myStrLen(specStr), d->getSpecialization());
                subtitle->setString(specStr);
            }
        }

        if (dataViewer.isActive()) dataViewer.update(window);
        else if (attendPatientForm.isActive()) attendPatientForm.update(window);
        else for (int i = 0; i < 8; i++) buttons[i]->update(window);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(*title);
        window.draw(*subtitle);
        for (int i = 0; i < 8; i++) buttons[i]->draw(window);

        if (attendPatientForm.isActive()) attendPatientForm.draw(window);
        if (dataViewer.isActive()) dataViewer.draw(window);
    }
};