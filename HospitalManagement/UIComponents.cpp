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

    // Aesthetic upgrade: Text size automatically scales with button height (approx 42%)
    unsigned int scaledCharSize = static_cast<unsigned int>(size.y * 0.42f);
    text->setCharacterSize(scaledCharSize);
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

    float formWidth = 600.f;
    float formHeight = 150.f + (count * 75.f);

    // Dynamically calculate centering offsets on a 1920x1080 projection
    float startX = (1920.f - formWidth) / 2.0f;
    float startY = (1080.f - formHeight) / 2.0f;

    background = new sf::RectangleShape({ formWidth, formHeight });
    background->setPosition({ startX, startY });
    background->setFillColor(sf::Color(30, 30, 30, 245));
    background->setOutlineThickness(2.f);
    background->setOutlineColor(sf::Color::Cyan);

    formTitle = new sf::Text(font);
    formTitle->setString(title);
    formTitle->setCharacterSize(24);
    formTitle->setFillColor(sf::Color::Cyan);
    formTitle->setPosition({ startX + 50.f, startY + 40.f });

    labels = new sf::Text * [count];
    inputBoxes = new sf::RectangleShape * [count];
    inputTexts = new sf::Text * [count];
    fieldValues = new STRING[count];

    for (int i = 0; i < count; i++) {
        *(labels + i) = new sf::Text(font);
        (*(labels + i))->setString(*(fields + i));
        (*(labels + i))->setCharacterSize(16);
        (*(labels + i))->setFillColor(sf::Color::White);
        (*(labels + i))->setPosition({ startX + 50.f, startY + 95.f + (i * 75.f) });

        *(inputBoxes + i) = new sf::RectangleShape({ 500.f, 35.f });
        (*(inputBoxes + i))->setPosition({ startX + 50.f, startY + 120.f + (i * 75.f) });
        (*(inputBoxes + i))->setFillColor(sf::Color::Black);
        (*(inputBoxes + i))->setOutlineThickness(1.f);
        (*(inputBoxes + i))->setOutlineColor(sf::Color(100, 100, 100));

        *(inputTexts + i) = new sf::Text(font);
        (*(inputTexts + i))->setCharacterSize(16);
        (*(inputTexts + i))->setFillColor(sf::Color::Green);
        (*(inputTexts + i))->setPosition({ startX + 60.f, startY + 125.f + (i * 75.f) });

        (*(fieldValues + i)).clear();
    }

    float btnY = startY + 150.f + (count * 75.f) - 60.f;
    submitBtn = new Button({ 200.f, 45.f }, { startX + 50.f, btnY }, "Submit", font, sf::Color(0, 150, 0), sf::Color(0, 200, 0));
    cancelBtn = new Button({ 200.f, 45.f }, { startX + 350.f, btnY }, "Cancel", font, sf::Color(150, 0, 0), sf::Color(200, 0, 0));
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
        if (textEvent->unicode == 8) {
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
        else if (textEvent->unicode == 9) {
            (*(inputBoxes + activeField))->setOutlineColor(sf::Color(100, 100, 100));
            activeField = (activeField + 1) % fieldCount;
            (*(inputBoxes + activeField))->setOutlineColor(sf::Color::Cyan);
        }
        else if (textEvent->unicode == 13) {
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

    float width = 750.f;
    float height = 550.f;
    float startX = (1920.f - width) / 2.0f;
    float startY = (1080.f - height) / 2.0f;

    background = new sf::RectangleShape({ width, height });
    background->setPosition({ startX, startY });
    background->setFillColor(sf::Color(20, 20, 20, 250));
    background->setOutlineThickness(3.f);
    background->setOutlineColor(sf::Color::Green);

    titleText = new sf::Text(font);
    titleText->setString(title);
    titleText->setCharacterSize(24);
    titleText->setFillColor(sf::Color::Green);
    titleText->setPosition({ startX + 40.f, startY + 40.f });

    contentText = new sf::Text(font);
    contentText->setCharacterSize(16);
    contentText->setFillColor(sf::Color::White);
    contentText->setPosition({ startX + 40.f, startY + 100.f });

    closeBtn = new Button({ 150.f, 45.f }, { startX + (width / 2.0f) - 75.f, startY + height - 70.f }, "Dismiss", font, sf::Color(50, 50, 50), sf::Color(80, 80, 80));
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