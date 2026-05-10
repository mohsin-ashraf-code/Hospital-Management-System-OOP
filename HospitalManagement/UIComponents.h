#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include "STRING.h"

class Button {
private:
    sf::RectangleShape* shape;
    sf::Text* text;
    sf::Color idleColor;
    sf::Color hoverColor;

public:
    Button();
    Button(sf::Vector2f size, sf::Vector2f position, const char* label, const sf::Font& font, sf::Color idle, sf::Color hover);
    ~Button();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window, std::function<void()> onClick);
    void update(const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
};

class InputForm {
private:
    sf::RectangleShape* background;
    sf::Text* formTitle;
    sf::Text** labels;
    sf::RectangleShape** inputBoxes;
    sf::Text** inputTexts;
    STRING* fieldValues; // Using your custom STRING class!
    int fieldCount;
    int activeField;
    Button* submitBtn;
    Button* cancelBtn;
    bool active;

public:
    InputForm();
    ~InputForm();
    void init(const sf::Font& font, const char* title, const char** fields, int count);
    void show();
    void hide();
    bool isActive() const;

    // Callback passes a dynamic array of const char* back to the screen
    void handleEvent(const sf::Event& event, sf::RenderWindow& window, std::function<void(const char**)> onSubmit);
    void update(const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
};

class DataViewer {
private:
    sf::RectangleShape* background;
    sf::Text* titleText;
    sf::Text* contentText;
    Button* closeBtn;
    bool active;

public:
    DataViewer();
    ~DataViewer();
    void init(const sf::Font& font, const char* title);

    // Crucial fix: explicitly taking sf::String for SFML rendering
    void show(sf::String text);
    void hide();
    bool isActive() const;
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void update(const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
};