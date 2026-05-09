#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"
#include "STRING.h"

using namespace sf;

class PatientScreen 
{
private:
    AppState* state;
    Text* title;
    Text* subtitle;
    Button** buttons;
    int buttonCount;

    InputForm bookApptForm;
    InputForm cancelApptForm;
    InputForm payBillForm;
    InputForm topUpForm;
    DataViewer dataViewer;

    InputForm finalizeApptForm;  
    STRING currentSearchSpec;
public:
    PatientScreen();

    void init(const Font& font, AppState& appState);
    void handleEvent(const Event& event, RenderWindow& window, AppState& appState);
    void update(float dt, RenderWindow& window);
    void draw(RenderWindow& window);

    ~PatientScreen();
};