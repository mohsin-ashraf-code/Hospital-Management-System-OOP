#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"

using namespace sf;

class DoctorScreen 
{
private:
    AppState* state;
    Text* title;
    Text* subtitle;
    Button** buttons;
    int buttonCount;

    InputForm markCompleteForm;
    InputForm markNoShowForm;
    InputForm writePrescriptionForm;
    InputForm viewHistoryForm;
    DataViewer dataViewer;

public:
    DoctorScreen();
    ~DoctorScreen();

    void init(const Font& font, AppState& appState);
    void handleEvent(const Event& event, RenderWindow& window, AppState& appState);
    void update(float dt, RenderWindow& window);
    void draw(RenderWindow& window);
};