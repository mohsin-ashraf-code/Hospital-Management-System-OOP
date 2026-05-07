#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <functional>
#include "utility.h"

// --- BUTTON COMPONENT ---
class Button {
private:
    sf::RectangleShape rect;
    sf::Text label;
    sf::Color idleColor;
    sf::Color hoverColor;
    bool isHovered;

public:
    Button(sf::Vector2f size, sf::Vector2f position, const char* text, const sf::Font& font, sf::Color idle, sf::Color hover);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window, std::function<void()> onClick);
    void update(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;
};

// --- TEXTBOX COMPONENT ---
class TextBox {
private:
    sf::RectangleShape rect;
    sf::Text textDisplay;
    sf::Text placeholderText;
    char buffer[100];
    int cursor;
    int maxLen;
    bool isFocused;
    bool isPassword;

    void updateDisplay();

public:
    TextBox(sf::Vector2f size, sf::Vector2f position, const char* placeholder, const sf::Font& font, int maxLen, bool isPassword = false);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;
    const char* getText() const;
    void clear();
};

// --- LIST VIEW COMPONENT ---
class ListView {
private:
    sf::RectangleShape rect;
    sf::Text title;
    sf::Text content;

public:
    ListView(sf::Vector2f size, sf::Vector2f position, const char* titleStr, const sf::Font& font);
    void setContent(const char* text); // STRICTLY const char*
    void draw(sf::RenderWindow& window) const;
};

// --- MESSAGE BOX COMPONENT ---
class MsgBox {
private:
    sf::RectangleShape overlay;
    sf::RectangleShape box;
    sf::Text titleDisplay;
    sf::Text messageDisplay;
    Button* okButton;
    bool isVisible;

public:
    MsgBox(const sf::Font& font);
    ~MsgBox();

    void show(const char* title, const char* message);
    void handleEvent(const sf::Event& event, const sf::RenderWindow& window);
    void update(sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;

    bool getVisible() const;
};