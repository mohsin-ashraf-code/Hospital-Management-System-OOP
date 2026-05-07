#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"

class AdminScreen {
private:
    AppState* state;
    sf::Text* title;
    Button* buttons[10];

public:
    AdminScreen() : state(nullptr), title(nullptr) {
        for (int i = 0; i < 10; i++) buttons[i] = nullptr;
    }

    ~AdminScreen() {
        delete title;
        for (int i = 0; i < 10; i++) delete buttons[i];
    }

    void init(const sf::Font& font, AppState& appState) {
        state = &appState;

        title = new sf::Text(font);
        title->setString("Admin Panel - MediCore\n======================");
        title->setCharacterSize(40);
        title->setFillColor(sf::Color(0, 51, 102));
        title->setPosition({ 50.f, 50.f });

        const char* labels[] = {
            "1. Add Doctor", "2. Remove Doctor", "3. View All Patients",
            "4. View All Doctors", "5. View All Appointments", "6. View Unpaid Bills",
            "7. Discharge Patient", "8. View Security Log", "9. Generate Daily Report",
            "10. Logout"
        };

        for (int i = 0; i < 10; i++) {
            sf::Color idle = (i == 9) ? sf::Color(200, 50, 50) : sf::Color(80, 80, 80); // Gray theme for admin
            sf::Color hover = (i == 9) ? sf::Color(150, 0, 0) : sf::Color(50, 50, 50);

            buttons[i] = new Button({ 400.f, 50.f }, { 50.f, 150.f + (i * 60.f) }, labels[i], font, idle, hover);
        }
    }

    void handleEvent(const sf::Event& event, sf::RenderWindow& window, AppState& appState) {
        for (int i = 0; i < 10; i++) {
            buttons[i]->handleEvent(event, window, [&, i]() {
                if (i == 9) { // 10. Logout
                    appState.loggedInUserId = -1;
                    appState.currentScreen = ScreenType::Login;
                }
                });
        }
    }

    void update(float dt, sf::RenderWindow& window) {
        for (int i = 0; i < 10; i++) buttons[i]->update(window);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(*title);
        for (int i = 0; i < 10; i++) buttons[i]->draw(window);
    }
};