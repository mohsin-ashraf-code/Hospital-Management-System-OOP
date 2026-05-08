#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <functional>
#include "utility.h"
using namespace sf;

#define MAX_FORM_FIELDS 6


// --- BUTTON COMPONENT ---
class Button {
private:
     RectangleShape rect;
     Text label;
     Color idleColor;
     Color hoverColor;
    bool isHovered;

public:
    Button( Vector2f size,  Vector2f position, const char* text, const  Font& font,  Color idle,  Color hover);
    void handleEvent(const  Event& event, const  RenderWindow& window, std::function<void()> onClick);
    void update( RenderWindow& window);
    void draw( RenderWindow& window) const;
};

// --- TEXTBOX COMPONENT ---
class TextBox {
private:
     RectangleShape rect;
     Text textDisplay;
     Text placeholderText;
    char buffer[100];
    int cursor;
    int maxLen;
    bool isFocused;
    bool isPassword;

    void updateDisplay();

public:
    TextBox( Vector2f size,  Vector2f position, const char* placeholder, const  Font& font, int maxLen, bool isPassword = false);
    void handleEvent(const  Event& event, const  RenderWindow& window);
    void update( RenderWindow& window);
    void draw( RenderWindow& window) const;
    const char* getText() const;
    void clear();
};

// --- LIST VIEW COMPONENT ---
class ListView {
private:
     RectangleShape rect;
     Text title;
     Text content;

public:
    ListView( Vector2f size,  Vector2f position, const char* titleStr, const  Font& font);
    void setContent(const char* text); // STRICTLY const char*
    void draw( RenderWindow& window) const;
};

// --- MESSAGE BOX COMPONENT ---
class MsgBox {
private:
     RectangleShape overlay;
     RectangleShape box;
     Text titleDisplay;
     Text messageDisplay;
    Button* okButton;
    bool isVisible;

public:
    MsgBox(const  Font& font);
    ~MsgBox();

    void show(const char* title, const char* message);
    void handleEvent(const  Event& event, const  RenderWindow& window);
    void update( RenderWindow& window);
    void draw( RenderWindow& window) const;

    bool getVisible() const;
};

//INPUT FORMS

class InputForm {
private:
     RectangleShape overlay;
     RectangleShape panel;
     Text* titleText; // <--- FIX: CHANGED TO POINTER

     Text* labels[MAX_FORM_FIELDS];
    TextBox* inputs[MAX_FORM_FIELDS];

    Button* btnSubmit;
    Button* btnCancel;

    int fieldCount;
    bool active;

public:
    InputForm();
    ~InputForm();

    void init(const  Font& font, const char* title, const char* fieldNames[], int count);
    void handleEvent(const  Event& event,  RenderWindow& window, std::function<void(const char**)> onSubmit);
    void update( RenderWindow& window);
    void draw( RenderWindow& window);

    void show();
    void hide();
    bool isActive() const;
    void clear();
};

class DataViewer {
private:
     RectangleShape overlay;
     RectangleShape panel;
     Text* titleText;
     Text* contentText;
    Button* btnClose;
    bool active;

public:
    DataViewer();
    ~DataViewer();

    void init(const  Font& font, const char* title);
    void handleEvent(const  Event& event,  RenderWindow& window);
    void update( RenderWindow& window);
    void draw( RenderWindow& window);

    // Pass in a massive stitched-together string of all the data!
    void show(const char* content);
    void hide();
    bool isActive() const;
};