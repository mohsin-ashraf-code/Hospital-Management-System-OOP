#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include "AppState.h"
#include "UIComponents.h"
#include "utility.h"
#include "Validator.h"

using namespace sf;

class AdminScreen
{
private:
    AppState* state;
    Text* title;
    Button* buttons[10];

    InputForm addDoctorForm;
    InputForm removeDoctorForm;
    InputForm dischargeForm;
    DataViewer dataViewer;

    int calculateDaysBetween(const char* dateStr);

public:
    AdminScreen();
    ~AdminScreen();

    void init(const Font& font, AppState& appState);
    void handleEvent(const Event& event, RenderWindow& window, AppState& appState);
    void update(float dt, RenderWindow& window);
    void draw(RenderWindow& window);
};