#pragma once
#include <SFML/Graphics.hpp>
#include "AppState.h"
#include "UIComponents.h"
#include "STRING.h"

enum class BookingState { None, SearchSpec, SelectDoc, EnterDate, EnterTime };

class PatientScreen {
private:
    AppState* state;
    sf::Text* title;
    sf::Text* balanceText;
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
    ~PatientScreen();
    void init(const sf::Font& font, AppState& appState);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window, AppState& appState);
    void update(float dt, sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
};