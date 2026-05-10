#include "LoginScreen.h"
#include "Validator.h"
#include "FileHandler.h"

extern sf::String intToStr(int num);

LoginScreen::LoginScreen() : state(nullptr), title(nullptr), buttons(nullptr), failedAttempts(0), activeRoleChoice(0) {}

LoginScreen::~LoginScreen() {
    delete title;
    if (buttons) {
        for (int i = 0; i < 4; i++) delete* (buttons + i);
        delete[] buttons;
    }
}

void LoginScreen::init(const sf::Font& font, AppState& appState) {
    state = &appState;

    title = new sf::Text(font);
    title->setString("MediCore Hospital Management System");
    title->setCharacterSize(30);
    title->setFillColor(sf::Color::Cyan);
    title->setPosition({ 120.f, 60.f });

    buttons = new Button * [4];
    const char* labels[] = { "1. Patient Login", "2. Doctor Login", "3. Admin Login", "4. Exit Application" };

    for (int i = 0; i < 4; i++) {
        sf::Color idle = (i == 3) ? sf::Color(150, 0, 0) : sf::Color(0, 102, 204);
        sf::Color hover = (i == 3) ? sf::Color(200, 0, 0) : sf::Color(0, 120, 240);
        *(buttons + i) = new Button({ 300.f, 50.f }, { 250.f, 160.f + (i * 75.f) }, *(labels + i), font, idle, hover);
    }

    const char* fields[] = { "Enter User ID", "Enter Password" };
    loginForm.init(font, "Security Portal Verification", fields, 2);
    dataViewer.init(font, "Security Monitor");
}

void LoginScreen::handleEvent(const sf::Event& event, sf::RenderWindow& window, AppState& appState) {
    if (dataViewer.isActive()) {
        dataViewer.handleEvent(event, window);
        return;
    }

    if (loginForm.isActive()) {
        loginForm.handleEvent(event, window, [&](const char** formData) {
            const char* enteredIdStr = *(formData + 0);
            const char* enteredPass = *(formData + 1);

            if (failedAttempts >= 3) {
                dataViewer.show("Account locked. Contact admin.");
                return;
            }

            int id = Validator::charToInt(enteredIdStr);

            if (activeRoleChoice == 1) { // Patient
                Patient* p = state->patients.findByID(id);
                if (p && Validator::myStrEqual(p->getPassword(), enteredPass)) {
                    failedAttempts = 0;
                    FileHandler::appendSecurityLog("Patient", enteredIdStr, "SUCCESS");
                    appState.loggedInUserId = id;
                    appState.currentScreen = ScreenType::Patient;
                }
                else {
                    failedAttempts++;
                    FileHandler::appendSecurityLog("Patient", enteredIdStr, "FAILED");
                    dataViewer.show("Invalid Credentials. Attempts remaining: " + intToStr(3 - failedAttempts));
                }
            }
            else if (activeRoleChoice == 2) { // Doctor
                Doctor* d = state->doctors.findByID(id);
                if (d && Validator::myStrEqual(d->getPassword(), enteredPass)) {
                    failedAttempts = 0;
                    FileHandler::appendSecurityLog("Doctor", enteredIdStr, "SUCCESS");
                    appState.loggedInUserId = id;
                    appState.currentScreen = ScreenType::Doctor;
                }
                else {
                    failedAttempts++;
                    FileHandler::appendSecurityLog("Doctor", enteredIdStr, "FAILED");
                    dataViewer.show("Invalid Credentials. Attempts remaining: " + intToStr(3 - failedAttempts));
                }
            }
            else if (activeRoleChoice == 3) { // Admin
                if (state->admin && state->admin->getId() == id && Validator::myStrEqual(state->admin->getPassword(), enteredPass)) {
                    failedAttempts = 0;
                    FileHandler::appendSecurityLog("Admin", enteredIdStr, "SUCCESS");
                    appState.loggedInUserId = id;
                    appState.currentScreen = ScreenType::Admin;
                }
                else {
                    failedAttempts++;
                    FileHandler::appendSecurityLog("Admin", enteredIdStr, "FAILED");
                    dataViewer.show("Invalid Credentials. Attempts remaining: " + intToStr(3 - failedAttempts));
                }
            }
            });
        return;
    }

    for (int i = 0; i < 4; i++) {
        (*(buttons + i))->handleEvent(event, window, [&, i]() {
            if (i == 3) window.close();
            else {
                activeRoleChoice = i + 1;
                loginForm.show();
            }
            });
    }
}

void LoginScreen::update(float dt, sf::RenderWindow& window) {
    if (dataViewer.isActive()) dataViewer.update(window);
    else if (loginForm.isActive()) loginForm.update(window);
    else if (buttons) {
        for (int i = 0; i < 4; i++) (*(buttons + i))->update(window);
    }
}

void LoginScreen::draw(sf::RenderWindow& window) {
    if (title) window.draw(*title);
    if (buttons) {
        for (int i = 0; i < 4; i++) (*(buttons + i))->draw(window);
    }
    if (loginForm.isActive()) loginForm.draw(window);
    if (dataViewer.isActive()) dataViewer.draw(window);
}