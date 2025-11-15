#include "Menu.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

Menu::Menu(RenderWindow& window)
    : window(window)
    , fontLoaded(false)
    , inputString("20x20")
    , mazeWidth(80)
    , mazeHeight(45)
    , confirmed(false)
    , focused(true)
    , usePrim(false)
{
    setupUI();
}

Menu::~Menu() = default;

bool Menu::initialize()
{
    if (font.loadFromFile("assets/font.ttf"))
    {
        fontLoaded = true;
        setupUI();
    }
    else
    {
        fontLoaded = false;
    }
    
    return true;
}

void Menu::setupUI()
{
    Vector2u windowSize = window.getSize();
    
    background.setSize(Vector2f(windowSize.x, windowSize.y));
    background.setFillColor(Color(20, 20, 30, 255));
    
    if (fontLoaded)
    {
        titleText.setFont(font);
        titleText.setString("MAZE GAME");
        titleText.setCharacterSize(80);
        titleText.setFillColor(Color::White);
        titleText.setStyle(Text::Bold);
        FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setPosition(
            (windowSize.x - titleBounds.width) / 2.0f,
            windowSize.y * 0.2f
        );
        
        instructionsText.setFont(font);
        instructionsText.setString("Enter Maze Size (Width x Height)\nExample: 21*21 or 21x21\nPress ESC in-game for rules");
        instructionsText.setCharacterSize(28);
        instructionsText.setFillColor(Color(200, 200, 200));
        FloatRect instrBounds = instructionsText.getLocalBounds();
        instructionsText.setPosition(
            (windowSize.x - instrBounds.width) / 2.0f,
            windowSize.y * 0.32f
        );
       
        dfsButtonText.setFont(font);
        dfsButtonText.setString("DFS");
        dfsButtonText.setCharacterSize(32);
        dfsButtonText.setFillColor(Color::White);
        dfsButtonText.setStyle(Text::Bold);
        FloatRect dfsBounds = dfsButtonText.getLocalBounds();
        dfsButtonText.setPosition(
            (windowSize.x - 200) / 2.0f - 80.0f + (160 - dfsBounds.width) / 2.0f,
            windowSize.y * 0.48f + 15.0f
        );
        
        primButtonText.setFont(font);
        primButtonText.setString("PRIM");
        primButtonText.setCharacterSize(32);
        primButtonText.setFillColor(Color::White);
        primButtonText.setStyle(Text::Bold);
        FloatRect primBounds = primButtonText.getLocalBounds();
        primButtonText.setPosition(
            (windowSize.x + 200) / 2.0f - 80.0f + (160 - primBounds.width) / 2.0f,
            windowSize.y * 0.48f + 15.0f
        );
        
        inputLabelText.setFont(font);
        inputLabelText.setString("Maze Size:");
        inputLabelText.setCharacterSize(36);
        inputLabelText.setFillColor(Color::White);
        FloatRect labelBounds = inputLabelText.getLocalBounds();
        inputLabelText.setPosition(
            (windowSize.x - labelBounds.width) / 2.0f,
            windowSize.y * 0.58f
        );
        
        inputText.setFont(font);
        inputText.setCharacterSize(44);
        inputText.setFillColor(Color::White);
        updateInputDisplay();
        
        confirmText.setFont(font);
        confirmText.setString("START GAME");
        confirmText.setCharacterSize(36);
        confirmText.setFillColor(Color::White);
        confirmText.setStyle(Text::Bold);
        FloatRect confirmBounds = confirmText.getLocalBounds();
        confirmText.setPosition(
            (windowSize.x - confirmBounds.width) / 2.0f,
            windowSize.y * 0.75f + 15.0f
        );
    }
    
    dfsButton.setSize(Vector2f(160, 60));
    dfsButton.setFillColor(Color(50, 100, 200, 255));
    dfsButton.setOutlineColor(Color::White);
    dfsButton.setOutlineThickness(3.0f);
    dfsButton.setPosition(
        (windowSize.x - 200) / 2.0f - 80.0f,
        windowSize.y * 0.48f
    );
    
    primButton.setSize(Vector2f(160, 60));
    primButton.setFillColor(Color(60, 60, 70, 255));
    primButton.setOutlineColor(Color::White);
    primButton.setOutlineThickness(3.0f);
    primButton.setPosition(
        (windowSize.x + 200) / 2.0f - 80.0f,
        windowSize.y * 0.48f
    );
    
    inputBox.setSize(Vector2f(400, 65));
    inputBox.setFillColor(Color(40, 40, 50, 255));
    inputBox.setOutlineColor(Color::White);
    inputBox.setOutlineThickness(3.0f);
    inputBox.setPosition(
        (windowSize.x - 400) / 2.0f,
        windowSize.y * 0.63f
    );
    
    confirmButton.setSize(Vector2f(300, 65));
    confirmButton.setFillColor(Color(0, 150, 0, 255));
    confirmButton.setOutlineColor(Color::White);
    confirmButton.setOutlineThickness(3.0f);
    confirmButton.setPosition(
        (windowSize.x - 300) / 2.0f,
        windowSize.y * 0.75f
    );
    
    updateInputDisplay();
}

void Menu::handleEvent(Event& event)
{
    if (event.type == Event::TextEntered && focused)
    {
        char c = (char)(event.text.unicode);
        
        if ((c >= '0' && c <= '9') || c == 'x' || c == 'X' || c == '*')
        {
            inputString += c;
            updateInputDisplay();
        }
    }
    else if (event.type == Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == Mouse::Left)
        {
            Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            
            if (inputBox.getGlobalBounds().contains(mousePos))
            {
                focused = true;
                inputBox.setOutlineColor(Color::Yellow);
            }
            else
            {
                focused = false;
                inputBox.setOutlineColor(Color::White);
            }
            
            if (dfsButton.getGlobalBounds().contains(mousePos))
            {
                usePrim = false;
                dfsButton.setFillColor(Color(50, 100, 200, 255));
                primButton.setFillColor(Color(60, 60, 70, 255));
            }
            else if (primButton.getGlobalBounds().contains(mousePos))
            {
                usePrim = true;
                dfsButton.setFillColor(Color(60, 60, 70, 255));
                primButton.setFillColor(Color(50, 100, 200, 255));
            }
            else if (confirmButton.getGlobalBounds().contains(mousePos))
            {
                parseInput();
                if (mazeWidth >= 10 && mazeWidth <= 200 && mazeHeight >= 10 && mazeHeight <= 200)
                {
                    confirmed = true;
                }
            }
        }
    }
    else if (event.type == Event::KeyPressed)
    {
        if (event.key.code == Keyboard::Enter)
        {
            parseInput();
            if (mazeWidth >= 10 && mazeWidth <= 200 && mazeHeight >= 10 && mazeHeight <= 200)
            {
                confirmed = true;
            }
        }
        else if (event.key.code == Keyboard::BackSpace && focused)
        {
            if (!inputString.empty())
            {
                inputString.pop_back();
                updateInputDisplay();
            }
        }
        else if (event.key.code == Keyboard::Escape)
        {
            focused = false;
            inputBox.setOutlineColor(Color::White);
        }
    }
}

void Menu::parseInput()
{
    string input = inputString;
    transform(input.begin(), input.end(), input.begin(), ::tolower);
    
    size_t xPos = input.find('x');
    if (xPos == string::npos)
    {
        xPos = input.find('*');
    }
    
    if (xPos != string::npos)
    {
        string widthStr = input.substr(0, xPos);
        string heightStr = input.substr(xPos + 1);
        
        try
        {
            mazeWidth = stoi(widthStr);
            mazeHeight = stoi(heightStr);
            
            mazeWidth = max(10, min(200, mazeWidth));
            mazeHeight = max(10, min(200, mazeHeight));
            
            inputString = to_string(mazeWidth) + "x" + to_string(mazeHeight);
            updateInputDisplay();
        }
        catch (exception e)
        {
            cerr << e.what() << endl;
        }
    }
}

void Menu::updateInputDisplay()
{
    if (fontLoaded)
    {
        string displayText = inputString;
        if (focused)
        {
            displayText += "_";
        }
        inputText.setString(displayText);
        FloatRect textBounds = inputText.getLocalBounds();
        Vector2u windowSize = window.getSize();
        inputText.setPosition(
            (windowSize.x - textBounds.width) / 2.0f,
            windowSize.y * 0.64f
        );
    }
}

void Menu::update()
{
    static Clock blinkClock;
    if (focused)
    {
        float time = blinkClock.getElapsedTime().asSeconds();
        float alpha = 200 + 55 * sin(time * 3.0f);
        if (alpha < 0) alpha = 0;
        if (alpha > 255) alpha = 255;
        inputBox.setOutlineColor(Color(255, 255, 0, (int)(alpha)));
    }
    else
    {
        inputBox.setOutlineColor(Color::White);
    }
    
    updateInputDisplay();
}

void Menu::render()
{
    window.draw(background);
    
    if (fontLoaded)
    {
        window.draw(titleText);
        window.draw(instructionsText);
        window.draw(algorithmLabelText);
        window.draw(inputLabelText);
    }
    
    window.draw(dfsButton);
    window.draw(primButton);
    
    if (fontLoaded)
    {
        window.draw(dfsButtonText);
        window.draw(primButtonText);
    }
    
    window.draw(inputBox);
    
    if (fontLoaded)
    {
        window.draw(inputText);
        window.draw(confirmButton);
        window.draw(confirmText);
    }
}

bool Menu::isConfirmed() const
{
    return confirmed;
}

int Menu::getMazeWidth() const
{
    return mazeWidth;
}

int Menu::getMazeHeight() const
{
    return mazeHeight;
}

Menu::Algorithm Menu::getAlgorithm() const
{
    return usePrim ? Algorithm::PRIM : Algorithm::DFS;
}

void Menu::reset()
{
    confirmed = false;
    inputString = "20x20";
    mazeWidth = 80;
    mazeHeight = 45;
    usePrim = false;
    dfsButton.setFillColor(Color(50, 100, 200, 255));
    primButton.setFillColor(Color(60, 60, 70, 255));
    updateInputDisplay();
}

