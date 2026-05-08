#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cstdio>
#include "AppState.h"
#include "UIComponents.h"
#include "utility.h"
#include "Validator.h"

class PatientScreen {
private:
    AppState* state;
    sf::Text* title;
    sf::Text* subtitle;
    Button* buttons[8];

    InputForm bookApptForm;
    InputForm cancelApptForm;
    InputForm payBillForm;
    InputForm topUpForm;
    DataViewer dataViewer;

public:
    PatientScreen() : state(nullptr), title(nullptr), subtitle(nullptr) {
        for (int i = 0; i < 8; i++) buttons[i] = nullptr;
    }

    ~PatientScreen() {
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
            "1. Book Appointment", "2. Cancel Appointment", "3. View My Appointments",
            "4. View My Medical Records", "5. View My Bills", "6. Pay Bill",
            "7. Top Up Balance", "8. Logout"
        };

        for (int i = 0; i < 8; i++) {
            sf::Color idle = (i == 7) ? sf::Color(200, 50, 50) : sf::Color(0, 120, 215);
            sf::Color hover = (i == 7) ? sf::Color(150, 0, 0) : sf::Color(0, 80, 160);
            buttons[i] = new Button({ 400.f, 50.f }, { 50.f, 180.f + (i * 60.f) }, labels[i], font, idle, hover);
        }

        const char* apptFields[] = { "Doctor ID", "Date (DD-MM-YYYY)" };
        bookApptForm.init(font, "Book an Appointment", apptFields, 2);

        const char* cancelFields[] = { "Appointment ID" };
        cancelApptForm.init(font, "Cancel Appointment", cancelFields, 1);

        const char* payFields[] = { "Bill ID" };
        payBillForm.init(font, "Pay Medical Bill", payFields, 1);

        const char* topUpFields[] = { "Amount (PKR)" };
        topUpForm.init(font, "Add Funds to Account", topUpFields, 1);

        dataViewer.init(font, "Patient Dashboard");
    }

    void handleEvent(const sf::Event& event, sf::RenderWindow& window, AppState& appState) {

        if (dataViewer.isActive()) {
            dataViewer.handleEvent(event, window);
            return;
        }

        if (bookApptForm.isActive()) {
            bookApptForm.handleEvent(event, window, [&](const char** formData) {
                int targetDocId = Validator::charToInt(formData[0]);
                const char* dateStr = formData[1];

                Patient* p = state->patients.findByID(state->loggedInUserId);
                Doctor* d = state->doctors.findByID(targetDocId);

                if (p != nullptr && d != nullptr && p->getBalance() >= d->getFee()) {
                    *p -= d->getFee();
                    int newApptId = 1;
                    std::ifstream inFile("appointments.txt");
                    char line[256];
                    while (inFile.getline(line, sizeof(line))) newApptId++;
                    inFile.close();

                    std::ofstream outFile("appointments.txt", std::ios::app);
                    if (outFile.is_open()) {
                        outFile << "\n" << newApptId << "," << p->getId() << ","
                            << d->getId() << "," << dateStr << ",Pending";
                        outFile.close();
                    }
                }
                });
            return;
        }

        if (cancelApptForm.isActive()) {
            cancelApptForm.handleEvent(event, window, [&](const char** formData) {
                int targetAppt = Validator::charToInt(formData[0]);
                int patId = state->loggedInUserId;

                std::ifstream inFile("appointments.txt");
                std::ofstream outFile("temp_appts.txt");

                if (inFile.is_open() && outFile.is_open()) {
                    char line[256];
                    while (inFile.getline(line, sizeof(line))) {
                        if (myStrLen(line) < 5) continue;

                        char lineCopy[256];
                        myStrCopy(lineCopy, line);

                        // MANUAL C-STYLE PARSER
                        char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                        for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                            if (lineCopy[k] == ',') {
                                lineCopy[k] = '\0';
                                cols[colIdx++] = &lineCopy[k + 1];
                            }
                        }

                        if (colIdx >= 2) {
                            int aId = Validator::charToInt(cols[0]);
                            int pId = Validator::charToInt(cols[1]);
                            if (aId == targetAppt && pId == patId) continue;
                        }
                        outFile << line << "\n";
                    }
                    inFile.close();
                    outFile.close();
                    remove("appointments.txt");
                    rename("temp_appts.txt", "appointments.txt");
                }
                });
            return;
        }

        if (payBillForm.isActive()) {
            payBillForm.handleEvent(event, window, [&](const char** formData) {
                int targetBill = Validator::charToInt(formData[0]);
                Patient* p = state->patients.findByID(state->loggedInUserId);

                if (p != nullptr) {
                    std::ifstream inFile("bills.txt");
                    std::ofstream outFile("temp_bills.txt");

                    if (inFile.is_open() && outFile.is_open()) {
                        char line[256];
                        while (inFile.getline(line, sizeof(line))) {
                            if (myStrLen(line) < 5) continue;

                            char lineCopy[256];
                            myStrCopy(lineCopy, line);

                            // MANUAL C-STYLE PARSER
                            char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                                if (lineCopy[k] == ',') {
                                    lineCopy[k] = '\0';
                                    cols[colIdx++] = &lineCopy[k + 1];
                                }
                            }

                            if (colIdx >= 5) {
                                int bId = Validator::charToInt(cols[0]);
                                int pId = Validator::charToInt(cols[1]);
                                float amt = myAtof(cols[2]);

                                if (bId == targetBill && pId == p->getId() && cols[4][0] == 'U') {
                                    if (p->getBalance() >= amt) {
                                        *p -= amt;
                                        outFile << cols[0] << "," << cols[1] << "," << cols[2] << "," << cols[3] << ",Paid\n";
                                        continue;
                                    }
                                }
                            }
                            outFile << line << "\n";
                        }
                        inFile.close();
                        outFile.close();
                        remove("bills.txt");
                        rename("temp_bills.txt", "bills.txt");
                    }
                }
                });
            return;
        }

        if (topUpForm.isActive()) {
            topUpForm.handleEvent(event, window, [&](const char** formData) {
                float amount = myAtof(formData[0]);
                Patient* p = state->patients.findByID(state->loggedInUserId);
                if (p != nullptr) *p += amount;
                });
            return;
        }

        for (int i = 0; i < 8; i++) {
            buttons[i]->handleEvent(event, window, [&, i]() {

                if (i == 0) bookApptForm.show();
                else if (i == 1) cancelApptForm.show();

                else if (i == 2) {
                    char displayBuffer[4096]; displayBuffer[0] = '\0';
                    std::ifstream inFile("appointments.txt");
                    if (inFile.is_open()) {
                        char line[256];
                        while (inFile.getline(line, sizeof(line))) {
                            if (myStrLen(line) < 5) continue;
                            char lineCopy[256]; myStrCopy(lineCopy, line);

                            // MANUAL C-STYLE PARSER
                            char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                                if (lineCopy[k] == ',') {
                                    lineCopy[k] = '\0';
                                    cols[colIdx++] = &lineCopy[k + 1];
                                }
                            }

                            if (colIdx >= 5 && Validator::charToInt(cols[1]) == state->loggedInUserId) {
                                char temp[256];
                                myStrCopy(temp, "Appt ID: "); myStrCopy(temp + myStrLen(temp), cols[0]);
                                myStrCopy(temp + myStrLen(temp), " | Dr. ID: "); myStrCopy(temp + myStrLen(temp), cols[2]);
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

                else if (i == 3) {
                    char displayBuffer[4096]; displayBuffer[0] = '\0';
                    std::ifstream inFile("prescriptions.txt");
                    if (inFile.is_open()) {
                        char line[256];
                        while (inFile.getline(line, sizeof(line))) {
                            if (myStrLen(line) < 5) continue;
                            char lineCopy[256]; myStrCopy(lineCopy, line);

                            // MANUAL C-STYLE PARSER
                            char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                                if (lineCopy[k] == ',') {
                                    lineCopy[k] = '\0';
                                    cols[colIdx++] = &lineCopy[k + 1];
                                }
                            }

                            if (colIdx >= 6 && Validator::charToInt(cols[1]) == state->loggedInUserId) {
                                char temp[256];
                                myStrCopy(temp, "Record ID: "); myStrCopy(temp + myStrLen(temp), cols[0]);
                                myStrCopy(temp + myStrLen(temp), " | Date: "); myStrCopy(temp + myStrLen(temp), cols[3]);
                                myStrCopy(temp + myStrLen(temp), " | Diagnosis: "); myStrCopy(temp + myStrLen(temp), cols[4]);
                                myStrCopy(temp + myStrLen(temp), " | Meds: "); myStrCopy(temp + myStrLen(temp), cols[5]);
                                myStrCopy(temp + myStrLen(temp), "\n");
                                myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                            }
                        }
                        inFile.close();
                    }
                    if (myStrLen(displayBuffer) == 0) myStrCopy(displayBuffer, "No medical records found.");
                    dataViewer.show(displayBuffer);
                }

                else if (i == 4) {
                    char displayBuffer[4096]; displayBuffer[0] = '\0';
                    std::ifstream inFile("bills.txt");
                    if (inFile.is_open()) {
                        char line[256];
                        while (inFile.getline(line, sizeof(line))) {
                            if (myStrLen(line) < 5) continue;
                            char lineCopy[256]; myStrCopy(lineCopy, line);

                            // MANUAL C-STYLE PARSER
                            char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                                if (lineCopy[k] == ',') {
                                    lineCopy[k] = '\0';
                                    cols[colIdx++] = &lineCopy[k + 1];
                                }
                            }

                            if (colIdx >= 5 && Validator::charToInt(cols[1]) == state->loggedInUserId) {
                                char temp[256];
                                myStrCopy(temp, "Bill ID: "); myStrCopy(temp + myStrLen(temp), cols[0]);
                                myStrCopy(temp + myStrLen(temp), " | Amount: PKR "); myStrCopy(temp + myStrLen(temp), cols[2]);
                                myStrCopy(temp + myStrLen(temp), " | Reason: "); myStrCopy(temp + myStrLen(temp), cols[3]);
                                myStrCopy(temp + myStrLen(temp), " | Status: "); myStrCopy(temp + myStrLen(temp), cols[4]);
                                myStrCopy(temp + myStrLen(temp), "\n");
                                myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                            }
                        }
                        inFile.close();
                    }
                    if (myStrLen(displayBuffer) == 0) myStrCopy(displayBuffer, "No bills found.");
                    dataViewer.show(displayBuffer);
                }

                else if (i == 5) payBillForm.show();
                else if (i == 6) topUpForm.show();

                else if (i == 7) {
                    appState.loggedInUserId = -1;
                    appState.currentScreen = ScreenType::Login;
                }
                });
        }
    }

    void update(float dt, sf::RenderWindow& window) {
        if (state && state->loggedInUserId != -1 && state->currentScreen == ScreenType::Patient) {
            Patient* p = state->patients.findByID(state->loggedInUserId);
            if (p != nullptr) {
                char welcomeMsg[150];
                myStrCopy(welcomeMsg, "Welcome, ");
                myStrCopy(welcomeMsg + myStrLen(welcomeMsg), p->getName());
                title->setString(welcomeMsg);

                char balStr[50];
                myStrCopy(balStr, "Balance: PKR ");
                char balNum[32];
                myFloatToStr(p->getBalance(), balNum);
                myStrCopy(balStr + myStrLen(balStr), balNum);
                subtitle->setString(balStr);
            }
        }

        if (dataViewer.isActive()) dataViewer.update(window);
        else if (bookApptForm.isActive()) bookApptForm.update(window);
        else if (cancelApptForm.isActive()) cancelApptForm.update(window);
        else if (payBillForm.isActive()) payBillForm.update(window);
        else if (topUpForm.isActive()) topUpForm.update(window);
        else for (int i = 0; i < 8; i++) buttons[i]->update(window);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(*title);
        window.draw(*subtitle);
        for (int i = 0; i < 8; i++) buttons[i]->draw(window);

        if (bookApptForm.isActive()) bookApptForm.draw(window);
        if (cancelApptForm.isActive()) cancelApptForm.draw(window);
        if (payBillForm.isActive()) payBillForm.draw(window);
        if (topUpForm.isActive()) topUpForm.draw(window);
        if (dataViewer.isActive()) dataViewer.draw(window);
    }
};