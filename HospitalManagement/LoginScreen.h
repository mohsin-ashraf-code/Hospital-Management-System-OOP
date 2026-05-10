#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"
using namespace sf;

class LoginScreen 
{
private:
    AppState* state;
    Text* title;
    Button** buttons;
    InputForm loginForm;
    DataViewer dataViewer;
    int failedAttempts;
    int activeRoleChoice;

public:
    LoginScreen();
    ~LoginScreen();
    void init(const Font& font, AppState& appState);
    void handleEvent(const Event& event, RenderWindow& window, AppState& appState);
    void update(float dt, RenderWindow& window);
    void draw(RenderWindow& window);
};