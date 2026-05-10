#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"
#include "STRING.h"
using namespace sf;

enum class BookingState { None, SearchSpec, SelectDoc, EnterDate, EnterTime };

class PatientScreen 
{
private:
    AppState* state;
    Text* title;
    Text* balanceText;
    Button** buttons;
    BookingState bookState;
    int selectedDocId;
    STRING selectedDate;
    int sessionalTopUpAttempts;

    InputForm topUpForm;
    InputForm payBillForm;
    InputForm cancelApptForm;

    InputForm searchSpecForm;
    InputForm selectDocForm;
    InputForm bookDateForm;
    InputForm bookTimeForm;

    DataViewer dataViewer;

    bool manualCaseInsensitiveMatch(const char* str1, const char* str2);
    int parseDateToInt(const char* dateStr);

public:
    PatientScreen();
   
    void init(const Font& font, AppState& appState);
    void handleEvent(const Event& event, RenderWindow& window, AppState& appState);
    void update(float dt, RenderWindow& window);
    void draw(RenderWindow& window);

    ~PatientScreen();
};