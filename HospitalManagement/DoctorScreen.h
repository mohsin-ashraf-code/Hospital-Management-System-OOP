#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"
#include "STRING.h"
using namespace sf;

enum class PrescStep { None, Step1Verify, Step2Form };

class DoctorScreen 
{
private:
    AppState* state;
    Text* title;
    Text* specText;
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
    
    void init(const Font& font, AppState& appState);
    void handleEvent(const Event& event, RenderWindow& window, AppState& appState);
    void update(float dt, RenderWindow& window);
    void draw(RenderWindow& window);

    ~DoctorScreen();
};