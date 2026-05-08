#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include "AppState.h"
#include "UIComponents.h"
#include "Validator.h"
#include "FileHandler.h"
#include "utility.h"

using namespace sf;
class LoginScreen {
private:
    // Internal State Tracker
    enum class LoginState { SelectRole, EnterCredentials, Locked };
    LoginState currentState;
    int selectedRole; // 1 = Patient, 2 = Doctor, 3 = Admin
    int failedAttempts;

    // Texts
     Text* title;
     Text* subtitle;
     Text* errorText;

    // State 1: Role Selection Buttons
    Button* btnPatient;
    Button* btnDoctor;
    Button* btnAdmin;
    Button* btnExit;

    // State 2: Credential Inputs
    TextBox* idInput;
    TextBox* passwordInput;
    Button* btnLogin;
    Button* btnBack;

public:
    LoginScreen() : currentState(LoginState::SelectRole), selectedRole(0), failedAttempts(0),
        title(nullptr), subtitle(nullptr), errorText(nullptr),
        btnPatient(nullptr), btnDoctor(nullptr), btnAdmin(nullptr), btnExit(nullptr),
        idInput(nullptr), passwordInput(nullptr), btnLogin(nullptr), btnBack(nullptr) {
    }

    ~LoginScreen() {
        delete title; delete subtitle; delete errorText;
        delete btnPatient; delete btnDoctor; delete btnAdmin; delete btnExit;
        delete idInput; delete passwordInput; delete btnLogin; delete btnBack;
    }

    void init(const  Font& font, AppState& appState) {
        float centerX = 1920.f / 2.f;
        float centerY = 1080.f / 2.f;

        // Texts
        title = new  Text(font);
        title->setString("Welcome to MediCore Hospital Management System");
        title->setCharacterSize(40);
        title->setFillColor( Color(0, 51, 102));
         FloatRect titleBounds = title->getLocalBounds();
        title->setPosition({ centerX - titleBounds.size.x / 2.f, 150.f });

        subtitle = new  Text(font);
        subtitle->setString("Select your role:");
        subtitle->setCharacterSize(30);
        subtitle->setFillColor( Color::Black);
         FloatRect subBounds = subtitle->getLocalBounds();
        subtitle->setPosition({ centerX - subBounds.size.x / 2.f, 250.f });

        errorText = new  Text(font);
        errorText->setString("");
        errorText->setCharacterSize(24);
        errorText->setFillColor( Color::Red);
        errorText->setPosition({ centerX - 200.f, centerY + 250.f });

        // State 1 Buttons (Role Selection)
        btnPatient = new Button({ 400.f, 60.f }, { centerX - 200.f, 350.f }, "1. Patient", font,  Color(0, 120, 215),  Color(0, 80, 160));
        btnDoctor = new Button({ 400.f, 60.f }, { centerX - 200.f, 430.f }, "2. Doctor", font,  Color(0, 120, 215),  Color(0, 80, 160));
        btnAdmin = new Button({ 400.f, 60.f }, { centerX - 200.f, 510.f }, "3. Admin", font,  Color(0, 120, 215),  Color(0, 80, 160));
        btnExit = new Button({ 400.f, 60.f }, { centerX - 200.f, 590.f }, "4. Exit", font,  Color(200, 50, 50),  Color(150, 0, 0));

        // State 2 Components (Credentials)
        idInput = new TextBox({ 400.f, 50.f }, { centerX - 200.f, 350.f }, "Enter ID", font, 10, false);
        passwordInput = new TextBox({ 400.f, 50.f }, { centerX - 200.f, 430.f }, "Enter Password", font, 50, true);
        btnLogin = new Button({ 190.f, 60.f }, { centerX - 200.f, 510.f }, "Login", font,  Color(0, 150, 0),  Color(0, 100, 0));
        btnBack = new Button({ 190.f, 60.f }, { centerX + 10.f, 510.f }, "Back", font,  Color(100, 100, 100),  Color(50, 50, 50));
    }

    void handleEvent(const  Event& event,  RenderWindow& window, AppState& appState) {
        if (currentState == LoginState::Locked) return; // Ignore clicks if locked

        if (currentState == LoginState::SelectRole) {
            btnPatient->handleEvent(event, window, [&]() { selectedRole = 1; subtitle->setString("Patient Login"); currentState = LoginState::EnterCredentials; errorText->setString(""); });
            btnDoctor->handleEvent(event, window, [&]() { selectedRole = 2; subtitle->setString("Doctor Login"); currentState = LoginState::EnterCredentials; errorText->setString(""); });
            btnAdmin->handleEvent(event, window, [&]() { selectedRole = 3; subtitle->setString("Admin Login"); currentState = LoginState::EnterCredentials; errorText->setString(""); });
            btnExit->handleEvent(event, window, [&]() { window.close(); });
        }
        else if (currentState == LoginState::EnterCredentials) {
            idInput->handleEvent(event, window);
            passwordInput->handleEvent(event, window);

            btnBack->handleEvent(event, window, [&]() {
                currentState = LoginState::SelectRole;
                subtitle->setString("Select your role:");
                errorText->setString("");
                idInput->clear();
                passwordInput->clear();
                });

            btnLogin->handleEvent(event, window, [&]() {
                int enteredId = Validator::charToInt(idInput->getText());
                const char* enteredPass = passwordInput->getText();
                bool success = false;
                const char* roleName = "";

                // --- DEBUG BLOCK ---
                std::cout << "\n--- LOGIN ATTEMPT ---" << std::endl;
                std::cout << "You typed ID: [" << enteredId << "] | Pass: [" << enteredPass << "]" << std::endl;

                Patient* dbPatient = appState.patients.findByID(1); // Force-grab the first patient
                if (dbPatient != nullptr) {
                    std::cout << "Database holds ID: [" << dbPatient->getId() << "] | Pass: [" << dbPatient->getPassword() << "]" << std::endl;
                }
                else {
                    std::cout << "Database could not find ID 1!" << std::endl;
                }
                // ----------------------------

                // Strict Validation based on selected role
                if (selectedRole == 3) { // ADMIN
                    roleName = "Admin";
                    if (appState.admin != nullptr && appState.admin->getId() == enteredId && myStrEqual(appState.admin->getPassword(), enteredPass)) {
                        success = true;
                    }
                }
                else if (selectedRole == 2) { // DOCTOR
                    roleName = "Doctor";
                    Doctor* d = appState.doctors.findByID(enteredId);
                    if (d != nullptr && myStrEqual(d->getPassword(), enteredPass)) {
                        success = true;
                    }
                }
                else if (selectedRole == 1) { // PATIENT
                    roleName = "Patient";
                    Patient* p = appState.patients.findByID(enteredId);
                    if (p != nullptr && myStrEqual(p->getPassword(), enteredPass)) {
                        success = true;
                    }
                }

                if (success) {
                    appState.loggedInUserId = enteredId;
                    myStrCopy(appState.loggedInRole, roleName);

                    // Route to correct screen
                    if (selectedRole == 1) appState.currentScreen = ScreenType::Patient;
                    if (selectedRole == 2) appState.currentScreen = ScreenType::Doctor;
                    if (selectedRole == 3) appState.currentScreen = ScreenType::Admin;

                    errorText->setString("");
                    failedAttempts = 0; // Reset on success
                }
                else {
                    failedAttempts++;
                    if (failedAttempts >= 3) {
                        currentState = LoginState::Locked;
                        errorText->setString("Account locked. Contact admin.");
                        FileHandler::appendSecurityLog(roleName, idInput->getText(), "LOCKED OUT - 3 Fails");
                    }
                    else {
                        // STRICTLY C-STYLE STRING BUILDING (No std::to_string)
                        char errorMsg[100];
                        myStrCopy(errorMsg, "Invalid ID or Password! Attempts left: ");

                        char numStr[16];
                        myIntToStr(3 - failedAttempts, numStr);

                        myStrCopy(errorMsg + myStrLen(errorMsg), numStr); // Stitch together

                        errorText->setString(errorMsg);
                        FileHandler::appendSecurityLog(roleName, idInput->getText(), "Failed Login");
                    }
                }
                });
        }
    }

    void update(float dt,  RenderWindow& window) {
        if (currentState == LoginState::SelectRole) {
            btnPatient->update(window);
            btnDoctor->update(window);
            btnAdmin->update(window);
            btnExit->update(window);
        }
        else if (currentState == LoginState::EnterCredentials) {
            idInput->update(window);
            passwordInput->update(window);
            btnLogin->update(window);
            btnBack->update(window);
        }
    }

    void draw( RenderWindow& window) {
        window.draw(*title);
        window.draw(*subtitle);

        if (currentState == LoginState::SelectRole) {
            btnPatient->draw(window);
            btnDoctor->draw(window);
            btnAdmin->draw(window);
            btnExit->draw(window);
        }
        else if (currentState == LoginState::EnterCredentials) {
            idInput->draw(window);
            passwordInput->draw(window);
            btnLogin->draw(window);
            btnBack->draw(window);
        }

        // Draw error/lockout message in all states
        window.draw(*errorText);
    }
};