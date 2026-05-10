#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"

class AdminScreen {
private:
    AppState* state;
    sf::Text* title;
    Button* buttons[10]; // 10 Options exactly as requested

    InputForm addDoctorForm;
    InputForm removeDoctorForm;
    InputForm dischargeForm;

    DataViewer dataViewer;

    int calculateDaysBetween(const char* dateStr);

public:
    AdminScreen();
    ~AdminScreen();
    void init(const sf::Font& font, AppState& appState);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window, AppState& appState);
    void update(float dt, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
};