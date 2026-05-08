#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>

// Backend & State
#include "AppState.h"
#include "FileHandler.h"

// Screens
#include "LoginScreen.h"
#include "PatientScreen.h"
#include "DoctorScreen.h"
#include "AdminScreen.h"

int main() 
{
    // 1. Create window: 1920x1080, title "MediCore"
    // SFML 3 VideoMode syntax
    RenderWindow window( VideoMode({ 1920u, 1080u }), "MediCore");
    window.setFramerateLimit(60); // Keeps CPU usage low

    // 2. Load ONE font from file
    Font font;
    // Note: SFML 3 standardizes resource loading methods to openFromFile
    if (!font.openFromFile("arial.ttf")) 
    {
        std::cerr << "CRITICAL ERROR: Failed to load arial.ttf" << std::endl;
        return 1;
    }

    // 3. Declare AppState
    AppState appState;

    // 4. Call FileHandler::load* for all 6 data types
    FileHandler::loadPatients(appState.patients);
    FileHandler::loadDoctors(appState.doctors);
    FileHandler::loadAdmin(appState.admin);
    FileHandler::loadAppointments(appState.appointments);
    FileHandler::loadBills(appState.bills);
    FileHandler::loadPrescriptions(appState.prescriptions);

    // 5. Declare all 4 screens
    LoginScreen loginScreen;
    PatientScreen patientScreen;
    DoctorScreen doctorScreen;
    AdminScreen adminScreen;

    // 6. Call .init(font, appState) on all 4
    loginScreen.init(font, appState);
    patientScreen.init(font, appState);
    doctorScreen.init(font, appState);
    adminScreen.init(font, appState);

    // 7.  Clock for delta time
    Clock clock;

    // 8. Main loop
    while (window.isOpen()) 
    {

        // --- EVENT POLLING (SFML 3 optional syntax) ---
        while (const std::optional< Event> event = window.pollEvent()) 
        {

            // Close window request
            if (event->is< Event::Closed>()) 
            {
                window.close();
            }

            // Route events to the currently active screen
            switch (appState.currentScreen) 
            {
            case ScreenType::Login:   
                loginScreen.handleEvent(*event, window, appState); 
                break;
            case ScreenType::Patient: 
                patientScreen.handleEvent(*event, window, appState); 
                break;
            case ScreenType::Doctor:  
                doctorScreen.handleEvent(*event, window, appState); 
                break;
            case ScreenType::Admin:   
                adminScreen.handleEvent(*event, window, appState); 
                break;
            }
        }

        // --- DELTA TIME ---
        float dt = clock.restart().asSeconds();

        // --- UPDATE ROUTING ---
        switch (appState.currentScreen) {
        case ScreenType::Login:   loginScreen.update(dt, window); break;
        case ScreenType::Patient: patientScreen.update(dt, window); break;
        case ScreenType::Doctor:  doctorScreen.update(dt, window); break;
        case ScreenType::Admin:   adminScreen.update(dt, window); break;
        }

        // --- DRAWING ---
        window.clear( Color(240, 248, 255)); // A nice hospital-themed background (Alice Blue)

        switch (appState.currentScreen) 
        {
        case ScreenType::Login:   
            loginScreen.draw(window); 
            break;
        case ScreenType::Patient: 
            patientScreen.draw(window); 
            break;
        case ScreenType::Doctor:  
            doctorScreen.draw(window); 
            break;
        case ScreenType::Admin:   
            adminScreen.draw(window); 
            break;
        }

        window.display();
    }

    // Optional: Clean up dynamically allocated Admin on exit
    delete appState.admin;

    return 0;
}