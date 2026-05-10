#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"
#include "STRING.h"

enum class PrescStep { None, Step1Verify, Step2Form };

class DoctorScreen {
private:
    AppState* state;
    sf::Text* title;
    sf::Text* specText;
    Button** buttons;

    DataViewer dataViewer;
    InputForm completeForm;
    InputForm noShowForm;
    InputForm historyForm;

    PrescStep prescStep;
    int activePrescApptId;
    InputForm prescVerifyForm;
    InputForm prescDetailsForm;

    int parseDateToCompare(const char* dateStr);

public:
    DoctorScreen();
    ~DoctorScreen();
    void init(const sf::Font& font, AppState& appState);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window, AppState& appState);
    void update(float dt, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
};