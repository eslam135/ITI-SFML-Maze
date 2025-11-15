#include "InstructionsPopup.h"
#include <vector>

InstructionsPopup::InstructionsPopup(RenderWindow& window)
    : window(window)
    , fontLoaded(false)
    , visible(false)
{
    setupUI();
}

InstructionsPopup::~InstructionsPopup() = default;

bool InstructionsPopup::initialize()
{
    if (font.loadFromFile("assets/font.ttf"))
    {
        fontLoaded = true;
        setupUI();
    }
    return true;
}

void InstructionsPopup::setupUI()
{
    Vector2u windowSize = this->window.getSize();
    
    overlay.setSize(Vector2f(windowSize.x, windowSize.y));
    overlay.setFillColor(Color(0, 0, 0, 180));
    
    float boxWidth = 600;
    float boxHeight = 400;
    popupBox.setSize(Vector2f(boxWidth, boxHeight));
    popupBox.setFillColor(Color(30, 30, 40, 255));
    popupBox.setOutlineColor(Color::White);
    popupBox.setOutlineThickness(4.0f);
    popupBox.setPosition(
        (windowSize.x - boxWidth) / 2.0f,
        (windowSize.y - boxHeight) / 2.0f
    );
    
    if (fontLoaded)
    {
        titleText.setFont(font);
        titleText.setString("INSTRUCTIONS");
        titleText.setCharacterSize(48);
        titleText.setFillColor(Color::Yellow);
        titleText.setStyle(Text::Bold);
        FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(
            (windowSize.x - titleBounds.width) / 2.0f,
            popupBox.getPosition().y + 20.0f
        );
        
        vector<string> instructions = {
            "Movement: WASD or Arrow Keys",
            "Show Path: Press ENTER",
            "Return to Menu: Press R",
            "Close Menu: Press ESCAPE",
            "Quit Game: Press Q",
            "Reach the yellow circle to win!"
        };
        
        instructionLines.clear();
        float startY = popupBox.getPosition().y + 100.0f;
        for (size_t i = 0; i < instructions.size(); ++i)
        {
            Text line;
            line.setFont(font);
            line.setString(instructions[i]);
            line.setCharacterSize(28);
            line.setFillColor(Color::White);
            FloatRect lineBounds = line.getLocalBounds();
            line.setPosition(
                (windowSize.x - lineBounds.width) / 2.0f,
                startY + i * 40.0f
            );
            instructionLines.push_back(line);
        }
        
        closeText.setFont(font);
        closeText.setString("Press ESCAPE to close");
        closeText.setCharacterSize(24);
        closeText.setFillColor(Color(200, 200, 200));
        FloatRect closeBounds = closeText.getLocalBounds();
        closeText.setPosition(
            (windowSize.x - closeBounds.width) / 2.0f,
            popupBox.getPosition().y + boxHeight - 50.0f
        );
    }
}

void InstructionsPopup::setVisible(bool visible)
{
    this->visible = visible;
}

bool InstructionsPopup::isVisible() const
{
    return visible;
}

void InstructionsPopup::toggle()
{
    visible = !visible;
}

void InstructionsPopup::handleEvent(Event& event)
{
    if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
    {
        toggle();
    }
}

void InstructionsPopup::render()
{
    if (!visible)
    {
        return;
    }
    
    Vector2u windowSize = this->window.getSize();
    
    float boxWidth = 600;
    float boxHeight = 400;
    popupBox.setPosition(
        (windowSize.x - boxWidth) / 2.0f,
        (windowSize.y - boxHeight) / 2.0f
    );
    
    overlay.setSize(Vector2f(windowSize.x, windowSize.y));
    
    if (fontLoaded)
    {
        FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(
            (windowSize.x - titleBounds.width) / 2.0f,
            popupBox.getPosition().y + 20.0f
        );
        
        float startY = popupBox.getPosition().y + 100.0f;
        for (size_t i = 0; i < instructionLines.size(); ++i)
        {
            FloatRect lineBounds = instructionLines[i].getLocalBounds();
            instructionLines[i].setPosition(
                (windowSize.x - lineBounds.width) / 2.0f,
                startY + i * 40.0f
            );
        }
        
        FloatRect closeBounds = closeText.getLocalBounds();
        closeText.setPosition(
            (windowSize.x - closeBounds.width) / 2.0f,
            popupBox.getPosition().y + boxHeight - 50.0f
        );
    }
    
    window.draw(overlay);
    window.draw(popupBox);
    
    if (fontLoaded)
    {
        window.draw(titleText);
        for (const auto& line : instructionLines)
        {
            window.draw(line);
        }
        window.draw(closeText);
    }
}

