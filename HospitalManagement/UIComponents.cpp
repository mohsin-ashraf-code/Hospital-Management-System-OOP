#include "UIComponents.h"
#include "Validator.h"
using namespace sf;

Button::Button() : shape(nullptr), text(nullptr) 
{
}

Button::Button(Vector2f size, Vector2f position, const char* label, const Font& font, Color idle, Color hover) 
{
    idleColor = idle;
    hoverColor = hover;

    shape = new RectangleShape(size);
    shape->setPosition(position);
    shape->setFillColor(idle);
    shape->setOutlineThickness(1.f);
    shape->setOutlineColor(Color::White);

    text = new Text(font);
    text->setString(label);

    unsigned int scaledCharSize = static_cast<unsigned int>(size.y * 0.42f);
    text->setCharacterSize(scaledCharSize);
    text->setFillColor(Color::White);

    FloatRect textBounds = text->getLocalBounds();
    text->setOrigin({ textBounds.position.x + textBounds.size.x / 2.0f, textBounds.position.y + textBounds.size.y / 2.0f });
    text->setPosition({ position.x + size.x / 2.0f, position.y + size.y / 2.0f });
}

Button::~Button() 
{
    delete shape;
    delete text;
}

void Button::handleEvent(const Event& event, RenderWindow& window, std::function<void()> onClick) 
{
    if (const auto* mouseEvent = event.getIf<Event::MouseButtonPressed>())
    {
        if (mouseEvent->button == Mouse::Button::Left) 
        {
            Vector2i mousePos = Mouse::getPosition(window);
            if (shape && shape->getGlobalBounds().contains(Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) 
            {
                onClick();
            }
        }
    }
}

void Button::update(const RenderWindow& window) 
{
    if (!shape) 
    {
        return;
    }
    Vector2i mousePos = Mouse::getPosition(window);
    if (shape->getGlobalBounds().contains(Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) 
    {
        shape->setFillColor(hoverColor);
    }
    else 
    {
        shape->setFillColor(idleColor);
    }
}

void Button::draw(RenderWindow& window) 
{
    if (shape) 
    {
        window.draw(*shape);
    }
    if (text) 
    {
        window.draw(*text);
    }
}

InputForm::InputForm() : background(nullptr), formTitle(nullptr), labels(nullptr), inputBoxes(nullptr), inputTexts(nullptr), fieldValues(nullptr), submitBtn(nullptr), cancelBtn(nullptr), fieldCount(0), activeField(0), active(false) 
{
}

InputForm::~InputForm() 
{
    delete background;
    delete formTitle;
    if (labels && inputBoxes && inputTexts && fieldValues) 
    {
        for (int i = 0; i < fieldCount; i++) 
        {
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

void InputForm::init(const Font& font, const char* title, const char** fields, int count) 
{
    fieldCount = count;
    activeField = 0;
    active = false;

    float formWidth = 600.f;
    float formHeight = 150.f + (count * 75.f);

    float startX = (1920.f - formWidth) / 2.0f;
    float startY = (1080.f - formHeight) / 2.0f;

    background = new RectangleShape({ formWidth, formHeight });
    background->setPosition({ startX, startY });
    background->setFillColor(Color(30, 30, 30, 245));
    background->setOutlineThickness(2.f);
    background->setOutlineColor(Color::Cyan);

    formTitle = new Text(font);
    formTitle->setString(title);
    formTitle->setCharacterSize(24);
    formTitle->setFillColor(Color::Cyan);
    formTitle->setPosition({ startX + 50.f, startY + 40.f });

    labels = new Text * [count];
    inputBoxes = new RectangleShape * [count];
    inputTexts = new Text * [count];
    fieldValues = new STRING[count];

    for (int i = 0; i < count; i++)
    {
        *(labels + i) = new Text(font);
        (*(labels + i))->setString(*(fields + i));
        (*(labels + i))->setCharacterSize(16);
        (*(labels + i))->setFillColor(Color::White);
        (*(labels + i))->setPosition({ startX + 50.f, startY + 95.f + (i * 75.f) });

        *(inputBoxes + i) = new RectangleShape({ 500.f, 35.f });
        (*(inputBoxes + i))->setPosition({ startX + 50.f, startY + 120.f + (i * 75.f) });
        (*(inputBoxes + i))->setFillColor(Color::Black);
        (*(inputBoxes + i))->setOutlineThickness(1.f);
        (*(inputBoxes + i))->setOutlineColor(Color(100, 100, 100));

        *(inputTexts + i) = new Text(font);
        (*(inputTexts + i))->setCharacterSize(16);
        (*(inputTexts + i))->setFillColor(Color::Green);
        (*(inputTexts + i))->setPosition({ startX + 60.f, startY + 125.f + (i * 75.f) });

        (*(fieldValues + i)).clear();
    }

    float btnY = startY + 150.f + (count * 75.f) - 60.f;
    submitBtn = new Button({ 200.f, 45.f }, { startX + 50.f, btnY }, "Submit", font, Color(0, 150, 0), Color(0, 200, 0));
    cancelBtn = new Button({ 200.f, 45.f }, { startX + 350.f, btnY }, "Cancel", font, Color(150, 0, 0), Color(200, 0, 0));
}

void InputForm::show() 
{
    active = true;
    activeField = 0;
    for (int i = 0; i < fieldCount; i++) 
    {
        (*(fieldValues + i)).clear();
        (*(inputTexts + i))->setString("");
        (*(inputBoxes + i))->setOutlineColor(Color(100, 100, 100));
    }
    if (fieldCount > 0) (*(inputBoxes + 0))->setOutlineColor(Color::Cyan);
}

void InputForm::hide() 
{ 
    active = false; 
}

bool InputForm::isActive() const 
{ 
    return active; 
}

void InputForm::handleEvent(const Event& event, RenderWindow& window, std::function<void(const char**)> onSubmit) 
{
    if (!active) 
    {
        return;
    }

    if (const auto* keyEvent = event.getIf<Event::KeyPressed>()) 
    {
        if (keyEvent->code == Keyboard::Key::Escape)
        {
            hide();
            return;
        }
    }

    cancelBtn->handleEvent(event, window, [&]() { hide(); });

    submitBtn->handleEvent(event, window, [&]() 
    {
        const char** outData = new const char* [fieldCount];
        for (int k = 0; k < fieldCount; k++) 
        {
            *(outData + k) = (*(fieldValues + k)).get();
        }
        onSubmit(outData);
        delete[] outData;
        hide();
    });

    if (const auto* mouseEvent = event.getIf<Event::MouseButtonPressed>()) 
    {
        if (mouseEvent->button == Mouse::Button::Left) 
        {
            Vector2i mousePos = Mouse::getPosition(window);
            for (int i = 0; i < fieldCount; i++) 
            {
                if ((*(inputBoxes + i))->getGlobalBounds().contains(Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) 
                {
                    (*(inputBoxes + activeField))->setOutlineColor(Color(100, 100, 100));
                    activeField = i;
                    (*(inputBoxes + activeField))->setOutlineColor(Color::Cyan);
                }
            }
        }
    }

    if (const auto* textEvent = event.getIf<Event::TextEntered>()) 
    {
        if (textEvent->unicode == 8) 
        {
            const char* current = (*(fieldValues + activeField)).get();
            int len = Validator::myStrLen(current);
            if (len > 0)
            {
                char* temp = new char[len];
                for (int k = 0; k < len - 1; k++) 
                {
                    *(temp + k) = *(current + k);
                } 
                *(temp + len - 1) = '\0';
                (*(fieldValues + activeField)).copy(temp);
                (*(inputTexts + activeField))->setString((*(fieldValues + activeField)).get());
                delete[] temp;
            }
        }
        else if (textEvent->unicode == 9) 
        {
            (*(inputBoxes + activeField))->setOutlineColor(Color(100, 100, 100));
            activeField = (activeField + 1) % fieldCount;
            (*(inputBoxes + activeField))->setOutlineColor(Color::Cyan);
        }
        else if (textEvent->unicode == 13)
        {
            const char** outData = new const char* [fieldCount];
            for (int k = 0; k < fieldCount; k++)       
            {
                *(outData + k) = (*(fieldValues + k)).get();
            } 
            onSubmit(outData);
            delete[] outData;
            hide();
        }
        else if (textEvent->unicode >= 32 && textEvent->unicode < 127) 
        {
            (*(fieldValues + activeField)).concatenate(static_cast<char>(textEvent->unicode));
            (*(inputTexts + activeField))->setString((*(fieldValues + activeField)).get());
        }
    }
}

void InputForm::update(const RenderWindow& window) 
{
    if (!active) 
    {
        return;
    }
    submitBtn->update(window);
    cancelBtn->update(window);
}

void InputForm::draw(RenderWindow& window) 
{
    if (!active) 
    {
        return;
    } 
    window.draw(*background);
    window.draw(*formTitle);
    for (int i = 0; i < fieldCount; i++) 
    {
        window.draw(*(*(labels + i)));
        window.draw(*(*(inputBoxes + i)));
        window.draw(*(*(inputTexts + i)));
    }
    submitBtn->draw(window);
    cancelBtn->draw(window);
}

DataViewer::DataViewer() : background(nullptr), titleText(nullptr), contentText(nullptr), closeBtn(nullptr), active(false) 
{
}

DataViewer::~DataViewer()
{
    delete background;
    delete titleText;
    delete contentText;
    delete closeBtn;
}


void DataViewer::init(const Font& font, const char* title)
{
    active = false;

    float width = 750.f;
    float height = 550.f;
    float startX = (1920.f - width) / 2.0f;
    float startY = (1080.f - height) / 2.0f;

    background = new RectangleShape({ width, height });
    background->setPosition({ startX, startY });
    background->setFillColor(Color(20, 20, 20, 250));
    background->setOutlineThickness(3.f);
    background->setOutlineColor(Color::Green);

    titleText = new Text(font);
    titleText->setString(title);
    titleText->setCharacterSize(24);
    titleText->setFillColor(Color::Green);
    titleText->setPosition({ startX + 40.f, startY + 40.f });

    contentText = new Text(font);
    contentText->setCharacterSize(16);
    contentText->setFillColor(Color::White);
    contentText->setPosition({ startX + 40.f, startY + 100.f });

    closeBtn = new Button({ 150.f, 45.f }, { startX + (width / 2.0f) - 75.f, startY + height - 70.f }, "Dismiss", font, Color(50, 50, 50), Color(80, 80, 80));
}

void DataViewer::show(String text) 
{
    active = true;
    contentText->setString(text);
}

void DataViewer::hide() 
{ 
    active = false; 
}

bool DataViewer::isActive() const 
{ 
    return active; 
}

void DataViewer::handleEvent(const Event& event, RenderWindow& window)
{
    if (!active) 
    {
        return;
    }
    if (const auto* keyEvent = event.getIf<Event::KeyPressed>()) 
    {
        if (keyEvent->code == Keyboard::Key::Escape) 
        {
            hide();
            return;
        }
    }
    closeBtn->handleEvent(event, window, [&]() { hide(); });
}

void DataViewer::update(const RenderWindow& window) 
{
    if (!active) 
    {
        return;
    }
    closeBtn->update(window);
}

void DataViewer::draw(RenderWindow& window) 
{
    if (!active) 
    {
        return;
    }
    window.draw(*background);
    window.draw(*titleText);
    window.draw(*contentText);
    closeBtn->draw(window);
}