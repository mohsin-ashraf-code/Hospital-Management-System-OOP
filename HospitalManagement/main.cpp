#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <optional>

// Backend & State
#include "AppState.h"
#include "FileHandler.h"
#include "FileNotFoundException.h"

// Screens
#include "LoginScreen.h"
#include "PatientScreen.h"
#include "DoctorScreen.h"
#include "AdminScreen.h"

using namespace sf;

int main()
{
    RenderWindow window(VideoMode({ 1920u, 1080u }), "MediCore");
    window.setFramerateLimit(60);

    Font font;
    if (!font.openFromFile("arial.ttf"))
    {
        std::cerr << "CRITICAL ERROR: Failed to load arial.ttf" << std::endl;
        return 1;
    }

    AppState appState;

    try 
    {
        FileHandler::loadPatients(appState.patients);
        FileHandler::loadDoctors(appState.doctors);
        FileHandler::loadAdmin(appState.admin);
        FileHandler::loadAppointments(appState.appointments);
        FileHandler::loadBills(appState.bills);
        FileHandler::loadPrescriptions(appState.prescriptions);
    }
    catch (const FileNotFoundException& e) 
    {
        Text errorText(font);
        errorText.setString(String("SYSTEM HALTED:\n") + e.what() + "\n\nPlease ensure the file is in your project directory.\nPress ESC to close.");
        errorText.setCharacterSize(35);
        errorText.setFillColor(Color::Red);
        errorText.setPosition({ 100.f, 100.f });

        while (window.isOpen()) 
        {
            while (const std::optional<Event> event = window.pollEvent()) 
            {
                if (event->is<Event::Closed>()) 
                {
                    window.close();
                }
                else if (const auto* keyPress = event->getIf<Event::KeyPressed>()) 
                {
                    if (keyPress->code == Keyboard::Key::Escape) 
                    {
                        window.close();
                    } 
                }
            }
            window.clear(Color::Black);
            window.draw(errorText);
            window.display();
        }
        return 1;
    }

   
    LoginScreen loginScreen;
    PatientScreen patientScreen;
    DoctorScreen doctorScreen;
    AdminScreen adminScreen;

    loginScreen.init(font, appState);
    patientScreen.init(font, appState);
    doctorScreen.init(font, appState);
    adminScreen.init(font, appState);

    Clock clock;

    while (window.isOpen())
    {
        while (const std::optional<Event> event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }

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

       
        float dt = clock.restart().asSeconds();

        switch (appState.currentScreen) 
        {
            case ScreenType::Login:
                loginScreen.update(dt, window); 
                break;
            case ScreenType::Patient: 
                patientScreen.update(dt, window); 
                break;
            case ScreenType::Doctor:
                doctorScreen.update(dt, window); 
                break;
            case ScreenType::Admin:
                adminScreen.update(dt, window); 
                break;
        }

        window.clear(Color(240, 248, 255));

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

    delete appState.admin;
    return 0;
}