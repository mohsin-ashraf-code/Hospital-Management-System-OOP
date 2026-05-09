#define _CRT_SECURE_NO_WARNINGS
#include "DoctorScreen.h"
#include "utility.h"
#include "Validator.h"
#include "FileHandler.h"
#include <fstream>
#include <cstdio>
#include <ctime>

// --- Helper Structs for Sorting ---
struct TodayApptRow {
    char id[16];
    char patName[100];
    char timeSlot[16];
    char status[16];
};

struct HistoryRow {
    char date[16];
    char meds[512];
    char notes[312];
};

// Constructor
DoctorScreen::DoctorScreen() : state(nullptr), title(nullptr), subtitle(nullptr), buttonCount(6) {
    buttons = new Button * [buttonCount];
    for (int i = 0; i < buttonCount; i++) {
        buttons[i] = nullptr;
    }
}

// Destructor
DoctorScreen::~DoctorScreen() {
    delete title;
    delete subtitle;
    for (int i = 0; i < buttonCount; i++) {
        delete buttons[i];
    }
    delete[] buttons;
}

// Init
void DoctorScreen::init(const Font& font, AppState& appState) {
    state = &appState;

    title = new Text(font);
    title->setCharacterSize(40);
    title->setFillColor(Color(0, 51, 102));
    title->setPosition({ 50.f, 50.f });

    subtitle = new Text(font);
    subtitle->setCharacterSize(30);
    subtitle->setFillColor(Color(0, 100, 0));
    subtitle->setPosition({ 50.f, 100.f });

    // EXACTLY the 6 buttons from the PDF (Point 1-6)
    const char* labels[] = {
        "1. View Today's Appointments",
        "2. Mark Appointment Complete",
        "3. Mark Appointment No-Show",
        "4. Write Prescription",
        "5. View Patient Medical History",
        "6. Logout"
    };

    for (int i = 0; i < buttonCount; i++) {
        Color idle = (i == 5) ? Color(200, 50, 50) : Color(0, 120, 215);
        Color hover = (i == 5) ? Color(150, 0, 0) : Color(0, 80, 160);
        buttons[i] = new Button({ 450.f, 50.f }, { 50.f, 180.f + (i * 60.f) }, labels[i], font, idle, hover);
    }

    const char* compFields[] = { "Enter Appointment ID" };
    markCompleteForm.init(font, "Mark Appointment Complete", compFields, 1);

    const char* noShowFields[] = { "Enter Appointment ID" };
    markNoShowForm.init(font, "Mark Appointment No-Show", noShowFields, 1);

    const char* prescFields[] = { "Appointment ID", "Medicines (e.g. Panadol 500mg)", "Notes" };
    writePrescriptionForm.init(font, "Write Prescription", prescFields, 3);

    const char* histFields[] = { "Enter Patient ID" };
    viewHistoryForm.init(font, "View Patient Medical History", histFields, 1);

    dataViewer.init(font, "Doctor Dashboard");
}

// Handle Event
void DoctorScreen::handleEvent(const Event& event, RenderWindow& window, AppState& appState) {
    if (dataViewer.isActive()) {
        dataViewer.handleEvent(event, window);
        return;
    }

    // Get today's date using time() and strftime() exactly as rubric requested (Point 1)
    char todayDate[16];
    time_t t = time(nullptr);
    struct tm* now = localtime(&t);
    strftime(todayDate, sizeof(todayDate), "%d-%m-%Y", now);

    // --- 2. MARK APPOINTMENT COMPLETE (Point 2) ---
    if (markCompleteForm.isActive()) {
        markCompleteForm.handleEvent(event, window, [&](const char** formData) {
            int targetAppt = Validator::charToInt(formData[0]);
            bool success = false;

            std::ifstream inFile("appointments.txt");
            std::ofstream outFile("temp_appts.txt");

            if (inFile.is_open() && outFile.is_open()) {
                char line[256];
                while (inFile.getline(line, sizeof(line))) {
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
                        int aId = Validator::charToInt(cols[0]);
                        int docId = Validator::charToInt(cols[2]);

                        // Validate: Belongs to Doc, Pending, and Dated Today
                        if (aId == targetAppt && docId == state->loggedInUserId && (cols[5][0] == 'P' || cols[5][0] == 'p') && myStrEqual(cols[3], todayDate)) {
                            outFile << cols[0] << "," << cols[1] << "," << cols[2] << "," << cols[3] << "," << cols[4] << ",completed\n";
                            success = true;
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
            if (success) {
                dataViewer.show("Appointment marked as completed.");
            }
            else {
                dataViewer.show("Error: Invalid ID, wrong date, or already updated.");
            }
            });
        return;
    }

    // --- 3. MARK APPOINTMENT NO-SHOW (Point 3) ---
    if (markNoShowForm.isActive()) {
        markNoShowForm.handleEvent(event, window, [&](const char** formData) {
            int targetAppt = Validator::charToInt(formData[0]);
            bool success = false;

            std::ifstream inFile("appointments.txt");
            std::ofstream outFile("temp_appts.txt");

            if (inFile.is_open() && outFile.is_open()) {
                char line[256];
                while (inFile.getline(line, sizeof(line))) {
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
                        int aId = Validator::charToInt(cols[0]);
                        int docId = Validator::charToInt(cols[2]);

                        if (aId == targetAppt && docId == state->loggedInUserId && (cols[5][0] == 'P' || cols[5][0] == 'p') && myStrEqual(cols[3], todayDate)) {
                            outFile << cols[0] << "," << cols[1] << "," << cols[2] << "," << cols[3] << "," << cols[4] << ",noshow\n";
                            success = true;
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

            if (success) {
                // Update corresponding bill to cancelled
                std::ifstream bIn("bills.txt");
                std::ofstream bOut("temp_bills.txt");
                if (bIn.is_open() && bOut.is_open()) {
                    char bLine[256];
                    while (bIn.getline(bLine, sizeof(bLine))) {
                        if (myStrLen(bLine) < 5) {
                            continue;
                        }
                        char bLineCopy[256];
                        myStrCopy(bLineCopy, bLine);

                        char* bCols[10];
                        int bColIdx = 0;
                        bCols[bColIdx++] = bLineCopy;

                        for (int k = 0; bLineCopy[k] != '\0' && bColIdx < 10; k++) {
                            if (bLineCopy[k] == ',') {
                                bLineCopy[k] = '\0';
                                bCols[bColIdx++] = &bLineCopy[k + 1];
                            }
                        }
                        if (bColIdx >= 6) {
                            int bApptId = Validator::charToInt(bCols[2]);
                            if (bApptId == targetAppt) {
                                bOut << bCols[0] << "," << bCols[1] << "," << bCols[2] << "," << bCols[3] << ",cancelled," << bCols[5] << "\n";
                                continue;
                            }
                        }
                        bOut << bLine << "\n";
                    }
                    bIn.close();
                    bOut.close();

                    (void)remove("bills.txt");
                    (void)rename("temp_bills.txt", "bills.txt");
                }
                // Exact string required by the rubric
                dataViewer.show("Appointment marked as no-show.");
            }
            else {
                dataViewer.show("Error: Invalid ID, wrong date, or already updated.");
            }
            });
        return;
    }

    // --- 4. WRITE PRESCRIPTION (Point 4) ---
    if (writePrescriptionForm.isActive()) {
        writePrescriptionForm.handleEvent(event, window, [&](const char** formData) {
            int targetAppt = Validator::charToInt(formData[0]);

            // Validate: Belongs to Doc & Status == Completed
            int foundPatId = -1;
            char apptDate[16];
            std::ifstream aCheck("appointments.txt");

            if (aCheck.is_open()) {
                char aLine[256];
                while (aCheck.getline(aLine, sizeof(aLine))) {
                    if (myStrLen(aLine) < 5) {
                        continue;
                    }
                    char aCopy[256];
                    myStrCopy(aCopy, aLine);

                    char* aCols[10];
                    int aIdx = 0;
                    aCols[aIdx++] = aCopy;

                    for (int k = 0; aCopy[k] != '\0' && aIdx < 10; k++) {
                        if (aCopy[k] == ',') {
                            aCopy[k] = '\0';
                            aCols[aIdx++] = &aCopy[k + 1];
                        }
                    }
                    if (aIdx >= 6) {
                        if (Validator::charToInt(aCols[0]) == targetAppt && Validator::charToInt(aCols[2]) == state->loggedInUserId && (aCols[5][0] == 'c' || aCols[5][0] == 'C')) {
                            foundPatId = Validator::charToInt(aCols[1]);
                            myStrCopy(apptDate, aCols[3]);
                        }
                    }
                }
                aCheck.close();
            }

            if (foundPatId == -1) {
                dataViewer.show("Error: Invalid Appointment ID or not completed.");
                return;
            }

            // Check if prescription already exists
            std::ifstream pCheck("prescriptions.txt");
            if (pCheck.is_open()) {
                char pLine[256];
                while (pCheck.getline(pLine, sizeof(pLine))) {
                    if (myStrLen(pLine) < 5) {
                        continue;
                    }
                    char pCopy[256];
                    myStrCopy(pCopy, pLine);

                    char* pCols[10];
                    int pIdx = 0;
                    pCols[pIdx++] = pCopy;

                    for (int k = 0; pCopy[k] != '\0' && pIdx < 10; k++) {
                        if (pCopy[k] == ',') {
                            pCopy[k] = '\0';
                            pCols[pIdx++] = &pCopy[k + 1];
                        }
                    }
                    if (pIdx >= 7 && Validator::charToInt(pCols[1]) == targetAppt) {
                        pCheck.close();
                        dataViewer.show("Prescription already written for this appointment.");
                        return;
                    }
                }
                pCheck.close();
            }

            // Truncate silently using strict pointer arithmetic per rubric!
            char safeMeds[500];
            int m = 0;
            while (*(formData[1] + m) != '\0' && m < 499) {
                *(safeMeds + m) = *(formData[1] + m);
                m++;
            }
            *(safeMeds + m) = '\0';

            char safeNotes[300];
            int n = 0;
            while (*(formData[2] + n) != '\0' && n < 299) {
                *(safeNotes + n) = *(formData[2] + n);
                n++;
            }
            *(safeNotes + n) = '\0';

            // Generate New ID & Write
            int newPrescId = 1;
            std::ifstream pIn("prescriptions.txt");
            char tempLine[256];
            while (pIn.getline(tempLine, sizeof(tempLine))) {
                newPrescId++;
            }
            pIn.close();

            std::ofstream pOut("prescriptions.txt", std::ios::app);
            if (pOut.is_open()) {
                pOut << "\n" << newPrescId << "," << targetAppt << "," << foundPatId << "," << state->loggedInUserId << "," << apptDate << "," << safeMeds << "," << safeNotes;
                pOut.close();
            }
            dataViewer.show("Prescription saved.");
            });
        return;
    }

    // --- 5. VIEW PATIENT MEDICAL HISTORY (Point 5) ---
    if (viewHistoryForm.isActive()) {
        viewHistoryForm.handleEvent(event, window, [&](const char** formData) {
            int targetPatId = Validator::charToInt(formData[0]);

            // Validate: Patient exists and has at least one completed appt with this doctor
            bool hasAccess = false;
            std::ifstream aCheck("appointments.txt");
            if (aCheck.is_open()) {
                char aLine[256];
                while (aCheck.getline(aLine, sizeof(aLine))) {
                    if (myStrLen(aLine) < 5) {
                        continue;
                    }
                    char aCopy[256];
                    myStrCopy(aCopy, aLine);

                    char* aCols[10];
                    int aIdx = 0;
                    aCols[aIdx++] = aCopy;

                    for (int k = 0; aCopy[k] != '\0' && aIdx < 10; k++) {
                        if (aCopy[k] == ',') {
                            aCopy[k] = '\0';
                            aCols[aIdx++] = &aCopy[k + 1];
                        }
                    }
                    if (aIdx >= 6) {
                        int pId = Validator::charToInt(aCols[1]);
                        int docId = Validator::charToInt(aCols[2]);
                        if (pId == targetPatId && docId == state->loggedInUserId && (aCols[5][0] == 'c' || aCols[5][0] == 'C')) {
                            hasAccess = true;
                        }
                    }
                }
                aCheck.close();
            }

            if (!hasAccess) {
                dataViewer.show("Access denied. You can only view records of your own patients.");
                return;
            }

            // Fetch and sort prescriptions descending by date
            HistoryRow rows[100];
            int rowCount = 0;

            std::ifstream inFile("prescriptions.txt");
            if (inFile.is_open()) {
                char line[256];
                while (inFile.getline(line, sizeof(line)) && rowCount < 100) {
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
                    if (colIdx >= 7 && Validator::charToInt(cols[2]) == targetPatId && Validator::charToInt(cols[3]) == state->loggedInUserId) {
                        myStrCopy(rows[rowCount].date, cols[4]);
                        myStrCopy(rows[rowCount].meds, cols[5]);
                        myStrCopy(rows[rowCount].notes, cols[6]);
                        rowCount++;
                    }
                }
                inFile.close();
            }

            // Sort Descending Date
            for (int a = 0; a < rowCount - 1; a++) {
                for (int b = 0; b < rowCount - a - 1; b++) {
                    const char* d1 = rows[b].date;
                    const char* d2 = rows[b + 1].date;

                    int y1 = (*(d1 + 6) - '0') * 1000 + (*(d1 + 7) - '0') * 100 + (*(d1 + 8) - '0') * 10 + (*(d1 + 9) - '0');
                    int y2 = (*(d2 + 6) - '0') * 1000 + (*(d2 + 7) - '0') * 100 + (*(d2 + 8) - '0') * 10 + (*(d2 + 9) - '0');

                    int m1 = (*(d1 + 3) - '0') * 10 + (*(d1 + 4) - '0');
                    int m2 = (*(d2 + 3) - '0') * 10 + (*(d2 + 4) - '0');

                    int dd1 = (*(d1 + 0) - '0') * 10 + (*(d1 + 1) - '0');
                    int dd2 = (*(d2 + 0) - '0') * 10 + (*(d2 + 1) - '0');

                    bool swap = false;
                    if (y1 < y2 || (y1 == y2 && m1 < m2) || (y1 == y2 && m1 == m2 && dd1 < dd2)) {
                        swap = true;
                    }

                    if (swap) {
                        HistoryRow tmp = rows[b];
                        rows[b] = rows[b + 1];
                        rows[b + 1] = tmp;
                    }
                }
            }

            char displayBuffer[8192];
            displayBuffer[0] = '\0';

            for (int j = 0; j < rowCount; j++) {
                char temp[512];
                myStrCopy(temp, "Date: ");
                myStrCopy(temp + myStrLen(temp), rows[j].date);
                myStrCopy(temp + myStrLen(temp), "\n   Notes: ");
                myStrCopy(temp + myStrLen(temp), rows[j].notes);
                myStrCopy(temp + myStrLen(temp), "\n   Meds:  ");
                myStrCopy(temp + myStrLen(temp), rows[j].meds);
                myStrCopy(temp + myStrLen(temp), "\n");
                myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
            }

            if (myStrLen(displayBuffer) == 0) {
                myStrCopy(displayBuffer, "No medical history found.");
            }
            dataViewer.show(displayBuffer);
            });
        return;
    }

    // --- BACKGROUND BUTTON CLICKS ---
    for (int i = 0; i < buttonCount; i++) {
        buttons[i]->handleEvent(event, window, [&, i]() {

            if (i == 0) { // 1. View Today's Appointments
                TodayApptRow rows[100];
                int rowCount = 0;

                std::ifstream inFile("appointments.txt");
                if (inFile.is_open()) {
                    char line[256];
                    while (inFile.getline(line, sizeof(line)) && rowCount < 100) {
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

                        if (colIdx >= 6 && Validator::charToInt(cols[2]) == state->loggedInUserId && myStrEqual(cols[3], todayDate)) {
                            myStrCopy(rows[rowCount].id, cols[0]);
                            myStrCopy(rows[rowCount].timeSlot, cols[4]);
                            myStrCopy(rows[rowCount].status, cols[5]);

                            Patient* pat = state->patients.findByID(Validator::charToInt(cols[1]));
                            if (pat != nullptr) {
                                myStrCopy(rows[rowCount].patName, pat->getName());
                            }
                            else {
                                myStrCopy(rows[rowCount].patName, "Unknown");
                            }
                            rowCount++;
                        }
                    }
                    inFile.close();
                }

                // Bubble Sort Ascending TimeSlot
                for (int a = 0; a < rowCount - 1; a++) {
                    for (int b = 0; b < rowCount - a - 1; b++) {
                        int h1 = (*(rows[b].timeSlot + 0) - '0') * 10 + (*(rows[b].timeSlot + 1) - '0');
                        int h2 = (*(rows[b + 1].timeSlot + 0) - '0') * 10 + (*(rows[b + 1].timeSlot + 1) - '0');

                        if (*(rows[b].timeSlot + 5) == 'P' && h1 != 12) {
                            h1 += 12;
                        }
                        if (*(rows[b].timeSlot + 5) == 'A' && h1 == 12) {
                            h1 = 0;
                        }
                        if (*(rows[b + 1].timeSlot + 5) == 'P' && h2 != 12) {
                            h2 += 12;
                        }
                        if (*(rows[b + 1].timeSlot + 5) == 'A' && h2 == 12) {
                            h2 = 0;
                        }

                        bool swap = false;
                        if (h1 > h2) {
                            swap = true;
                        }

                        if (swap) {
                            TodayApptRow tmp = rows[b];
                            rows[b] = rows[b + 1];
                            rows[b + 1] = tmp;
                        }
                    }
                }

                char displayBuffer[8192];
                displayBuffer[0] = '\0';

                for (int j = 0; j < rowCount; j++) {
                    char temp[256];
                    myStrCopy(temp, rows[j].id);
                    myStrCopy(temp + myStrLen(temp), " | ");
                    myStrCopy(temp + myStrLen(temp), rows[j].patName);
                    myStrCopy(temp + myStrLen(temp), " | ");
                    myStrCopy(temp + myStrLen(temp), rows[j].timeSlot);
                    myStrCopy(temp + myStrLen(temp), " | ");
                    myStrCopy(temp + myStrLen(temp), rows[j].status);
                    myStrCopy(temp + myStrLen(temp), "\n");
                    myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                }

                if (myStrLen(displayBuffer) == 0) {
                    myStrCopy(displayBuffer, "No appointments scheduled for today.");
                }
                dataViewer.show(displayBuffer);

            }
            else if (i == 1) {
                // Display pending first before form (Point 2)
                char displayBuffer[8192];
                displayBuffer[0] = '\0';
                myStrCopy(displayBuffer, "--- TODAY'S PENDING APPOINTMENTS ---\n");

                std::ifstream inFile("appointments.txt");
                if (inFile.is_open()) {
                    char line[256];
                    while (inFile.getline(line, sizeof(line))) {
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

                        if (colIdx >= 6 && Validator::charToInt(cols[2]) == state->loggedInUserId && myStrEqual(cols[3], todayDate) && (cols[5][0] == 'P' || cols[5][0] == 'p')) {
                            char temp[256];
                            myStrCopy(temp, "ID: ");
                            myStrCopy(temp + myStrLen(temp), cols[0]);
                            myStrCopy(temp + myStrLen(temp), " | Pat ID: ");
                            myStrCopy(temp + myStrLen(temp), cols[1]);
                            myStrCopy(temp + myStrLen(temp), " | Time: ");
                            myStrCopy(temp + myStrLen(temp), cols[4]);
                            myStrCopy(temp + myStrLen(temp), "\n");

                            myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                        }
                    }
                    inFile.close();
                }

                if (myStrEqual(displayBuffer, "--- TODAY'S PENDING APPOINTMENTS ---\n")) {
                    myStrCopy(displayBuffer + myStrLen(displayBuffer), "No pending appointments to complete.");
                }

                dataViewer.show(displayBuffer);
                markCompleteForm.show();

            }
            else if (i == 2) {
                // Display pending first before form (Point 3)
                char displayBuffer[8192];
                displayBuffer[0] = '\0';
                myStrCopy(displayBuffer, "--- TODAY'S PENDING APPOINTMENTS ---\n");

                std::ifstream inFile("appointments.txt");
                if (inFile.is_open()) {
                    char line[256];
                    while (inFile.getline(line, sizeof(line))) {
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

                        if (colIdx >= 6 && Validator::charToInt(cols[2]) == state->loggedInUserId && myStrEqual(cols[3], todayDate) && (cols[5][0] == 'P' || cols[5][0] == 'p')) {
                            char temp[256];
                            myStrCopy(temp, "ID: ");
                            myStrCopy(temp + myStrLen(temp), cols[0]);
                            myStrCopy(temp + myStrLen(temp), " | Pat ID: ");
                            myStrCopy(temp + myStrLen(temp), cols[1]);
                            myStrCopy(temp + myStrLen(temp), " | Time: ");
                            myStrCopy(temp + myStrLen(temp), cols[4]);
                            myStrCopy(temp + myStrLen(temp), "\n");

                            myStrCopy(displayBuffer + myStrLen(displayBuffer), temp);
                        }
                    }
                    inFile.close();
                }

                if (myStrEqual(displayBuffer, "--- TODAY'S PENDING APPOINTMENTS ---\n")) {
                    myStrCopy(displayBuffer + myStrLen(displayBuffer), "No pending appointments to mark as no-show.");
                }

                dataViewer.show(displayBuffer);
                markNoShowForm.show();

            }
            else if (i == 3) {
                writePrescriptionForm.show();
            }
            else if (i == 4) {
                viewHistoryForm.show();
            }
            else if (i == 5) {
                appState.loggedInUserId = -1;
                appState.currentScreen = ScreenType::Login;
            }
            });
    }
}

// Update
void DoctorScreen::update(float dt, RenderWindow& window) {
    if (state && state->loggedInUserId != -1 && state->currentScreen == ScreenType::Doctor) {
        Doctor* d = state->doctors.findByID(state->loggedInUserId);
        if (d != nullptr) {
            char welcomeMsg[150];
            myStrCopy(welcomeMsg, "Welcome, Dr. ");
            myStrCopy(welcomeMsg + myStrLen(welcomeMsg), d->getName());

            myStrCopy(welcomeMsg + myStrLen(welcomeMsg), " | Specialization: ");
            myStrCopy(welcomeMsg + myStrLen(welcomeMsg), d->getSpecialization());
            title->setString(welcomeMsg);

            subtitle->setString("");
        }
    }

    if (dataViewer.isActive()) {
        dataViewer.update(window);
    }
    else if (markCompleteForm.isActive()) {
        markCompleteForm.update(window);
    }
    else if (markNoShowForm.isActive()) {
        markNoShowForm.update(window);
    }
    else if (writePrescriptionForm.isActive()) {
        writePrescriptionForm.update(window);
    }
    else if (viewHistoryForm.isActive()) {
        viewHistoryForm.update(window);
    }
    else {
        for (int i = 0; i < buttonCount; i++) {
            buttons[i]->update(window);
        }
    }
}

// Draw
void DoctorScreen::draw(RenderWindow& window) {
    window.draw(*title);
    window.draw(*subtitle);

    for (int i = 0; i < buttonCount; i++) {
        buttons[i]->draw(window);
    }

    if (markCompleteForm.isActive()) 
    {
        markCompleteForm.draw(window);
    }

    if (markNoShowForm.isActive()) 
    {
        markNoShowForm.draw(window);
    }
    if (writePrescriptionForm.isActive()) {
        writePrescriptionForm.draw(window);
    }
    if (viewHistoryForm.isActive()) {
        viewHistoryForm.draw(window);
    }
    if (dataViewer.isActive()) {
        dataViewer.draw(window);
    }
}