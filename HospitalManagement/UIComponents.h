#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include "STRING.h"
using namespace sf;

class Button 
{
private:
    RectangleShape* shape;
    Text* text;
    Color idleColor;
    Color hoverColor;

public:
    Button();
    Button(Vector2f size, Vector2f position, const char* label, const Font& font, Color idle, Color hover);
    ~Button();
    void handleEvent(const Event& event, RenderWindow& window, std::function<void()> onClick);
    void update(const RenderWindow& window);
    void draw(RenderWindow& window);
};

class InputForm 
{
private:
    RectangleShape* background;
    Text* formTitle;
    Text** labels;
    RectangleShape** inputBoxes;
    Text** inputTexts;
    STRING* fieldValues;
    int fieldCount;
    int activeField;
    Button* submitBtn;
    Button* cancelBtn;
    bool active;

public:
    InputForm();
    ~InputForm();
    void init(const Font& font, const char* title, const char** fields, int count);
    void show();
    void hide();
    bool isActive() const;

    void handleEvent(const Event& event, RenderWindow& window, std::function<void(const char**)> onSubmit);
    void update(const RenderWindow& window);
    void draw(RenderWindow& window);
};

class DataViewer 
{
private:
    RectangleShape* background;
    Text* titleText;
    Text* contentText;
    Button* closeBtn;
    bool active;

public:
    DataViewer();
    ~DataViewer();
    void init(const Font& font, const char* title);

    void show(String text);
    void hide();
    bool isActive() const;
    void handleEvent(const Event& event, RenderWindow& window);
    void update(const RenderWindow& window);
    void draw(RenderWindow& window);
};