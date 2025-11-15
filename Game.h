#pragma once

#include "Maze.h"
#include "Player.h"
#include "Goal.h"
#include "PathFinder.h"
#include "Menu.h"
#include "InstructionsPopup.h"
#include <SFML/Graphics.hpp>
#include <vector>
using namespace std;
using namespace sf;

class Game
{
private:
    static const float CELL_SIZE;
    
    RenderWindow window;
    VideoMode videoMode;
    View gameView;
    View defaultView;
    Texture blockTexture;
    Font font;
    Text congratulationsText;
    bool fontLoaded;
    
    unique_ptr<Menu> menu;
    unique_ptr<InstructionsPopup> instructionsPopup;
    
    unique_ptr<Maze> maze;
    unique_ptr<Player> player;
    unique_ptr<Goal> goal;
    PathFinder pathFinder;
    
    vector<Vector2i> path;
    bool showPath;
    bool gameWon;
    bool showCongratulations;
    bool inMenu;
    bool gameStarted;
    float congratulationsAnimationTime;
    
    int mazeWidth;
    int mazeHeight;
    Maze::Algorithm algorithm;
    
    Clock clock;
    float generationTimer;
    float generationInterval;
    
    float currentZoom;
    float targetZoom;
    float zoomTransitionTime;
    float zoomTransitionDuration;
    bool isZoomingBack;
    Vector2f baseViewSize;
    
    void handleMenuInput(Event& event);
    void handleGameInput(Event& event);
    void update(float deltaTime);
    void updateMenu(float deltaTime);
    void updateGame(float deltaTime);
    void render();
    void renderMenu();
    void renderGame();
    void initializeGame();
    void generateMaze();
    void placeGoal();
    void checkGoalCollision();
    void updatePath();
    Vector2i worldToGrid(const Vector2f& worldPos) const;
    Vector2f gridToWorld(const Vector2i& gridPos) const;
    bool canMoveTo(float x, float y) const;
    void drawPath();
    void drawCongratulationsScreen();

public:
    Game();
    ~Game();
    
    void run();
    bool initialize();
};

