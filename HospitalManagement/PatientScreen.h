#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"
#include "utility.h" // ADDED: This is required for myStrCopy, myStrLen, and myFloatToStr!

class PatientScreen {
private:
    AppState* state; // Pointer to fetch live data
    sf::Text* title;
    sf::Text* subtitle;
    Button* buttons[8];

public:
    PatientScreen() : state(nullptr), title(nullptr), subtitle(nullptr) {
        for (int i = 0; i < 8; i++) buttons[i] = nullptr;
    }

    ~PatientScreen() {
        delete title; delete subtitle;
        for (int i = 0; i < 8; i++) delete buttons[i];
    }

    void init(const sf::Font& font, AppState& appState) {
        state = &appState; // Store reference to global state

        title = new sf::Text(font);
        title->setCharacterSize(40);
        title->setFillColor(sf::Color(0, 51, 102));
        title->setPosition({ 50.f, 50.f });

        subtitle = new sf::Text(font);
        subtitle->setCharacterSize(30);
        subtitle->setFillColor(sf::Color(0, 100, 0)); // Green for money
        subtitle->setPosition({ 50.f, 100.f });

        const char* labels[] = {
            "1. Book Appointment", "2. Cancel Appointment", "3. View My Appointments",
            "4. View My Medical Records", "5. View My Bills", "6. Pay Bill",
            "7. Top Up Balance", "8. Logout"
        };

        for (int i = 0; i < 8; i++) {
            // Make the Logout button red
            sf::Color idle = (i == 7) ? sf::Color(200, 50, 50) : sf::Color(0, 120, 215);
            sf::Color hover = (i == 7) ? sf::Color(150, 0, 0) : sf::Color(0, 80, 160);

            buttons[i] = new Button({ 400.f, 50.f }, { 50.f, 180.f + (i * 60.f) }, labels[i], font, idle, hover);
        }
    }

    void handleEvent(const sf::Event& event, sf::RenderWindow& window, AppState& appState) {
        for (int i = 0; i < 8; i++) {
            buttons[i]->handleEvent(event, window, [&, i]() {
                if (i == 7) { // 8. Logout
                    appState.loggedInUserId = -1;
                    appState.currentScreen = ScreenType::Login;
                }
                else {
                    // TODO: Hook up the other menus here later!
                }
                });
        }
    }

    void update(float dt, sf::RenderWindow& window) {
        if (state && state->loggedInUserId != -1 && state->currentScreen == ScreenType::Patient) {
            Patient* p = state->patients.findByID(state->loggedInUserId);
            if (p != nullptr) {
                // 1. Build the welcome message safely (No snprintf)
                char welcomeMsg[150];
                myStrCopy(welcomeMsg, "Welcome, ");
                myStrCopy(welcomeMsg + myStrLen(welcomeMsg), p->getName()); // Append Name
                title->setString(welcomeMsg);

                // 2. Build the balance string safely (No snprintf)
                char balStr[50];
                myStrCopy(balStr, "Balance: PKR ");

                char balNum[32];
                myFloatToStr(p->getBalance(), balNum); // Convert Float to char[]
                myStrCopy(balStr + myStrLen(balStr), balNum); // Append Balance

                subtitle->setString(balStr);
            }
        }

        for (int i = 0; i < 8; i++) buttons[i]->update(window);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(*title);
        window.draw(*subtitle);
        for (int i = 0; i < 8; i++) buttons[i]->draw(window);
    }
};