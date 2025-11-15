#pragma once

#include <SFML/Graphics.hpp>
#include <string>
using namespace std;
using namespace sf;

class Menu
{
private:
    RenderWindow& window;
    Font font;
    bool fontLoaded;
    Text titleText;
    Text instructionsText;
    Text inputLabelText;
    Text inputText;
    Text confirmText;
    Text algorithmLabelText;
    Text dfsButtonText;
    Text primButtonText;
    RectangleShape background;
    RectangleShape inputBox;
    RectangleShape confirmButton;
    RectangleShape dfsButton;
    RectangleShape primButton;
    string inputString;
    int mazeWidth;
    int mazeHeight;
    bool confirmed;
    bool focused;
    bool usePrim;
    
    void setupUI();
    void parseInput();
    void updateInputDisplay();
    
public:
    enum class Algorithm
    {
        DFS,
        PRIM
    };
    
    Menu(RenderWindow& window);
    ~Menu();
    
    bool initialize();
    void handleEvent(Event& event);
    void update();
    void render();
    
    bool isConfirmed() const;
    int getMazeWidth() const;
    int getMazeHeight() const;
    Algorithm getAlgorithm() const;
    void reset();
};

