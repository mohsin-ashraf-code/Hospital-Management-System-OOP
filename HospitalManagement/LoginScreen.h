#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"

class LoginScreen 
{
private:
    AppState* state;
    sf::Text* title;
    Button** buttons;
    InputForm loginForm;
    DataViewer dataViewer;
    int failedAttempts;
    int activeRoleChoice;

public:
    LoginScreen();
    ~LoginScreen();
    void init(const sf::Font& font, AppState& appState);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window, AppState& appState);
    void update(float dt, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
};