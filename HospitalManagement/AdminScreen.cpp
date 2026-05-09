#define _CRT_SECURE_NO_WARNINGS
#include "AdminScreen.h"
#include "utility.h"
#include "Validator.h"
#include "FileHandler.h"
#include <fstream>
#include <cstdio>

// Constructor
AdminScreen::AdminScreen() : state(nullptr), title(nullptr), buttonCount(10) {
    buttons = new Button * [buttonCount];
    for (int i = 0; i < buttonCount; i++) {
        buttons[i] = nullptr;
    }
}

// Destructor
AdminScreen::~AdminScreen() {
    delete title;
    for (int i = 0; i < buttonCount; i++) {
        delete buttons[i];
    }
    delete[] buttons;
}

// Init
void AdminScreen::init(const Font& font, AppState& appState) {
    state = &appState;

    title = new Text(font);
    title->setCharacterSize(40);
    title->setFillColor(Color(0, 51, 102));
    title->setPosition({ 50.f, 30.f });

    // EXACTLY the 10 buttons from the PDF
    const char* labels[] = {
        "1. Add Doctor",
        "2. Remove Doctor",
        "3. View All Patients",
        "4. View All Doctors",
        "5. View All Appointments",
        "6. View Unpaid Bills",
        "7. Discharge Patient",
        "8. View Security Log",
        "9. Generate Daily Report",
        "10. Logout"
    };

    // Adjusted spacing (50.f) so all 10 fit on screen
    for (int i = 0; i < buttonCount; i++) {
        Color idle = (i == 9) ? Color(200, 50, 50) : Color(0, 120, 215);
        Color hover = (i == 9) ? Color(150, 0, 0) : Color(0, 80, 160);
        buttons[i] = new Button({ 450.f, 40.f }, { 50.f, 100.f + (i * 50.f) }, labels[i], font, idle, hover);
    }

    const char* addDocFields[] = { "Name", "Specialization", "Fee" };
    addDoctorForm.init(font, "Add New Doctor", addDocFields, 3);

    const char* remDocFields[] = { "Doctor ID" };
    removeDoctorForm.init(font, "Remove Doctor", remDocFields, 1);

    const char* disPatFields[] = { "Patient ID" };
    dischargePatientForm.init(font, "Discharge Patient", disPatFields, 1);

    dataViewer.init(font, "Admin Dashboard");
}

// Handle Event
void AdminScreen::handleEvent(const Event& event, RenderWindow& window, AppState& appState) {
    if (dataViewer.isActive()) {
        dataViewer.handleEvent(event, window);
        return;
    }

    if (addDoctorForm.isActive()) {
        addDoctorForm.handleEvent(event, window, [&](const char** formData) {
            // TODO: Awaiting strict PDF logic
            dataViewer.show("Add Doctor logic pending PDF audit.");
            });
        return;
    }

    if (removeDoctorForm.isActive()) {
        removeDoctorForm.handleEvent(event, window, [&](const char** formData) {
            // TODO: Awaiting strict PDF logic
            dataViewer.show("Remove Doctor logic pending PDF audit.");
            });
        return;
    }

    if (dischargePatientForm.isActive()) {
        dischargePatientForm.handleEvent(event, window, [&](const char** formData) {
            // TODO: Awaiting strict PDF logic
            dataViewer.show("Discharge Patient logic pending PDF audit.");
            });
        return;
    }

    // --- BACKGROUND BUTTON CLICKS ---
    for (int i = 0; i < buttonCount; i++) {
        buttons[i]->handleEvent(event, window, [&, i]() {

            if (i == 0) {
                addDoctorForm.show();
            }
            else if (i == 1) {
                removeDoctorForm.show();
            }
            else if (i == 2) {
                dataViewer.show("View Patients logic pending PDF audit.");
            }
            else if (i == 3) {
                dataViewer.show("View Doctors logic pending PDF audit.");
            }
            else if (i == 4) {
                dataViewer.show("View Appts logic pending PDF audit.");
            }
            else if (i == 5) {
                dataViewer.show("View Unpaid Bills logic pending PDF audit.");
            }
            else if (i == 6) {
                dischargePatientForm.show();
            }
            else if (i == 7) {
                dataViewer.show("Security Log logic pending PDF audit.");
            }
            else if (i == 8) {
                dataViewer.show("Daily Report logic pending PDF audit.");
            }
            else if (i == 9) {
                appState.loggedInUserId = -1;
                appState.currentScreen = ScreenType::Login;
            }
            });
    }
}

// Update
void AdminScreen::update(float dt, RenderWindow& window) {
    if (state && state->loggedInUserId != -1 && state->currentScreen == ScreenType::Admin) {
        title->setString("Admin Panel - MediCore");
    }

    if (dataViewer.isActive()) {
        dataViewer.update(window);
    }
    else if (addDoctorForm.isActive()) {
        addDoctorForm.update(window);
    }
    else if (removeDoctorForm.isActive()) {
        removeDoctorForm.update(window);
    }
    else if (dischargePatientForm.isActive()) {
        dischargePatientForm.update(window);
    }
    else {
        for (int i = 0; i < buttonCount; i++) {
            buttons[i]->update(window);
        }
    }
}

// Draw
void AdminScreen::draw(RenderWindow& window) {
    window.draw(*title);

    for (int i = 0; i < buttonCount; i++) {
        buttons[i]->draw(window);
    }

    if (addDoctorForm.isActive()) {
        addDoctorForm.draw(window);
    }
    if (removeDoctorForm.isActive()) {
        removeDoctorForm.draw(window);
    }
    if (dischargePatientForm.isActive()) {
        dischargePatientForm.draw(window);
    }
    if (dataViewer.isActive()) {
        dataViewer.draw(window);
    }
}