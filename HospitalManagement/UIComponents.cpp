#include "UIComponents.h"
using namespace sf;
// ==========================================
// BUTTON IMPLEMENTATION
// ==========================================

Button::Button( Vector2f size,  Vector2f position, const char* text, const  Font& font,  Color idle,  Color hover)
    : label(font), idleColor(idle), hoverColor(hover), isHovered(false) {

    rect.setSize(size);
    rect.setPosition(position);
    rect.setFillColor(idleColor);
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor( Color(100, 100, 100));

    label.setString(text);
    label.setCharacterSize(24);
    label.setFillColor( Color::White);

     FloatRect textBounds = label.getLocalBounds();
    label.setPosition({
        position.x + (size.x / 2.f) - (textBounds.size.x / 2.f) - textBounds.position.x,
        position.y + (size.y / 2.f) - (textBounds.size.y / 2.f) - textBounds.position.y
        });
}

void Button::update( RenderWindow& window) {
     Vector2i pixelPos =  Mouse::getPosition(window);
     Vector2f worldPos = window.mapPixelToCoords(pixelPos);

    isHovered = rect.getGlobalBounds().contains(worldPos);
    rect.setFillColor(isHovered ? hoverColor : idleColor);
}

void Button::handleEvent(const  Event& event, const  RenderWindow& window, std::function<void()> onClick) {
    if (const auto* mouseEvent = event.getIf< Event::MouseButtonReleased>()) {
        if (mouseEvent->button ==  Mouse::Button::Left) {
             Vector2f worldPos = window.mapPixelToCoords(mouseEvent->position);
            if (rect.getGlobalBounds().contains(worldPos)) {
                onClick();
            }
        }
    }
}

void Button::draw( RenderWindow& window) const {
    window.draw(rect);
    window.draw(label);
}

// ==========================================
// TEXTBOX IMPLEMENTATION
// ==========================================

TextBox::TextBox( Vector2f size,  Vector2f position, const char* placeholder, const  Font& font, int maxLen, bool isPassword)
    : textDisplay(font), placeholderText(font), cursor(0), maxLen(maxLen), isFocused(false), isPassword(isPassword) {

    buffer = new char[maxLen];
    buffer[0] = '\0';

    rect.setSize(size);
    rect.setPosition(position);
    rect.setFillColor( Color::White);
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor( Color(200, 200, 200));

    textDisplay.setCharacterSize(20);
    textDisplay.setFillColor( Color::Black);
    textDisplay.setPosition({ position.x + 10.f, position.y + (size.y / 2.f) - 12.f });

    placeholderText.setString(placeholder);
    placeholderText.setCharacterSize(20);
    placeholderText.setFillColor( Color(150, 150, 150));
    placeholderText.setPosition({ position.x + 10.f, position.y + (size.y / 2.f) - 12.f });
}

TextBox::~TextBox() {
    delete[] buffer;
}

void TextBox::update( RenderWindow& window) {
     Vector2i pixelPos =  Mouse::getPosition(window);
     Vector2f worldPos = window.mapPixelToCoords(pixelPos);

    if (rect.getGlobalBounds().contains(worldPos)) {
        window.setMouseCursor( Cursor::createFromSystem( Cursor::Type::Text).value());
    }
    else {
        window.setMouseCursor( Cursor::createFromSystem( Cursor::Type::Arrow).value());
    }

    rect.setOutlineColor(isFocused ?  Color(0, 120, 215) :  Color(200, 200, 200));
}

void TextBox::handleEvent(const  Event& event, const  RenderWindow& window) {
    if (const auto* mouseEvent = event.getIf< Event::MouseButtonPressed>()) {
        if (mouseEvent->button ==  Mouse::Button::Left) {
             Vector2f worldPos = window.mapPixelToCoords(mouseEvent->position);
            isFocused = rect.getGlobalBounds().contains(worldPos);
        }
    }

    if (isFocused) {
        if (const auto* textEvent = event.getIf< Event::TextEntered>()) {
            uint32_t unicode = textEvent->unicode;

            if (unicode == '\b' && cursor > 0) {
                cursor--;
                buffer[cursor] = '\0';
            }
            else if (unicode >= 32 && unicode < 128 && cursor < maxLen - 1) {
                buffer[cursor] = static_cast<char>(unicode);
                cursor++;
                buffer[cursor] = '\0';
            }
            updateDisplay();
        }
    }
}

void TextBox::updateDisplay() {
    if (isPassword) {
        char* stars = new char[maxLen];
        for (int i = 0; i < cursor; i++) stars[i] = '*';
        stars[cursor] = '\0';
        textDisplay.setString(stars);
        delete[] stars;
    }
    else {
        textDisplay.setString(buffer);
    }
}

void TextBox::draw( RenderWindow& window) const {
    window.draw(rect);
    if (cursor == 0 && !isFocused) {
        window.draw(placeholderText);
    }
    else {
        window.draw(textDisplay);
    }
}

const char* TextBox::getText() const {
    return buffer;
}

void TextBox::clear() {
    cursor = 0;
    buffer[0] = '\0';
    updateDisplay();
}

// ==========================================
// LIST VIEW IMPLEMENTATION
// ==========================================

ListView::ListView( Vector2f size,  Vector2f position, const char* titleStr, const  Font& font)
    : title(font), content(font) {

    rect.setSize(size);
    rect.setPosition(position);
    rect.setFillColor( Color(250, 250, 250));
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor( Color(150, 150, 150));

    title.setString(titleStr);
    title.setCharacterSize(24);
    title.setFillColor( Color(0, 51, 102));
    title.setPosition({ position.x + 10.f, position.y + 10.f });

    content.setCharacterSize(18);
    content.setFillColor( Color::Black);
    content.setPosition({ position.x + 10.f, position.y + 50.f });
}

void ListView::setContent(const char* text) {
    content.setString(text);
}

void ListView::draw( RenderWindow& window) const {
    window.draw(rect);
    window.draw(title);
    window.draw(content);
}

// ==========================================
// MESSAGE BOX IMPLEMENTATION
// ==========================================

MsgBox::MsgBox(const  Font& font)
    : titleDisplay(font), messageDisplay(font), isVisible(false) {

    overlay.setSize({ 1920.f, 1080.f });
    overlay.setFillColor( Color(0, 0, 0, 150));

    box.setSize({ 500.f, 250.f });
    box.setPosition({ (1920.f - 500.f) / 2.f, (1080.f - 250.f) / 2.f });
    box.setFillColor( Color::White);
    box.setOutlineThickness(3.f);
    box.setOutlineColor( Color(0, 51, 102));

    titleDisplay.setCharacterSize(24);
    titleDisplay.setFillColor( Color(0, 51, 102));

    messageDisplay.setCharacterSize(20);
    messageDisplay.setFillColor( Color::Black);

    okButton = new Button({ 150.f, 50.f },
        { box.getPosition().x + 175.f, box.getPosition().y + 180.f },
        "OK", font,  Color(0, 120, 215),  Color(0, 80, 160));
}

MsgBox::~MsgBox() {
    delete okButton;
}

void MsgBox::show(const char* title, const char* message) {
    titleDisplay.setString(title);
    messageDisplay.setString(message);

     FloatRect titleBounds = titleDisplay.getLocalBounds();
    titleDisplay.setPosition({
        box.getPosition().x + (500.f / 2.f) - (titleBounds.size.x / 2.f),
        box.getPosition().y + 20.f
        });

    messageDisplay.setPosition({ box.getPosition().x + 20.f, box.getPosition().y + 80.f });

    isVisible = true;
}

bool MsgBox::getVisible() const {
    return isVisible;
}

void MsgBox::update( RenderWindow& window) {
    if (isVisible) {
        okButton->update(window);
    }
}

void MsgBox::handleEvent(const  Event& event, const  RenderWindow& window) {
    if (isVisible) {
        okButton->handleEvent(event, window, [&]() {
            isVisible = false;
            });
    }
}

void MsgBox::draw( RenderWindow& window) const {
    if (isVisible) {
        window.draw(overlay);
        window.draw(box);
        window.draw(titleDisplay);
        window.draw(messageDisplay);
        okButton->draw(window);
    }
}

// ==========================================
// INPUT FORM IMPLEMENTATION
// ==========================================

InputForm::InputForm() : fieldCount(0), active(false), titleText(nullptr), btnSubmit(nullptr), btnCancel(nullptr), maxFields(6) {
    labels = new Text * [maxFields];
    inputs = new TextBox * [maxFields];
    for (int i = 0; i < maxFields; i++) {
        labels[i] = nullptr;
        inputs[i] = nullptr;
    }
}

InputForm::~InputForm() {
    delete titleText; // <--- FIX: CLEANUP POINTER
    for (int i = 0; i < maxFields; i++) {
        delete labels[i];
        delete inputs[i];
    }
    delete[] labels;
    delete[] inputs;
    delete btnSubmit;
    delete btnCancel;
}

void InputForm::init(const  Font& font, const char* title, const char* fieldNames[], int count) {
    fieldCount = count;
    if (fieldCount > maxFields) fieldCount = maxFields;

    overlay.setSize({ 1920.f, 1080.f });
    overlay.setFillColor( Color(0, 0, 0, 150));

    float panelHeight = 200.f + (count * 80.f);
    panel.setSize({ 600.f, panelHeight });
    panel.setFillColor( Color(240, 240, 240));
    panel.setPosition({ 1920.f / 2.f - 300.f, 1080.f / 2.f - panelHeight / 2.f });
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor( Color(100, 100, 100));

    // <--- FIX: ALLOCATE POINTER DYNAMICALLY --->
    titleText = new  Text(font);
    titleText->setString(title);
    titleText->setCharacterSize(30);
    titleText->setFillColor( Color(0, 51, 102));
    titleText->setPosition({ panel.getPosition().x + 20.f, panel.getPosition().y + 20.f });

    float startY = panel.getPosition().y + 80.f;

    for (int i = 0; i < fieldCount; i++) {
        labels[i] = new  Text(font);
        labels[i]->setString(fieldNames[i]);
        labels[i]->setCharacterSize(20);
        labels[i]->setFillColor( Color::Black);
        labels[i]->setPosition({ panel.getPosition().x + 20.f, startY + (i * 80.f) });

        bool isPass = false;
        if (myStrLen(fieldNames[i]) >= 8 && fieldNames[i][0] == 'P' && fieldNames[i][1] == 'a') isPass = true;

        inputs[i] = new TextBox({ 350.f, 40.f }, { panel.getPosition().x + 200.f, startY + (i * 80.f) - 5.f }, "", font, 49, isPass);
    }

    float btnY = startY + (fieldCount * 80.f);
    btnSubmit = new Button({ 200.f, 50.f }, { panel.getPosition().x + 50.f, btnY }, "Submit", font,  Color(0, 150, 0),  Color(0, 100, 0));
    btnCancel = new Button({ 200.f, 50.f }, { panel.getPosition().x + 350.f, btnY }, "Cancel", font,  Color(200, 50, 50),  Color(150, 0, 0));
}

void InputForm::handleEvent(const  Event& event,  RenderWindow& window, std::function<void(const char**)> onSubmit) {
    if (!active) return;

    for (int i = 0; i < fieldCount; i++) inputs[i]->handleEvent(event, window);

    btnCancel->handleEvent(event, window, [&]() {
        hide();
        });

    btnSubmit->handleEvent(event, window, [&]() {
        const char** results = new const char* [fieldCount];
        for (int i = 0; i < fieldCount; i++) {
            results[i] = inputs[i]->getText();
        }
        onSubmit(results);
        delete[] results;
        hide();
        });
}

void InputForm::update( RenderWindow& window) {
    if (!active) return;
    for (int i = 0; i < fieldCount; i++) inputs[i]->update(window);
    btnSubmit->update(window);
    btnCancel->update(window);
}

void InputForm::draw( RenderWindow& window) {
    if (!active) return;
    window.draw(overlay);
    window.draw(panel);

    // <--- FIX: DEREFERENCE POINTER --->
    if (titleText) window.draw(*titleText);

    for (int i = 0; i < fieldCount; i++) {
        window.draw(*labels[i]);
        inputs[i]->draw(window);
    }
    btnSubmit->draw(window);
    btnCancel->draw(window);
}

void InputForm::show() { active = true; clear(); }
void InputForm::hide() { active = false; }
bool InputForm::isActive() const { return active; }
void InputForm::clear() {
    for (int i = 0; i < fieldCount; i++) inputs[i]->clear();
}

// ==========================================
// DATA VIEWER IMPLEMENTATION
// ==========================================

DataViewer::DataViewer() : active(false), titleText(nullptr), contentText(nullptr), btnClose(nullptr) {}

DataViewer::~DataViewer() {
    delete titleText;
    delete contentText;
    delete btnClose;
}

void DataViewer::init(const  Font& font, const char* title) {
    overlay.setSize({ 1920.f, 1080.f });
    overlay.setFillColor( Color(0, 0, 0, 180)); // Darker background

    panel.setSize({ 800.f, 600.f });
    panel.setFillColor( Color(250, 250, 250));
    panel.setPosition({ 1920.f / 2.f - 400.f, 1080.f / 2.f - 300.f });
    panel.setOutlineThickness(3.f);
    panel.setOutlineColor( Color(50, 50, 50));

    titleText = new  Text(font);
    titleText->setString(title);
    titleText->setCharacterSize(35);
    titleText->setFillColor( Color(0, 51, 102));
    titleText->setPosition({ panel.getPosition().x + 30.f, panel.getPosition().y + 20.f });

    contentText = new  Text(font);
    contentText->setCharacterSize(20);
    contentText->setFillColor( Color::Black);
    contentText->setPosition({ panel.getPosition().x + 30.f, panel.getPosition().y + 90.f });
    // Adjust line spacing so lists look clean
    contentText->setLineSpacing(1.5f);

    btnClose = new Button({ 200.f, 50.f }, { panel.getPosition().x + 300.f, panel.getPosition().y + 520.f }, "Close", font,  Color(200, 50, 50),  Color(150, 0, 0));
}

void DataViewer::handleEvent(const  Event& event,  RenderWindow& window) {
    if (!active) return;
    btnClose->handleEvent(event, window, [&]() {
        hide();
        });
}

void DataViewer::update( RenderWindow& window) {
    if (!active) return;
    btnClose->update(window);
}

void DataViewer::draw( RenderWindow& window) {
    if (!active) return;
    window.draw(overlay);
    window.draw(panel);
    if (titleText) window.draw(*titleText);
    if (contentText) window.draw(*contentText);
    btnClose->draw(window);
}

void DataViewer::show(const char* content) {
    if (contentText) contentText->setString(content);
    active = true;
}

void DataViewer::hide() {
    active = false;
}

bool DataViewer::isActive() const {
    return active;
}