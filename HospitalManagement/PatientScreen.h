#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <cstdio>
#include <exception>
#include "AppState.h"
#include "UIComponents.h"
#include "utility.h"
#include "Validator.h"

using namespace sf;

// --- Custom Exceptions ---
class SlotUnavailableException : public std::exception 
{
public:
    const char* what() const noexcept override 
    { 
        return "Error: Time slot is already booked for this date!"; 
    }
};

class InsufficientFundsException : public std::exception
{
public:
    const char* what() const noexcept override 
    { 
        return "Error: Insufficient account balance!"; 
    }
};

// --- Sorting Structures ---
struct ApptRow 
{
    char id[8];
    char docId[8];
    char date[12];
    char slot[16];
    char status[16];
};

struct RecordRow 
{
    char id[8];
    char docId[8];
    char date[12];
    char diag[64];
    char meds[64];
};

class PatientScreen 
{
private:
    AppState* state;
    Text* title;
    Text* subtitle;
    Button* buttons[8];

    InputForm bookApptForm;
    InputForm cancelApptForm;
    InputForm payBillForm;
    InputForm topUpForm;
    DataViewer dataViewer;

public:
    PatientScreen() : state(nullptr), title(nullptr), subtitle(nullptr) 
    {
        for (int i = 0; i < 8; i++) 
        {
            buttons[i] = nullptr;
        }
    }

    ~PatientScreen() 
    {
        delete title; 
        delete subtitle;
        for (int i = 0; i < 8; i++) 
        {
            delete buttons[i];
        }
    }

    void init(const Font& font, AppState& appState) 
    {
        state = &appState;

        title = new Text(font);
        title->setCharacterSize(40);
        title->setFillColor(Color(0, 51, 102));
        title->setPosition({ 50.f, 50.f });

        subtitle = new Text(font);
        subtitle->setCharacterSize(30);
        subtitle->setFillColor(Color(0, 100, 0));
        subtitle->setPosition({ 50.f, 100.f });

        const char* labels[] = 
        {
            "1. Book Appointment", 
            "2. Cancel Appointment", 
            "3. View My Appointments",
            "4. View My Medical Records", 
            "5. View My Bills", 
            "6. Pay Bill",
            "7. Top Up Balance", 
            "8. Logout"
        };

        for (int i = 0; i < 8; i++) 
        {
            Color idle = (i == 7) ? Color(200, 50, 50) : Color(0, 120, 215);
            Color hover = (i == 7) ? Color(150, 0, 0) : Color(0, 80, 160);
            buttons[i] = new Button({ 400.f, 50.f }, { 50.f, 180.f + (i * 60.f) }, labels[i], font, idle, hover);
        }

        // 1d: Updated Booking Form
        const char* apptFields[] = { "Specialization", "Doctor ID", "Date (DD-MM-YYYY)", "Time Slot (e.g. 10:00AM)" };
        bookApptForm.init(font, "Book an Appointment", apptFields, 4);

        const char* cancelFields[] = { "Appointment ID" };
        cancelApptForm.init(font, "Cancel Appointment", cancelFields, 1);

        const char* payFields[] = { "Bill ID" };
        payBillForm.init(font, "Pay Medical Bill", payFields, 1);

        const char* topUpFields[] = { "Amount (PKR)" };
        topUpForm.init(font, "Add Funds to Account", topUpFields, 1);

        dataViewer.init(font, "Patient Dashboard");
    }

    void handleEvent(const Event& event, RenderWindow& window, AppState& appState) 
    {

        if (dataViewer.isActive()) 
        {
            dataViewer.handleEvent(event, window);
            return;
        }

        // --- 1. BOOK APPOINTMENT (With 1d Logic) ---
        if (bookApptForm.isActive()) 
        {
            bookApptForm.handleEvent(event, window, [&](const char** formData) 
            {
                try 
                {
                    const char* reqSpec = formData[0];
                    int targetDocId = Validator::charToInt(formData[1]);
                    const char* dateStr = formData[2];
                    const char* slotStr = formData[3];

                    Patient* p = state->patients.findByID(state->loggedInUserId);
                    Doctor* d = state->doctors.findByID(targetDocId);

                    if (p == nullptr || d == nullptr) 
                        throw std::runtime_error("Error: Invalid Doctor ID.");
                    if (!myStrEqual(d->getSpecialization(), reqSpec)) 
                        throw std::runtime_error("Error: Doctor Specialization mismatch.");
                    if (p->getBalance() < d->getFee()) 
                        throw InsufficientFundsException();

                    // Conflict Check Loop
                    std::ifstream checkFile("appointments.txt");
                    if (checkFile.is_open()) 
                    {
                        char line[256];
                        while (checkFile.getline(line, sizeof(line))) 
                        {
                            if (myStrLen(line) < 5) 
                            {
                                continue;
                            }
                            char lineCopy[256]; myStrCopy(lineCopy, line);
                            char* cols[10]; 
                            int colIdx = 0; 
                            cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) 
                            {
                                if (lineCopy[k] == ',') 
                                { 
                                    lineCopy[k] = '\0'; 
                                    cols[colIdx++] = &lineCopy[k + 1]; 
                                }
                            }
                            if (colIdx >= 6) 
                            {
                                int existDocId = Validator::charToInt(cols[2]);
                                // Check if same doctor, same date, same slot, and not cancelled/completed
                                if (existDocId == targetDocId && myStrEqual(cols[3], dateStr) && myStrEqual(cols[4], slotStr) && cols[5][0] == 'P') 
                                {
                                    checkFile.close();
                                    throw SlotUnavailableException();
                                }
                            }
                        }
                        checkFile.close();
                    }

                    // Success: Deduct balance and write file
                    *p -= d->getFee();
                    int newApptId = 1;
                    std::ifstream inFile("appointments.txt");
                    char line[256];
                    while (inFile.getline(line, sizeof(line))) 
                    {
                        newApptId++;
                    }
                    inFile.close();

                    std::ofstream outFile("appointments.txt", std::ios::app);
                    if (outFile.is_open()) 
                    {
                        // Added the slot field!
                        outFile << "\n" << newApptId << "," << p->getId() << "," << d->getId() << "," << dateStr << "," << slotStr << ",Pending";
                        outFile.close();
                    }
                    dataViewer.show("Appointment Booked Successfully!");
                }
                catch (const std::exception& e) 
                {
                    dataViewer.show(e.what()); // Display the exception message natively!
                }

            });
            return;
        }

        // --- 2. CANCEL APPOINTMENT ---
        if (cancelApptForm.isActive()) 
        {
            cancelApptForm.handleEvent(event, window, [&](const char** formData) 
            {
                int targetAppt = Validator::charToInt(formData[0]);
                int patId = state->loggedInUserId;

                std::ifstream inFile("appointments.txt");
                std::ofstream outFile("temp_appts.txt");

                if (inFile.is_open() && outFile.is_open()) 
                {
                    char line[256];
                    while (inFile.getline(line, sizeof(line))) 
                    {
                        if (myStrLen(line) < 5) 
                        {
                            continue;
                        }
                        char lineCopy[256]; 
                        myStrCopy(lineCopy, line);
                        char* cols[10]; 
                        int colIdx = 0; 
                        cols[colIdx++] = lineCopy;
                        for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) 
                        {
                            if (lineCopy[k] == ',') 
                            { 
                                lineCopy[k] = '\0'; 
                                cols[colIdx++] = &lineCopy[k + 1]; 
                            }
                        }
                        if (colIdx >= 2) 
                        {
                            int aId = Validator::charToInt(cols[0]);
                            int pId = Validator::charToInt(cols[1]);
                            if (aId == targetAppt && pId == patId) 
                            {
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

        // --- 3. PAY BILL ---
        if (payBillForm.isActive()) 
        {
            payBillForm.handleEvent(event, window, [&](const char** formData) 
            {
                int targetBill = Validator::charToInt(formData[0]);
                Patient* p = state->patients.findByID(state->loggedInUserId);

                if (p != nullptr) 
                {
                    std::ifstream inFile("bills.txt");
                    std::ofstream outFile("temp_bills.txt");

                    if (inFile.is_open() && outFile.is_open()) 
                    {
                        char line[256];
                        while (inFile.getline(line, sizeof(line))) 
                        {
                            if (myStrLen(line) < 5) continue;
                            char lineCopy[256]; myStrCopy(lineCopy, line);

                            char* cols[10]; 
                            int colIdx = 0; 
                            cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) 
                            {
                                if (lineCopy[k] == ',') 
                                { 
                                    lineCopy[k] = '\0'; 
                                    cols[colIdx++] = &lineCopy[k + 1]; 
                                }
                            }

                            if (colIdx >= 5) 
                            {
                                int bId = Validator::charToInt(cols[0]);
                                int pId = Validator::charToInt(cols[1]);
                                float amt = myAtof(cols[2]);

                                if (bId == targetBill && pId == p->getId() && cols[4][0] == 'U') 
                                {
                                    if (p->getBalance() >= amt) 
                                    {
                                        *p -= amt;
                                        outFile << cols[0] << "," << cols[1] << "," << cols[2] << "," << cols[3] << ",Paid\n";
                                        continue;
                                    }
                                }
                            }
                            outFile << line << "\n";
                        }
                        inFile.close(); outFile.close();
                        (void)remove("bills.txt");
                        (void)rename("temp_bills.txt", "bills.txt");
                    }
                }
            });
            return;
        }

        // --- 4. TOP UP ---
        if (topUpForm.isActive()) 
        {
            topUpForm.handleEvent(event, window, [&](const char** formData) 
            {
                float amount = myAtof(formData[0]);
                Patient* p = state->patients.findByID(state->loggedInUserId);
                if (p != nullptr) 
                {
                    *p += amount;
                } 
            });
            return;
        }

        // --- BACKGROUND BUTTON CLICKS ---
        for (int i = 0; i < 8; i++) 
        {
            buttons[i]->handleEvent(event, window, [&, i]() 
            {

                if (i == 0) 
                {
                    bookApptForm.show();
                }
                else if (i == 1) 
                {
                    cancelApptForm.show();
                }

                else if (i == 2) 
                {
                    // 1a: View My Appointments with Ascending Sort
                    ApptRow rows[100]; int rowCount = 0;

                    std::ifstream inFile("appointments.txt");
                    if (inFile.is_open()) 
                    {
                        char line[256];
                        while (inFile.getline(line, sizeof(line)) && rowCount < 100) 
                        {
                            if (myStrLen(line) < 5) 
                            {
                                continue;
                            }
                            char lineCopy[256]; 
                            myStrCopy(lineCopy, line);

                            char* cols[10]; 
                            int colIdx = 0; 
                            cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) 
                            {
                                if (lineCopy[k] == ',') 
                                { 
                                    lineCopy[k] = '\0'; 
                                    cols[colIdx++] = &lineCopy[k + 1]; 
                                }
                            }

                            if (colIdx >= 6 && Validator::charToInt(cols[1]) == state->loggedInUserId) 
                            {
                                myStrCopy(rows[rowCount].id, cols[0]);
                                myStrCopy(rows[rowCount].docId, cols[2]);
                                myStrCopy(rows[rowCount].date, cols[3]);
                                myStrCopy(rows[rowCount].slot, cols[4]);
                                myStrCopy(rows[rowCount].status, cols[5]);
                                rowCount++;
                            }
                        }
                        inFile.close();
                    }

                    // Bubble sort by date ascending (DD-MM-YYYY)
                    for (int a = 0; a < rowCount - 1; a++) 
                    {
                        for (int b = 0; b < rowCount - a - 1; b++) 
                        {
                            const char* d1 = rows[b].date;
                            const char* d2 = rows[b + 1].date;
                            int y1 = (d1[6] - '0') * 1000 + (d1[7] - '0') * 100 + (d1[8] - '0') * 10 + (d1[9] - '0');
                            int y2 = (d2[6] - '0') * 1000 + (d2[7] - '0') * 100 + (d2[8] - '0') * 10 + (d2[9] - '0');
                            int m1 = (d1[3] - '0') * 10 + (d1[4] - '0'), m2 = (d2[3] - '0') * 10 + (d2[4] - '0');
                            int dd1 = (d1[0] - '0') * 10 + (d1[1] - '0'), dd2 = (d2[0] - '0') * 10 + (d2[1] - '0');

                            bool swap = y1 > y2 || (y1 == y2 && m1 > m2) || (y1 == y2 && m1 == m2 && dd1 > dd2);
                            if (swap) 
                            { 
                                ApptRow tmp = rows[b]; 
                                rows[b] = rows[b + 1]; 
                                rows[b + 1] = tmp; 
                            }
                        }
                    }

                    char displayBuffer[8192]; displayBuffer[0] = '\0';
                    for (int j = 0; j < rowCount; j++) 
                    {
                        char temp[256];
                        myStrCopy(temp, "Appt ID: "); myStrCopy(temp + myStrLen(temp), rows[j].id);
                        myStrCopy(temp + myStrLen(temp), " | Dr. ID: "); myStrCopy(temp + myStrLen(temp), rows[j].docId);
                        myStrCopy(temp + myStrLen(temp), " | Date: "); myStrCopy(temp + myStrLen(temp), rows[j].date);
                        myStrCopy(temp + myStrLen(temp), " | Slot: "); myStrCopy(temp + myStrLen(temp), rows[j].slot);
                        myStrCopy(temp + myStrLen(temp), " | Status: "); myStrCopy(temp + myStrLen(temp), rows[j].status);
                        myStrCopy(temp + myStrLen(temp), "\n");
                        myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                    }

                    if (myStrLen(displayBuffer) == 0) 
                    {
                        myStrCopy(displayBuffer, "No appointments found.");
                    }
                    dataViewer.show(displayBuffer);
                }

                else if (i == 3) 
                {
                    // 1b: View Medical Records with Descending Sort
                    RecordRow rows[100]; 
                    int rowCount = 0;
                    std::ifstream inFile("prescriptions.txt");
                    if (inFile.is_open()) 
                    {
                        char line[256];
                        while (inFile.getline(line, sizeof(line)) && rowCount < 100) 
                        {
                            if (myStrLen(line) < 5) 
                            {
                                continue;
                            }
                            char lineCopy[256]; myStrCopy(lineCopy, line);

                            char* cols[10]; int colIdx = 0; cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) 
                            {
                                if (lineCopy[k] == ',') 
                                { 
                                    lineCopy[k] = '\0'; 
                                    cols[colIdx++] = &lineCopy[k + 1]; 
                                }
                            }

                            if (colIdx >= 6 && Validator::charToInt(cols[1]) == state->loggedInUserId) 
                            {
                                myStrCopy(rows[rowCount].id, cols[0]);
                                myStrCopy(rows[rowCount].docId, cols[2]);
                                myStrCopy(rows[rowCount].date, cols[3]);
                                myStrCopy(rows[rowCount].diag, cols[4]);
                                myStrCopy(rows[rowCount].meds, cols[5]);
                                rowCount++;
                            }
                        }
                        inFile.close();
                    }

                    // Bubble sort by date descending (Flipped swap condition)
                    for (int a = 0; a < rowCount - 1; a++) 
                    {
                        for (int b = 0; b < rowCount - a - 1; b++) 
                        {
                            const char* d1 = rows[b].date;
                            const char* d2 = rows[b + 1].date;
                            int y1 = (d1[6] - '0') * 1000 + (d1[7] - '0') * 100 + (d1[8] - '0') * 10 + (d1[9] - '0');
                            int y2 = (d2[6] - '0') * 1000 + (d2[7] - '0') * 100 + (d2[8] - '0') * 10 + (d2[9] - '0');
                            int m1 = (d1[3] - '0') * 10 + (d1[4] - '0'), m2 = (d2[3] - '0') * 10 + (d2[4] - '0');
                            int dd1 = (d1[0] - '0') * 10 + (d1[1] - '0'), dd2 = (d2[0] - '0') * 10 + (d2[1] - '0');

                            bool swap = y1 < y2 || (y1 == y2 && m1 < m2) || (y1 == y2 && m1 == m2 && dd1 < dd2);
                            if (swap) 
                            { 
                                RecordRow tmp = rows[b]; 
                                rows[b] = rows[b + 1]; 
                                rows[b + 1] = tmp; 
                            }
                        }
                    }

                    char displayBuffer[8192]; displayBuffer[0] = '\0';
                    for (int j = 0; j < rowCount; j++) 
                    {
                        char temp[256];
                        myStrCopy(temp, "Rec ID: "); myStrCopy(temp + myStrLen(temp), rows[j].id);
                        myStrCopy(temp + myStrLen(temp), " | Date: "); myStrCopy(temp + myStrLen(temp), rows[j].date);
                        myStrCopy(temp + myStrLen(temp), " | Diag: "); myStrCopy(temp + myStrLen(temp), rows[j].diag);
                        myStrCopy(temp + myStrLen(temp), " | Meds: "); myStrCopy(temp + myStrLen(temp), rows[j].meds);
                        myStrCopy(temp + myStrLen(temp), "\n");
                        myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                    }

                    if (myStrLen(displayBuffer) == 0) 
                    {
                        myStrCopy(displayBuffer, "No medical records found.");
                    }
                    dataViewer.show(displayBuffer);
                }

                else if (i == 4) 
                {
                    // 1c: View Bills with Total Outstanding calculation
                    char displayBuffer[8192]; displayBuffer[0] = '\0';
                    float totalUnpaid = 0.0f;

                    std::ifstream inFile("bills.txt");
                    if (inFile.is_open()) 
{
                        char line[256];
                        while (inFile.getline(line, sizeof(line))) 
                        {
                            if (myStrLen(line) < 5) 
                            {
                                continue;
                            }
                            char lineCopy[256]; 
                            myStrCopy(lineCopy, line);

                            char* cols[10]; 
                            int colIdx = 0; 
                            cols[colIdx++] = lineCopy;
                            for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) 
                            {
                                if (lineCopy[k] == ',') 
                                { 
                                    lineCopy[k] = '\0'; 
                                    cols[colIdx++] = &lineCopy[k + 1]; 
                                }
                            }

                            if (colIdx >= 5 && Validator::charToInt(cols[1]) == state->loggedInUserId) 
                            {
                                char temp[256];
                                myStrCopy(temp, "Bill ID: "); myStrCopy(temp + myStrLen(temp), cols[0]);
                                myStrCopy(temp + myStrLen(temp), " | Amount: PKR "); myStrCopy(temp + myStrLen(temp), cols[2]);
                                myStrCopy(temp + myStrLen(temp), " | Reason: "); myStrCopy(temp + myStrLen(temp), cols[3]);
                                myStrCopy(temp + myStrLen(temp), " | Status: "); myStrCopy(temp + myStrLen(temp), cols[4]);
                                myStrCopy(temp + myStrLen(temp), "\n");
                                myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);

                                // Add to total if status is 'U' (Unpaid)
                                if (cols[4][0] == 'U') 
                                {
                                    totalUnpaid += myAtof(cols[2]);
                                }
                            }
                        }
                        inFile.close();
                    }

                    if (myStrLen(displayBuffer) == 0) 
                    {
                        myStrCopy(displayBuffer, "No bills found.");
                    }
                    else 
                    {
                        // Append the total outstanding sum
                        char unpaidStr[128];
                        char unpaidVal[32];
                        myFloatToStr(totalUnpaid, unpaidVal);
                        myStrCopy(unpaidStr, "\n------------------------------\nTotal Outstanding: PKR ");
                        myStrCopy(unpaidStr + myStrLen(unpaidStr), unpaidVal);
                        myStrCopy(displayBuffer + myStrLen(displayBuffer), unpaidStr);
                    }

                    dataViewer.show(displayBuffer);
                }

                else if (i == 5) 
                {
                    payBillForm.show();
                } 
                else if (i == 6) 
                {
                    topUpForm.show();
                }

                else if (i == 7) 
                {
                    appState.loggedInUserId = -1;
                    appState.currentScreen = ScreenType::Login;
                }
            });
        }
    }

    void update(float dt, RenderWindow& window) 
    {
        if (state && state->loggedInUserId != -1 && state->currentScreen == ScreenType::Patient) 
        {
            Patient* p = state->patients.findByID(state->loggedInUserId);
            if (p != nullptr) 
            {
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

        if (dataViewer.isActive()) 
        {
            dataViewer.update(window);
        }
        else if (bookApptForm.isActive())
        {
            bookApptForm.update(window);
        } 
        else if (cancelApptForm.isActive())
        {
            cancelApptForm.update(window);
        }
        else if (payBillForm.isActive())
        {
            payBillForm.update(window);
        }
        else if (topUpForm.isActive())
        {
            topUpForm.update(window);
        }
        else for (int i = 0; i < 8; i++)
        {
            buttons[i]->update(window);
        }
    }

    void draw( RenderWindow& window) 
    {
        window.draw(*title);
        window.draw(*subtitle);
        for (int i = 0; i < 8; i++)
        {
            buttons[i]->draw(window);
        }
        if (bookApptForm.isActive())
        {
            bookApptForm.draw(window);
        }
        if (cancelApptForm.isActive())
        {
            cancelApptForm.draw(window);
        }
        if (payBillForm.isActive())
        {
            payBillForm.draw(window);
        }
        if (topUpForm.isActive())
        {
            topUpForm.draw(window);
        }
        if (dataViewer.isActive())
        {
            dataViewer.draw(window);
        } 
    }
};