#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"
#include "utility.h" // Needed for myStrCopy and myStrLen

class DoctorScreen {
private:
    AppState* state;
    sf::Text* title;
    Button* buttons[6];

public:
    DoctorScreen() : state(nullptr), title(nullptr) {
        for (int i = 0; i < 6; i++) buttons[i] = nullptr;
    }

    ~DoctorScreen() {
        delete title;
        for (int i = 0; i < 6; i++) delete buttons[i];
    }

    void init(const sf::Font& font, AppState& appState) {
        state = &appState;

        title = new sf::Text(font);
        title->setCharacterSize(35);
        title->setFillColor(sf::Color(0, 51, 102));
        title->setPosition({ 50.f, 50.f });

        const char* labels[] = {
            "1. View Today's Appointments", "2. Mark Appointment Complete",
            "3. Mark Appointment No-Show", "4. Write Prescription",
            "5. View Patient Medical History", "6. Logout"
        };

        for (int i = 0; i < 6; i++) {
            sf::Color idle = (i == 5) ? sf::Color(200, 50, 50) : sf::Color(0, 150, 100); // Green theme for doctors
            sf::Color hover = (i == 5) ? sf::Color(150, 0, 0) : sf::Color(0, 100, 50);

            buttons[i] = new Button({ 450.f, 50.f }, { 50.f, 150.f + (i * 60.f) }, labels[i], font, idle, hover);
        }
    }

    void handleEvent(const sf::Event& event, sf::RenderWindow& window, AppState& appState) {
        for (int i = 0; i < 6; i++) {
            buttons[i]->handleEvent(event, window, [&, i]() {
                if (i == 5) { // 6. Logout
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
                // STRICTLY C-STYLE STRING BUILDING (No snprintf)
                char welcomeMsg[256];
                myStrCopy(welcomeMsg, "Welcome, Dr. ");
                myStrCopy(welcomeMsg + myStrLen(welcomeMsg), d->getName());
                myStrCopy(welcomeMsg + myStrLen(welcomeMsg), " | Specialization: ");
                myStrCopy(welcomeMsg + myStrLen(welcomeMsg), d->getSpecialization());

                title->setString(welcomeMsg);
            }
        }

        for (int i = 0; i < 6; i++) buttons[i]->update(window);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(*title);
        for (int i = 0; i < 6; i++) buttons[i]->draw(window);
    }
};