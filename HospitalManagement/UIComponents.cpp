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