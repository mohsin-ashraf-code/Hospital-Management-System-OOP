#include "UIComponents.h"
#include "Validator.h"

// --- BUTTON ---
Button::Button() : shape(nullptr), text(nullptr) {}

Button::Button(sf::Vector2f size, sf::Vector2f position, const char* label, const sf::Font& font, sf::Color idle, sf::Color hover) {
    idleColor = idle;
    hoverColor = hover;

    shape = new sf::RectangleShape(size);
    shape->setPosition(position);
    shape->setFillColor(idle);
    shape->setOutlineThickness(1.f);
    shape->setOutlineColor(sf::Color::White);

    text = new sf::Text(font);
    text->setString(label);
    text->setCharacterSize(18);
    text->setFillColor(sf::Color::White);

    sf::FloatRect textBounds = text->getLocalBounds();
    text->setOrigin({ textBounds.position.x + textBounds.size.x / 2.0f, textBounds.position.y + textBounds.size.y / 2.0f });
    text->setPosition({ position.x + size.x / 2.0f, position.y + size.y / 2.0f });
}

Button::~Button() {
    delete shape;
    delete text;
}

void Button::handleEvent(const sf::Event& event, sf::RenderWindow& window, std::function<void()> onClick) {
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (shape && shape->getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) {
                onClick();
            }
        }
    }
}

void Button::update(const sf::RenderWindow& window) {
    if (!shape) return;
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    if (shape->getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) {
        shape->setFillColor(hoverColor);
    }
    else {
        shape->setFillColor(idleColor);
    }
}

void Button::draw(sf::RenderWindow& window) {
    if (shape) window.draw(*shape);
    if (text) window.draw(*text);
}

// --- INPUT FORM ---
InputForm::InputForm() : background(nullptr), formTitle(nullptr), labels(nullptr), inputBoxes(nullptr), inputTexts(nullptr), fieldValues(nullptr), submitBtn(nullptr), cancelBtn(nullptr), fieldCount(0), activeField(0), active(false) {}

InputForm::~InputForm() {
    delete background;
    delete formTitle;
    if (labels && inputBoxes && inputTexts && fieldValues) {
        for (int i = 0; i < fieldCount; i++) {
            delete* (labels + i);
            delete* (inputBoxes + i);
            delete* (inputTexts + i);
        }
        delete[] labels;
        delete[] inputBoxes;
        delete[] inputTexts;
        delete[] fieldValues;
    }
    delete submitBtn;
    delete cancelBtn;
}

void InputForm::init(const sf::Font& font, const char* title, const char** fields, int count) {
    fieldCount = count;
    activeField = 0;
    active = false;

    background = new sf::RectangleShape({ 600.f, 150.f + (count * 70.f) });
    background->setPosition({ 100.f, 50.f });
    background->setFillColor(sf::Color(30, 30, 30, 245));
    background->setOutlineThickness(2.f);
    background->setOutlineColor(sf::Color::Cyan);

    formTitle = new sf::Text(font);
    formTitle->setString(title);
    formTitle->setCharacterSize(24);
    formTitle->setFillColor(sf::Color::Cyan);
    formTitle->setPosition({ 120.f, 70.f });

    labels = new sf::Text * [count];
    inputBoxes = new sf::RectangleShape * [count];
    inputTexts = new sf::Text * [count];
    fieldValues = new STRING[count];

    for (int i = 0; i < count; i++) {
        *(labels + i) = new sf::Text(font);
        (*(labels + i))->setString(*(fields + i));
        (*(labels + i))->setCharacterSize(16);
        (*(labels + i))->setFillColor(sf::Color::White);
        (*(labels + i))->setPosition({ 120.f, 120.f + (i * 70.f) });

        *(inputBoxes + i) = new sf::RectangleShape({ 400.f, 35.f });
        (*(inputBoxes + i))->setPosition({ 120.f, 145.f + (i * 70.f) });
        (*(inputBoxes + i))->setFillColor(sf::Color::Black);
        (*(inputBoxes + i))->setOutlineThickness(1.f);
        (*(inputBoxes + i))->setOutlineColor(sf::Color(100, 100, 100));

        *(inputTexts + i) = new sf::Text(font);
        (*(inputTexts + i))->setCharacterSize(16);
        (*(inputTexts + i))->setFillColor(sf::Color::Green);
        (*(inputTexts + i))->setPosition({ 130.f, 150.f + (i * 70.f) });

        (*(fieldValues + i)).clear();
    }

    float btnY = 160.f + (count * 70.f);
    submitBtn = new Button({ 180.f, 45.f }, { 120.f, btnY }, "Submit", font, sf::Color(0, 150, 0), sf::Color(0, 200, 0));
    cancelBtn = new Button({ 180.f, 45.f }, { 340.f, btnY }, "Cancel", font, sf::Color(150, 0, 0), sf::Color(200, 0, 0));
}

void InputForm::show() {
    active = true;
    activeField = 0;
    for (int i = 0; i < fieldCount; i++) {
        (*(fieldValues + i)).clear();
        (*(inputTexts + i))->setString("");
        (*(inputBoxes + i))->setOutlineColor(sf::Color(100, 100, 100));
    }
    if (fieldCount > 0) (*(inputBoxes + 0))->setOutlineColor(sf::Color::Cyan);
}

void InputForm::hide() { active = false; }
bool InputForm::isActive() const { return active; }

void InputForm::handleEvent(const sf::Event& event, sf::RenderWindow& window, std::function<void(const char**)> onSubmit) {
    if (!active) return;

    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->code == sf::Keyboard::Key::Escape) {
            hide();
            return;
        }
    }

    cancelBtn->handleEvent(event, window, [&]() { hide(); });

    submitBtn->handleEvent(event, window, [&]() {
        const char** outData = new const char* [fieldCount];
        for (int k = 0; k < fieldCount; k++) *(outData + k) = (*(fieldValues + k)).get();
        onSubmit(outData);
        delete[] outData;
        hide();
        });

    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            for (int i = 0; i < fieldCount; i++) {
                if ((*(inputBoxes + i))->getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) {
                    (*(inputBoxes + activeField))->setOutlineColor(sf::Color(100, 100, 100));
                    activeField = i;
                    (*(inputBoxes + activeField))->setOutlineColor(sf::Color::Cyan);
                }
            }
        }
    }

    if (const auto* textEvent = event.getIf<sf::Event::TextEntered>()) {
        if (textEvent->unicode == 8) { // Manual pointer arithmetic backspace
            const char* current = (*(fieldValues + activeField)).get();
            int len = Validator::myStrLen(current);
            if (len > 0) {
                char* temp = new char[len];
                for (int k = 0; k < len - 1; k++) *(temp + k) = *(current + k);
                *(temp + len - 1) = '\0';
                (*(fieldValues + activeField)).copy(temp);
                (*(inputTexts + activeField))->setString((*(fieldValues + activeField)).get());
                delete[] temp;
            }
        }
        else if (textEvent->unicode == 9) { // Tab
            (*(inputBoxes + activeField))->setOutlineColor(sf::Color(100, 100, 100));
            activeField = (activeField + 1) % fieldCount;
            (*(inputBoxes + activeField))->setOutlineColor(sf::Color::Cyan);
        }
        else if (textEvent->unicode == 13) { // Enter
            const char** outData = new const char* [fieldCount];
            for (int k = 0; k < fieldCount; k++) *(outData + k) = (*(fieldValues + k)).get();
            onSubmit(outData);
            delete[] outData;
            hide();
        }
        else if (textEvent->unicode >= 32 && textEvent->unicode < 127) {
            (*(fieldValues + activeField)).concatenate(static_cast<char>(textEvent->unicode));
            (*(inputTexts + activeField))->setString((*(fieldValues + activeField)).get());
        }
    }
}

void InputForm::update(const sf::RenderWindow& window) {
    if (!active) return;
    submitBtn->update(window);
    cancelBtn->update(window);
}

void InputForm::draw(sf::RenderWindow& window) {
    if (!active) return;
    window.draw(*background);
    window.draw(*formTitle);
    for (int i = 0; i < fieldCount; i++) {
        window.draw(*(*(labels + i)));
        window.draw(*(*(inputBoxes + i)));
        window.draw(*(*(inputTexts + i)));
    }
    submitBtn->draw(window);
    cancelBtn->draw(window);
}

// --- DATA VIEWER ---
DataViewer::DataViewer() : background(nullptr), titleText(nullptr), contentText(nullptr), closeBtn(nullptr), active(false) {}

DataViewer::~DataViewer() {
    delete background;
    delete titleText;
    delete contentText;
    delete closeBtn;
}

void DataViewer::init(const sf::Font& font, const char* title) {
    active = false;
    background = new sf::RectangleShape({ 650.f, 500.f });
    background->setPosition({ 75.f, 50.f });
    background->setFillColor(sf::Color(20, 20, 20, 250));
    background->setOutlineThickness(3.f);
    background->setOutlineColor(sf::Color::Green);

    titleText = new sf::Text(font);
    titleText->setString(title);
    titleText->setCharacterSize(22);
    titleText->setFillColor(sf::Color::Green);
    titleText->setPosition({ 95.f, 75.f });

    contentText = new sf::Text(font);
    contentText->setCharacterSize(14);
    contentText->setFillColor(sf::Color::White);
    contentText->setPosition({ 95.f, 120.f });

    closeBtn = new Button({ 120.f, 40.f }, { 340.f, 490.f }, "Dismiss", font, sf::Color(50, 50, 50), sf::Color(80, 80, 80));
}

void DataViewer::show(sf::String text) {
    active = true;
    contentText->setString(text);
}

void DataViewer::hide() { active = false; }
bool DataViewer::isActive() const { return active; }

void DataViewer::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (!active) return;
    if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
        if (keyEvent->code == sf::Keyboard::Key::Escape) {
            hide();
            return;
        }
    }
    closeBtn->handleEvent(event, window, [&]() { hide(); });
}

void DataViewer::update(const sf::RenderWindow& window) {
    if (!active) return;
    closeBtn->update(window);
}

void DataViewer::draw(sf::RenderWindow& window) {
    if (!active) return;
    window.draw(*background);
    window.draw(*titleText);
    window.draw(*contentText);
    closeBtn->draw(window);
}