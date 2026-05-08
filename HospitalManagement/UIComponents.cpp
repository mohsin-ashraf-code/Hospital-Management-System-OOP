#include "UIComponents.h"

// ==========================================
// BUTTON IMPLEMENTATION
// ==========================================

Button::Button(sf::Vector2f size, sf::Vector2f position, const char* text, const sf::Font& font, sf::Color idle, sf::Color hover)
    : label(font), idleColor(idle), hoverColor(hover), isHovered(false) {

    rect.setSize(size);
    rect.setPosition(position);
    rect.setFillColor(idleColor);
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(sf::Color(100, 100, 100));

    label.setString(text);
    label.setCharacterSize(24);
    label.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = label.getLocalBounds();
    label.setPosition({
        position.x + (size.x / 2.f) - (textBounds.size.x / 2.f) - textBounds.position.x,
        position.y + (size.y / 2.f) - (textBounds.size.y / 2.f) - textBounds.position.y
        });
}

void Button::update(sf::RenderWindow& window) {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

    isHovered = rect.getGlobalBounds().contains(worldPos);
    rect.setFillColor(isHovered ? hoverColor : idleColor);
}

void Button::handleEvent(const sf::Event& event, const sf::RenderWindow& window, std::function<void()> onClick) {
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f worldPos = window.mapPixelToCoords(mouseEvent->position);
            if (rect.getGlobalBounds().contains(worldPos)) {
                onClick();
            }
        }
    }
}

void Button::draw(sf::RenderWindow& window) const {
    window.draw(rect);
    window.draw(label);
}

// ==========================================
// TEXTBOX IMPLEMENTATION
// ==========================================

TextBox::TextBox(sf::Vector2f size, sf::Vector2f position, const char* placeholder, const sf::Font& font, int maxLen, bool isPassword)
    : textDisplay(font), placeholderText(font), cursor(0), maxLen(maxLen), isFocused(false), isPassword(isPassword) {

    buffer[0] = '\0';

    rect.setSize(size);
    rect.setPosition(position);
    rect.setFillColor(sf::Color::White);
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(sf::Color(200, 200, 200));

    textDisplay.setCharacterSize(20);
    textDisplay.setFillColor(sf::Color::Black);
    textDisplay.setPosition({ position.x + 10.f, position.y + (size.y / 2.f) - 12.f });

    placeholderText.setString(placeholder);
    placeholderText.setCharacterSize(20);
    placeholderText.setFillColor(sf::Color(150, 150, 150));
    placeholderText.setPosition({ position.x + 10.f, position.y + (size.y / 2.f) - 12.f });
}

void TextBox::update(sf::RenderWindow& window) {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos);

    if (rect.getGlobalBounds().contains(worldPos)) {
        window.setMouseCursor(sf::Cursor::createFromSystem(sf::Cursor::Type::Text).value());
    }
    else {
        window.setMouseCursor(sf::Cursor::createFromSystem(sf::Cursor::Type::Arrow).value());
    }

    rect.setOutlineColor(isFocused ? sf::Color(0, 120, 215) : sf::Color(200, 200, 200));
}

void TextBox::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f worldPos = window.mapPixelToCoords(mouseEvent->position);
            isFocused = rect.getGlobalBounds().contains(worldPos);
        }
    }

    if (isFocused) {
        if (const auto* textEvent = event.getIf<sf::Event::TextEntered>()) {
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
        char stars[100];
        for (int i = 0; i < cursor; i++) stars[i] = '*';
        stars[cursor] = '\0';
        textDisplay.setString(stars);
    }
    else {
        textDisplay.setString(buffer);
    }
}

void TextBox::draw(sf::RenderWindow& window) const {
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

ListView::ListView(sf::Vector2f size, sf::Vector2f position, const char* titleStr, const sf::Font& font)
    : title(font), content(font) {

    rect.setSize(size);
    rect.setPosition(position);
    rect.setFillColor(sf::Color(250, 250, 250));
    rect.setOutlineThickness(2.f);
    rect.setOutlineColor(sf::Color(150, 150, 150));

    title.setString(titleStr);
    title.setCharacterSize(24);
    title.setFillColor(sf::Color(0, 51, 102));
    title.setPosition({ position.x + 10.f, position.y + 10.f });

    content.setCharacterSize(18);
    content.setFillColor(sf::Color::Black);
    content.setPosition({ position.x + 10.f, position.y + 50.f });
}

void ListView::setContent(const char* text) {
    content.setString(text);
}

void ListView::draw(sf::RenderWindow& window) const {
    window.draw(rect);
    window.draw(title);
    window.draw(content);
}

// ==========================================
// MESSAGE BOX IMPLEMENTATION
// ==========================================

MsgBox::MsgBox(const sf::Font& font)
    : titleDisplay(font), messageDisplay(font), isVisible(false) {

    overlay.setSize({ 1920.f, 1080.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 150));

    box.setSize({ 500.f, 250.f });
    box.setPosition({ (1920.f - 500.f) / 2.f, (1080.f - 250.f) / 2.f });
    box.setFillColor(sf::Color::White);
    box.setOutlineThickness(3.f);
    box.setOutlineColor(sf::Color(0, 51, 102));

    titleDisplay.setCharacterSize(24);
    titleDisplay.setFillColor(sf::Color(0, 51, 102));

    messageDisplay.setCharacterSize(20);
    messageDisplay.setFillColor(sf::Color::Black);

    okButton = new Button({ 150.f, 50.f },
        { box.getPosition().x + 175.f, box.getPosition().y + 180.f },
        "OK", font, sf::Color(0, 120, 215), sf::Color(0, 80, 160));
}

MsgBox::~MsgBox() {
    delete okButton;
}

void MsgBox::show(const char* title, const char* message) {
    titleDisplay.setString(title);
    messageDisplay.setString(message);

    sf::FloatRect titleBounds = titleDisplay.getLocalBounds();
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

void MsgBox::update(sf::RenderWindow& window) {
    if (isVisible) {
        okButton->update(window);
    }
}

void MsgBox::handleEvent(const sf::Event& event, const sf::RenderWindow& window) {
    if (isVisible) {
        okButton->handleEvent(event, window, [&]() {
            isVisible = false;
            });
    }
}

void MsgBox::draw(sf::RenderWindow& window) const {
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

InputForm::InputForm() : fieldCount(0), active(false), titleText(nullptr), btnSubmit(nullptr), btnCancel(nullptr) {
    for (int i = 0; i < MAX_FORM_FIELDS; i++) {
        labels[i] = nullptr;
        inputs[i] = nullptr;
    }
}

InputForm::~InputForm() {
    delete titleText; // <--- FIX: CLEANUP POINTER
    for (int i = 0; i < MAX_FORM_FIELDS; i++) {
        delete labels[i];
        delete inputs[i];
    }
    delete btnSubmit;
    delete btnCancel;
}

void InputForm::init(const sf::Font& font, const char* title, const char* fieldNames[], int count) {
    fieldCount = count;
    if (fieldCount > MAX_FORM_FIELDS) fieldCount = MAX_FORM_FIELDS;

    overlay.setSize({ 1920.f, 1080.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 150));

    float panelHeight = 200.f + (count * 80.f);
    panel.setSize({ 600.f, panelHeight });
    panel.setFillColor(sf::Color(240, 240, 240));
    panel.setPosition({ 1920.f / 2.f - 300.f, 1080.f / 2.f - panelHeight / 2.f });
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(100, 100, 100));

    // <--- FIX: ALLOCATE POINTER DYNAMICALLY --->
    titleText = new sf::Text(font);
    titleText->setString(title);
    titleText->setCharacterSize(30);
    titleText->setFillColor(sf::Color(0, 51, 102));
    titleText->setPosition({ panel.getPosition().x + 20.f, panel.getPosition().y + 20.f });

    float startY = panel.getPosition().y + 80.f;

    for (int i = 0; i < fieldCount; i++) {
        labels[i] = new sf::Text(font);
        labels[i]->setString(fieldNames[i]);
        labels[i]->setCharacterSize(20);
        labels[i]->setFillColor(sf::Color::Black);
        labels[i]->setPosition({ panel.getPosition().x + 20.f, startY + (i * 80.f) });

        bool isPass = false;
        if (myStrLen(fieldNames[i]) >= 8 && fieldNames[i][0] == 'P' && fieldNames[i][1] == 'a') isPass = true;

        inputs[i] = new TextBox({ 350.f, 40.f }, { panel.getPosition().x + 200.f, startY + (i * 80.f) - 5.f }, "", font, 49, isPass);
    }

    float btnY = startY + (fieldCount * 80.f);
    btnSubmit = new Button({ 200.f, 50.f }, { panel.getPosition().x + 50.f, btnY }, "Submit", font, sf::Color(0, 150, 0), sf::Color(0, 100, 0));
    btnCancel = new Button({ 200.f, 50.f }, { panel.getPosition().x + 350.f, btnY }, "Cancel", font, sf::Color(200, 50, 50), sf::Color(150, 0, 0));
}

void InputForm::handleEvent(const sf::Event& event, sf::RenderWindow& window, std::function<void(const char**)> onSubmit) {
    if (!active) return;

    for (int i = 0; i < fieldCount; i++) inputs[i]->handleEvent(event, window);

    btnCancel->handleEvent(event, window, [&]() {
        hide();
        });

    btnSubmit->handleEvent(event, window, [&]() {
        const char* results[MAX_FORM_FIELDS];
        for (int i = 0; i < fieldCount; i++) {
            results[i] = inputs[i]->getText();
        }
        onSubmit(results);
        hide();
        });
}

void InputForm::update(sf::RenderWindow& window) {
    if (!active) return;
    for (int i = 0; i < fieldCount; i++) inputs[i]->update(window);
    btnSubmit->update(window);
    btnCancel->update(window);
}

void InputForm::draw(sf::RenderWindow& window) {
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

void DataViewer::init(const sf::Font& font, const char* title) {
    overlay.setSize({ 1920.f, 1080.f });
    overlay.setFillColor(sf::Color(0, 0, 0, 180)); // Darker background

    panel.setSize({ 800.f, 600.f });
    panel.setFillColor(sf::Color(250, 250, 250));
    panel.setPosition({ 1920.f / 2.f - 400.f, 1080.f / 2.f - 300.f });
    panel.setOutlineThickness(3.f);
    panel.setOutlineColor(sf::Color(50, 50, 50));

    titleText = new sf::Text(font);
    titleText->setString(title);
    titleText->setCharacterSize(35);
    titleText->setFillColor(sf::Color(0, 51, 102));
    titleText->setPosition({ panel.getPosition().x + 30.f, panel.getPosition().y + 20.f });

    contentText = new sf::Text(font);
    contentText->setCharacterSize(20);
    contentText->setFillColor(sf::Color::Black);
    contentText->setPosition({ panel.getPosition().x + 30.f, panel.getPosition().y + 90.f });
    // Adjust line spacing so lists look clean
    contentText->setLineSpacing(1.5f);

    btnClose = new Button({ 200.f, 50.f }, { panel.getPosition().x + 300.f, panel.getPosition().y + 520.f }, "Close", font, sf::Color(200, 50, 50), sf::Color(150, 0, 0));
}

void DataViewer::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (!active) return;
    btnClose->handleEvent(event, window, [&]() {
        hide();
        });
}

void DataViewer::update(sf::RenderWindow& window) {
    if (!active) return;
    btnClose->update(window);
}

void DataViewer::draw(sf::RenderWindow& window) {
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