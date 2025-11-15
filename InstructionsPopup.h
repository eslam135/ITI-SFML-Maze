#pragma once

#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

class InstructionsPopup
{
private:
    RenderWindow& window;
    Font font;
    bool fontLoaded;
    bool visible;
    RectangleShape overlay;
    RectangleShape popupBox;
    Text titleText;
    vector<Text> instructionLines;
    Text closeText;
    
    void setupUI();
    
public:
    InstructionsPopup(RenderWindow& window);
    ~InstructionsPopup();
    
    bool initialize();
    void setVisible(bool visible);
    bool isVisible() const;
    void toggle();
    void handleEvent(Event& event);
    void render();
};



