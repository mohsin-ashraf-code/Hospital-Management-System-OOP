#include "PatientScreen.h"
#include "utility.h"
#include "Validator.h"
#include "FileHandler.h"
#include "InvalidInputException.h"
#include "InsufficientFundsException.h"
#include "SlotUnavailableException.h"
#include <fstream>
#include <cstdio>

// --- Sorting Structures (Hidden inside the .cpp for encapsulation) ---
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

// Constructor
PatientScreen::PatientScreen() : state(nullptr), title(nullptr), subtitle(nullptr), buttonCount(8) 
{
    buttons = new Button * [buttonCount];
    for (int i = 0; i < buttonCount; i++) 
    {
        buttons[i] = nullptr;
    }
    currentSearchSpec = STRING("");
}

// Init
void PatientScreen::init(const Font& font, AppState& appState) 
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

    for (int i = 0; i < buttonCount; i++) 
    {
        Color idle = (i == 7) ? Color(200, 50, 50) : Color(0, 120, 215);
        Color hover = (i == 7) ? Color(150, 0, 0) : Color(0, 80, 160);
        buttons[i] = new Button({ 400.f, 50.f }, { 50.f, 180.f + (i * 60.f) }, labels[i], font, idle, hover);
    }

    // Step 1 Form: Search Only
    const char* specFields[] = { "Enter Specialization to Search" };
    bookApptForm.init(font, "Step 1: Search Doctors", specFields, 1);

    // Step 2 Form: Finalize Booking
    const char* apptFields[] = { "Doctor ID", "Date (DD-MM-YYYY)", "Time Slot (e.g. 10:00AM)" };
    finalizeApptForm.init(font, "Step 2: Book Appointment", apptFields, 3);

    const char* cancelFields[] = { "Appointment ID" };
    cancelApptForm.init(font, "Cancel Appointment", cancelFields, 1);

    const char* payFields[] = { "Bill ID" };
    payBillForm.init(font, "Pay Medical Bill", payFields, 1);

    const char* topUpFields[] = { "Amount (PKR)" };
    topUpForm.init(font, "Add Funds to Account", topUpFields, 1);

    dataViewer.init(font, "Patient Dashboard");
}

// Handle Event
void PatientScreen::handleEvent(const Event& event, RenderWindow& window, AppState& appState) 
{
    if (dataViewer.isActive()) 
    {
        dataViewer.handleEvent(event, window);
        return;
    }

    // --- 1. BOOK APPOINTMENT ---
// --- 1. BOOK APPOINTMENT (STEP 1: SEARCH) ---
    if (bookApptForm.isActive()) {
        bookApptForm.handleEvent(event, window, [&](const char** formData) {
            const char* reqSpec = formData[0];
            currentSearchSpec = STRING(reqSpec); // Save it for Step 2

            char displayBuffer[8192];
            displayBuffer[0] = '\0';
            int matchCount = 0;

            for (int i = 0; i < state->doctors.getSize(); i++) {
                Doctor* d = state->doctors.getAt(i);
                if (d != nullptr) {
                    // Using your custom case-insensitive function!
                    if (myCaseInsensitiveEqual(d->getSpecialization(), reqSpec)) {
                        char temp[256];
                        char feeStr[32];
                        char idStr[16];

                        myFloatToStr(d->getFee(), feeStr);
                        myIntToStr(d->getId(), idStr);

                        myStrCopy(temp, "Doc ID: ");
                        myStrCopy(temp + myStrLen(temp), idStr);
                        myStrCopy(temp + myStrLen(temp), " | Name: Dr. ");
                        myStrCopy(temp + myStrLen(temp), d->getName());
                        myStrCopy(temp + myStrLen(temp), " | Fee: PKR ");
                        myStrCopy(temp + myStrLen(temp), feeStr);
                        myStrCopy(temp + myStrLen(temp), "\n");

                        myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                        matchCount++;
                    }
                }
            }

            if (matchCount == 0) {
                // Rubric requirement: Print this exact message and return to menu
                dataViewer.show("No doctors available for that specialization.");
            }
            else {
                // Show the list of doctors, AND trigger Step 2!
                dataViewer.show(displayBuffer);
                finalizeApptForm.show();
            }
            });
        return;
    }

    // --- 1. BOOK APPOINTMENT (STEP 2: FINALIZE) ---
    if (finalizeApptForm.isActive()) {
        finalizeApptForm.handleEvent(event, window, [&](const char** formData) {
            try {
                int targetDocId = Validator::charToInt(formData[0]);
                const char* dateStr = formData[1];
                const char* slotStr = formData[2];

                if (!Validator::isValidDate(dateStr)) 
                {
                    throw InvalidInputException("Error: Invalid Date format! Use DD-MM-YYYY.");
                }
                if (!Validator::isValidTimeSlot(slotStr)) 
                {
                    throw InvalidInputException("Error: Invalid Time format! Use e.g., 10:00AM.");
                }

                Patient* p = state->patients.findByID(state->loggedInUserId);
                Doctor* d = state->doctors.findByID(targetDocId);

                if (p == nullptr || d == nullptr) 
                {
                    throw InvalidInputException("Error: Invalid Doctor ID.");
                }

                // Double check they didn't type an ID for a doctor in a different department
                if (!myCaseInsensitiveEqual(d->getSpecialization(), currentSearchSpec.getData())) {
                    throw InvalidInputException("Error: Doctor does not match searched specialization.");
                }

                if (p->getBalance() < d->getFee()) {
                    throw InsufficientFundsException("Error: Insufficient account balance!");
                }

                std::ifstream checkFile("appointments.txt");
                if (checkFile.is_open()) {
                    char line[256];
                    while (checkFile.getline(line, sizeof(line))) {
                        if (myStrLen(line) < 5) {
                            continue;
                        }
                        char lineCopy[256];
                        myStrCopy(lineCopy, line);

                        char* cols[10];
                        int colIdx = 0;
                        cols[colIdx++] = lineCopy;

                        for (int k = 0; lineCopy[k] != '\0' && colIdx < 10; k++) {
                            if (lineCopy[k] == ',') {
                                lineCopy[k] = '\0';
                                cols[colIdx++] = &lineCopy[k + 1];
                            }
                        }

                        if (colIdx >= 6) {
                            int existDocId = Validator::charToInt(cols[2]);
                            if (existDocId == targetDocId && myStrEqual(cols[3], dateStr) && myStrEqual(cols[4], slotStr) && cols[5][0] == 'P') {
                                checkFile.close();
                                throw SlotUnavailableException("Error: Time slot is already booked for this date!");
                            }
                        }
                    }
                    checkFile.close();
                }

                *p -= d->getFee();
                FileHandler::saveAllPatients(state->patients);

                int newApptId = 1;
                std::ifstream inFile("appointments.txt");
                char line[256];

                while (inFile.getline(line, sizeof(line))) {
                    newApptId++;
                }
                inFile.close();

                std::ofstream outFile("appointments.txt", std::ios::app);
                if (outFile.is_open()) {
                    outFile << "\n" << newApptId << "," << p->getId() << "," << d->getId() << "," << dateStr << "," << slotStr << ",Pending";
                    outFile.close();
                }

                // Close the dataviewer list and show success
                dataViewer.show("Appointment Booked Successfully!");
            }
            catch (const HospitalException& e) {
                dataViewer.show(e.what());
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
                inFile.close();
                outFile.close();

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
                                    FileHandler::saveAllPatients(state->patients);
                                    outFile << cols[0] << "," << cols[1] << "," << cols[2] << "," << cols[3] << ",Paid\n";
                                    continue;
                                }
                            }
                        }
                        outFile << line << "\n";
                    }
                    inFile.close();
                    outFile.close();

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
                FileHandler::saveAllPatients(state->patients);
            }
        });
        return;
    }

    // --- BACKGROUND BUTTON CLICKS ---
    for (int i = 0; i < buttonCount; i++) 
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
                ApptRow rows[100];
                int rowCount = 0;

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

                for (int a = 0; a < rowCount - 1; a++) 
                {
                    for (int b = 0; b < rowCount - a - 1; b++) 
                    {
                        const char* d1 = rows[b].date;
                        const char* d2 = rows[b + 1].date;

                        int y1 = (d1[6] - '0') * 1000 + (d1[7] - '0') * 100 + (d1[8] - '0') * 10 + (d1[9] - '0');
                        int y2 = (d2[6] - '0') * 1000 + (d2[7] - '0') * 100 + (d2[8] - '0') * 10 + (d2[9] - '0');

                        int m1 = (d1[3] - '0') * 10 + (d1[4] - '0');
                        int m2 = (d2[3] - '0') * 10 + (d2[4] - '0');

                        int dd1 = (d1[0] - '0') * 10 + (d1[1] - '0');
                        int dd2 = (d2[0] - '0') * 10 + (d2[1] - '0');

                        bool swap = false;
                        if (y1 > y2 || (y1 == y2 && m1 > m2) || (y1 == y2 && m1 == m2 && dd1 > dd2)) 
                        {
                            swap = true;
                        }

                        if (swap) 
                        {
                            ApptRow tmp = rows[b];
                            rows[b] = rows[b + 1];
                            rows[b + 1] = tmp;
                        }
                    }
                }

                char displayBuffer[8192];
                displayBuffer[0] = '\0';

                for (int j = 0; j < rowCount; j++) 
                {
                    char temp[256];
                    myStrCopy(temp, "Appt ID: ");
                    myStrCopy(temp + myStrLen(temp), rows[j].id);
                    myStrCopy(temp + myStrLen(temp), " | Dr. ID: ");
                    myStrCopy(temp + myStrLen(temp), rows[j].docId);
                    myStrCopy(temp + myStrLen(temp), " | Date: ");
                    myStrCopy(temp + myStrLen(temp), rows[j].date);
                    myStrCopy(temp + myStrLen(temp), " | Slot: ");
                    myStrCopy(temp + myStrLen(temp), rows[j].slot);
                    myStrCopy(temp + myStrLen(temp), " | Status: ");
                    myStrCopy(temp + myStrLen(temp), rows[j].status);
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

                        if (colIdx >= 7 && Validator::charToInt(cols[2]) == state->loggedInUserId) 
                        {
                            myStrCopy(rows[rowCount].id, cols[0]);
                            myStrCopy(rows[rowCount].docId, cols[3]);
                            myStrCopy(rows[rowCount].date, cols[4]);
                            myStrCopy(rows[rowCount].meds, cols[5]);
                            myStrCopy(rows[rowCount].diag, cols[6]);
                            rowCount++;
                        }
                    }
                    inFile.close();
                }

                for (int a = 0; a < rowCount - 1; a++) 
                {
                    for (int b = 0; b < rowCount - a - 1; b++) 
                    {
                        const char* d1 = rows[b].date;
                        const char* d2 = rows[b + 1].date;

                        int y1 = (d1[6] - '0') * 1000 + (d1[7] - '0') * 100 + (d1[8] - '0') * 10 + (d1[9] - '0');
                        int y2 = (d2[6] - '0') * 1000 + (d2[7] - '0') * 100 + (d2[8] - '0') * 10 + (d2[9] - '0');

                        int m1 = (d1[3] - '0') * 10 + (d1[4] - '0');
                        int m2 = (d2[3] - '0') * 10 + (d2[4] - '0');

                        int dd1 = (d1[0] - '0') * 10 + (d1[1] - '0');
                        int dd2 = (d2[0] - '0') * 10 + (d2[1] - '0');

                        bool swap = false;
                        if (y1 < y2 || (y1 == y2 && m1 < m2) || (y1 == y2 && m1 == m2 && dd1 < dd2)) 
                        {
                            swap = true;
                        }

                        if (swap) 
                        {
                            RecordRow tmp = rows[b];
                            rows[b] = rows[b + 1];
                            rows[b + 1] = tmp;
                        }
                    }
                }

                char displayBuffer[8192];
                displayBuffer[0] = '\0';

                for (int j = 0; j < rowCount; j++) 
                {
                    char temp[256];
                    myStrCopy(temp, "Rec ID: ");
                    myStrCopy(temp + myStrLen(temp), rows[j].id);
                    myStrCopy(temp + myStrLen(temp), " | Date: ");
                    myStrCopy(temp + myStrLen(temp), rows[j].date);
                    myStrCopy(temp + myStrLen(temp), " | Diag: ");
                    myStrCopy(temp + myStrLen(temp), rows[j].diag);
                    myStrCopy(temp + myStrLen(temp), " | Meds: ");
                    myStrCopy(temp + myStrLen(temp), rows[j].meds);
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
                char displayBuffer[8192];
                displayBuffer[0] = '\0';
                float totalUnpaid = 0.0f;

                std::ifstream inFile("bills.txt");
                if (inFile.is_open()) {
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
                            myStrCopy(temp, "Bill ID: ");
                            myStrCopy(temp + myStrLen(temp), cols[0]);
                            myStrCopy(temp + myStrLen(temp), " | Amount: PKR ");
                            myStrCopy(temp + myStrLen(temp), cols[2]);
                            myStrCopy(temp + myStrLen(temp), " | Reason: ");
                            myStrCopy(temp + myStrLen(temp), cols[3]);
                            myStrCopy(temp + myStrLen(temp), " | Status: ");
                            myStrCopy(temp + myStrLen(temp), cols[4]);
                            myStrCopy(temp + myStrLen(temp), "\n");

                            myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);

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

// Update
void PatientScreen::update(float dt, RenderWindow& window) 
{
    if (state != nullptr && state->loggedInUserId != -1 && state->currentScreen == ScreenType::Patient) 
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
    else if (finalizeApptForm.isActive()) 
    {
        finalizeApptForm.update(window);          // <--- ADD THIS
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
    else 
    {
        for (int i = 0; i < buttonCount; i++) 
        {
            buttons[i]->update(window);
        }
    }
}

// Draw
void PatientScreen::draw(RenderWindow& window) 
{
    window.draw(*title);
    window.draw(*subtitle);

    for (int i = 0; i < buttonCount; i++) 
    {
        buttons[i]->draw(window);
    }

    if (bookApptForm.isActive()) 
    {
        bookApptForm.draw(window);
    }
    if (finalizeApptForm.isActive()) 
    {            // <--- ADD THIS
        finalizeApptForm.draw(window);            // <--- ADD THIS
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

// Destructor
PatientScreen::~PatientScreen()
{
    delete title;
    delete subtitle;
    for (int i = 0; i < buttonCount; i++)
    {
        delete buttons[i];
    }
    delete[] buttons;
}