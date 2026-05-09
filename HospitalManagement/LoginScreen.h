#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"

using namespace sf;

class LoginScreen 
{
private:
    enum class LoginState 
    { 
        SelectRole, EnterCredentials, Locked 
    };
    LoginState currentState;
    int selectedRole; // 1 = Patient, 2 = Doctor, 3 = Admin
    int failedAttempts;

    Text* title;
    Text* subtitle;
    Text* errorText;

    // State 1 Role Selection
    Button* btnPatient;
    Button* btnDoctor;
    Button* btnAdmin;
    Button* btnExit;

    // State 2 Enter Credentials
    TextBox* idInput;
    TextBox* passwordInput;
    Button* btnLogin;
    Button* btnBack;

public:
    LoginScreen();
    ~LoginScreen();

    void init(const Font& font, AppState& appState);
    void handleEvent(const Event& event, RenderWindow& window, AppState& appState);
    void update(float dt, RenderWindow& window);
    void draw(RenderWindow& window);
};