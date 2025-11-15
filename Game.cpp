#include "Game.h"
#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;
using namespace sf;

const float Game::CELL_SIZE = 50.0f;

Game::Game()
    : window(VideoMode::getFullscreenModes()[0], "Maze Game", Style::Fullscreen)
    , videoMode(VideoMode::getFullscreenModes()[0])
    , showPath(false)
    , gameWon(false)
    , showCongratulations(false)
    , inMenu(true)
    , gameStarted(false)
    , mazeWidth(80)
    , mazeHeight(45)
    , algorithm(Maze::Algorithm::DFS)
    , generationTimer(0.0f)
    , generationInterval(0.01f)
    , fontLoaded(false)
    , congratulationsAnimationTime(0.0f)
    , currentZoom(1.0f)
    , targetZoom(1.0f)
    , zoomTransitionTime(0.0f)
    , zoomTransitionDuration(1.0f)
    , isZoomingBack(false)
{
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(true);
    
    Vector2u windowSize = window.getSize();
    defaultView = View(FloatRect(0, 0, (windowSize.x), (windowSize.y)));
    
    gameView = View(FloatRect(0, 0, (windowSize.x), (windowSize.y)));
    baseViewSize = Vector2f((windowSize.x), (windowSize.y));
}

Game::~Game() = default;

bool Game::initialize()
{
    if (!blockTexture.loadFromFile("assets/block1.png"))
    {
        cerr << "Failed to load block1.png" << endl;
        return false;
    }
    
    if (font.loadFromFile("font.ttf"))
    {
        fontLoaded = true;
    }
    
    menu = make_unique<Menu>(window);
    if (!menu->initialize())
    {
        cerr << "Failed to initialize menu" << endl;
        return false;
    }
    
    instructionsPopup = make_unique<InstructionsPopup>(window);
    if (!instructionsPopup->initialize())
    {
        cerr << "Failed to initialize instructions popup" << endl;
    }
    
    if (fontLoaded)
    {
        congratulationsText.setFont(font);
        congratulationsText.setString("Congratulations! You reached the goal!");
        congratulationsText.setCharacterSize(72);
        congratulationsText.setFillColor(Color::Green);
        congratulationsText.setStyle(Text::Bold);
        FloatRect textBounds = congratulationsText.getLocalBounds();
        Vector2u windowSize = window.getSize();
        congratulationsText.setPosition(
            (windowSize.x - textBounds.width) / 2.0f,
            (windowSize.y - textBounds.height) / 2.0f
        );
    }
    
    return true;
}

void Game::run()
{
    if (!initialize())
    {
        return;
    }
    
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                if (gameStarted && !inMenu && !showCongratulations)
                {
                    if (instructionsPopup)
                    {
                        instructionsPopup->toggle();
                    }
                }
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::R)
            {
                if (gameStarted && !inMenu)
                {
                    inMenu = true;
                    gameStarted = false;
                    showCongratulations = false;
                    gameWon = false;
                    showPath = false;
                    path.clear();
                    if (menu)
                    {
                        menu->reset();
                    }
                }
            }
            else if (event.type == Event::KeyPressed && event.key.code == Keyboard::Q)
            {
                window.close();
            }
            
            if (inMenu)
            {
                handleMenuInput(event);
            }
            else if (gameStarted)
            {
                if (!showCongratulations)
                {
                    handleGameInput(event);
                }
                
                if (!(event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) && !showCongratulations)
                {
                    if (instructionsPopup)
                    {
                        instructionsPopup->handleEvent(event);
                    }
                }
            }
        }
        
        float deltaTime = clock.restart().asSeconds();
        
        if (inMenu)
        {
            updateMenu(deltaTime);
            renderMenu();
        }
        else if (gameStarted)
        {
            updateGame(deltaTime);
            renderGame();
        }
    }
}

void Game::handleMenuInput(Event& event)
{
    if (menu)
    {
        menu->handleEvent(event);
        
        if (menu->isConfirmed())
        {
            mazeWidth = menu->getMazeWidth();
            mazeHeight = menu->getMazeHeight();
            algorithm = (menu->getAlgorithm() == Menu::Algorithm::PRIM) ? Maze::Algorithm::PRIM : Maze::Algorithm::DFS;
            inMenu = false;
            gameStarted = true;
            initializeGame();
        }
    }
}

void Game::handleGameInput(Event& event)
{
    if (showCongratulations)
    {
        return;
    }
    
    static bool enterPressed = false;
    if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter)
    {
        if (!enterPressed && maze && player && goal && maze->isGenerationComplete())
        {
            updatePath();
            showPath = !showPath;
            enterPressed = true;
        }
    }
    else if (event.type == Event::KeyReleased && event.key.code == Keyboard::Enter)
    {
        enterPressed = false;
    }
}

void Game::initializeGame()
{
    maze = make_unique<Maze>(mazeWidth, mazeHeight, CELL_SIZE, algorithm);
    
    gameWon = false;
    showCongratulations = false;
    showPath = false;
    path.clear();
    goal.reset();
    congratulationsAnimationTime = 0.0f;
    
    currentZoom = 1.0f;
    targetZoom = 1.0f;
    zoomTransitionTime = 0.0f;
    isZoomingBack = false;
    gameView.setSize(baseViewSize);
    
    generateMaze();
    
    Vector2f startPos = gridToWorld(Vector2i(1, 1));
    player = make_unique<Player>(startPos.x + CELL_SIZE / 2.0f - 8.0f, startPos.y + CELL_SIZE / 2.0f - 8.0f, CELL_SIZE);
    
    if (fontLoaded)
    {
        Vector2u windowSize = window.getSize();
        FloatRect textBounds = congratulationsText.getLocalBounds();
        congratulationsText.setPosition(
            (windowSize.x - textBounds.width) / 2.0f,
            (windowSize.y - textBounds.height) / 2.0f
        );
    }
}

void Game::generateMaze()
{
    if (maze)
    {
        maze->reset();
        for (int i = 0; i < 5; i++)
        {
            maze->generateStep();
        }
    }
}

void Game::placeGoal()
{
    if (!maze || !maze->isGenerationComplete())
    {
        return;
    }
    
    int mazeWidth = maze->getWidth();
    int mazeHeight = maze->getHeight();
    
    int maxOddX = ((mazeWidth - 1) / 2) * 2 + 1;
    int maxOddY = ((mazeHeight - 1) / 2) * 2 + 1;
    
    if (maxOddX >= mazeWidth) maxOddX -= 2;
    if (maxOddY >= mazeHeight) maxOddY -= 2;
    
    for (int y = maxOddY; y >= 1; y -= 2)
    {
        for (int x = maxOddX; x >= 1; x -= 2)
        {
            const Cell* cell = maze->getCell(x, y);
            if (cell && cell->getType() == CellType::PATH && (x != 1 || y != 1))
            {
                Vector2f goalPos = gridToWorld(Vector2i(x, y));
                goal = make_unique<Goal>(
                    goalPos.x + CELL_SIZE / 2.0f - 10.0f,
                    goalPos.y + CELL_SIZE / 2.0f - 10.0f,
                    12.0f
                );
                return;
            }
        }
    }
    
    for (int y = mazeHeight - 1; y >= 0; y--)
    {
        for (int x = mazeWidth - 1; x >= 0; x--)
        {
            const Cell* cell = maze->getCell(x, y);
            if (cell && cell->getType() == CellType::PATH && (x != 1 || y != 1))
            {
                Vector2f goalPos = gridToWorld(Vector2i(x, y));
                goal = make_unique<Goal>(
                    goalPos.x + CELL_SIZE / 2.0f - 10.0f,
                    goalPos.y + CELL_SIZE / 2.0f - 10.0f,
                    12.0f
                );
                return;
            }
        }
    }
}

void Game::updateMenu(float deltaTime)
{
    if (menu)
    {
        menu->update();
    }
}

void Game::updateGame(float deltaTime)
{
    if (maze && !maze->isGenerationComplete())
    {
        generationTimer += deltaTime;
        if (generationTimer >= generationInterval)
        {
            maze->generateStep();
            generationTimer = 0.0f;
            
            if (maze->isGenerationComplete() && !goal)
            {
                placeGoal();
                path.clear();
                showPath = false;
                
                targetZoom = 1.0f;
                zoomTransitionTime = 0.0f;
                isZoomingBack = true;
            }
        }
        
        if (!isZoomingBack)
        {
            float mazeWorldWidth = maze->getWidth() * CELL_SIZE;
            float mazeWorldHeight = maze->getHeight() * CELL_SIZE;
            
            float padding = 1.2f;
            float zoomX = (mazeWorldWidth * padding) / baseViewSize.x;
            float zoomY = (mazeWorldHeight * padding) / baseViewSize.y;
            targetZoom = max(zoomX, zoomY);
            
            if (fabs(currentZoom - targetZoom) > 0.01f)
            {
                currentZoom += (targetZoom - currentZoom) * deltaTime * 2.0f;
                Vector2f newSize = Vector2f(
                    baseViewSize.x * currentZoom,
                    baseViewSize.y * currentZoom
                );
                gameView.setSize(newSize);
            }
            else
            {
                currentZoom = targetZoom;
                Vector2f newSize = Vector2f(
                    baseViewSize.x * currentZoom,
                    baseViewSize.y * currentZoom
                );
                gameView.setSize(newSize);
            }
            
            Vector2f mazeCenter(
                mazeWorldWidth * 0.5f,
                mazeWorldHeight * 0.5f
            );
            gameView.setCenter(mazeCenter);
        }
    }
    
    if (isZoomingBack && currentZoom != targetZoom)
    {
        zoomTransitionTime += deltaTime;
        float t = min(1.0f, zoomTransitionTime / zoomTransitionDuration);
        
        t = 1.0f - (1.0f - t) * (1.0f - t);
        
        currentZoom = currentZoom + (targetZoom - currentZoom) * t;
        
        Vector2f newSize = Vector2f(
            baseViewSize.x * currentZoom,
            baseViewSize.y * currentZoom
        );
        gameView.setSize(newSize);
        
        if (t >= 1.0f)
        {
            currentZoom = targetZoom;
            isZoomingBack = false;
        }
    }
    
    if (player && maze && maze->isGenerationComplete() && !isZoomingBack && currentZoom <= 1.01f)
    {
        Vector2f playerCenter = player->getCenter();
        
        float mazeWorldWidth = maze->getWidth() * CELL_SIZE;
        float mazeWorldHeight = maze->getHeight() * CELL_SIZE;
        
        Vector2f viewSize = gameView.getSize();
        float halfViewWidth = viewSize.x * 0.5f;
        float halfViewHeight = viewSize.y * 0.5f;
        
        float clampedX = playerCenter.x;
        float clampedY = playerCenter.y;
        
        if (clampedX - halfViewWidth < 0.0f)
        {
            clampedX = halfViewWidth;
        }
        else if (clampedX + halfViewWidth > mazeWorldWidth)
        {
            clampedX = mazeWorldWidth - halfViewWidth;
        }
        
        if (clampedY - halfViewHeight < 0.0f)
        {
            clampedY = halfViewHeight;
        }
        else if (clampedY + halfViewHeight > mazeWorldHeight)
        {
            clampedY = mazeWorldHeight - halfViewHeight;
        }
        
        if (mazeWorldWidth < viewSize.x)
        {
            clampedX = mazeWorldWidth * 0.5f;
        }
        if (mazeWorldHeight < viewSize.y)
        {
            clampedY = mazeWorldHeight * 0.5f;
        }
        
        gameView.setCenter(clampedX, clampedY);
    }
    else if (player && maze && maze->isGenerationComplete() && isZoomingBack)
    {
        float mazeWorldWidth = maze->getWidth() * CELL_SIZE;
        float mazeWorldHeight = maze->getHeight() * CELL_SIZE;
        Vector2f mazeCenter(mazeWorldWidth * 0.5f, mazeWorldHeight * 0.5f);
        Vector2f playerCenter = player->getCenter();
        
        float t = min(1.0f, zoomTransitionTime / zoomTransitionDuration);
        t = 1.0f - (1.0f - t) * (1.0f - t);
        
        Vector2f currentCenter = mazeCenter + (playerCenter - mazeCenter) * t;
        gameView.setCenter(currentCenter);
    }
    
    bool canMove = player && maze && maze->isGenerationComplete() && 
                   (!instructionsPopup || !instructionsPopup->isVisible()) &&
                   !showCongratulations;
    
    if (!canMove && player)
    {
        player->setMoving(false);
    }
    
    if (canMove)
    {
        float moveSpeed = 200.0f;
        float dx = 0.0f;
        float dy = 0.0f;
        
        if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::Up))
        {
            dy = -moveSpeed;
        }
        if (Keyboard::isKeyPressed(Keyboard::S) || Keyboard::isKeyPressed(Keyboard::Down))
        {
            dy = moveSpeed;
        }
        if (Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::Left))
        {
            dx = -moveSpeed;
        }
        if (Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::Right))
        {
            dx = moveSpeed;
        }
        
        if (dx != 0.0f && dy != 0.0f)
        {
            dx *= 0.707f;
            dy *= 0.707f;
        }
        
        bool isMoving = (dx != 0.0f || dy != 0.0f);
        player->setMoving(isMoving);
        
        if (dx < 0.0f)
        {
            player->setFacingRight(false);
        }
        else if (dx > 0.0f)
        {
            player->setFacingRight(true);
        }
        
        if (dx != 0.0f || dy != 0.0f)
        {
            Vector2f currentPos = player->getPosition();
            float newX = currentPos.x + dx * deltaTime;
            float newY = currentPos.y + dy * deltaTime;
            
            if (canMoveTo(newX, currentPos.y))
            {
                player->setPosition(newX, currentPos.y);
                currentPos = player->getPosition();
            }
            
            if (canMoveTo(currentPos.x, newY))
            {
                player->setPosition(currentPos.x, newY);
            }
        }
        
        player->update(deltaTime);
    }
    
    if (!gameWon && maze && maze->isGenerationComplete())
    {
        checkGoalCollision();
    }
    
    if (showCongratulations)
    {
        congratulationsAnimationTime += deltaTime;
    }
}

void Game::checkGoalCollision()
{
    if (player && goal)
    {
        Vector2f playerCenter = player->getCenter();
        Vector2f goalCenter = goal->getCenter();
        float playerRadius = player->getRadius();
        float goalRadius = goal->getRadius();
        
        float dx = playerCenter.x - goalCenter.x;
        float dy = playerCenter.y - goalCenter.y;
        float distance = sqrt(dx * dx + dy * dy);
        
        if (distance < (playerRadius + goalRadius))
        {
            gameWon = true;
            showCongratulations = true;
            congratulationsAnimationTime = 0.0f;
            cout << "Congratulations! You reached the goal!" << endl;
        }
    }
}

void Game::updatePath()
{
    if (!maze || !player || !goal || !maze->isGenerationComplete())
    {
        return;
    }
    
    Vector2i playerGrid = worldToGrid(player->getCenter());
    Vector2i goalGrid = worldToGrid(goal->getCenter());
    
    int mazeWidth = maze->getWidth();
    int mazeHeight = maze->getHeight();
    
    if (playerGrid.x < 0 || playerGrid.x >= mazeWidth || playerGrid.y < 0 || playerGrid.y >= mazeHeight)
    {
        return;
    }
    if (goalGrid.x < 0 || goalGrid.x >= mazeWidth || goalGrid.y < 0 || goalGrid.y >= mazeHeight)
    {
        return;
    }
    
    path = pathFinder.findPath(*maze, playerGrid, goalGrid);
}

Vector2i Game::worldToGrid(const Vector2f& worldPos) const
{
    return Vector2i(
        (int)(worldPos.x / CELL_SIZE),
        (int)(worldPos.y / CELL_SIZE)
    );
}

Vector2f Game::gridToWorld(const Vector2i& gridPos) const
{
    return Vector2f(
        gridPos.x * CELL_SIZE,
        gridPos.y * CELL_SIZE
    );
}

bool Game::canMoveTo(float x, float y) const
{
    if (!maze || !maze->isGenerationComplete())
    {
        return false;
    }
    
    Vector2f playerCenter = Vector2f(x + player->getRadius(), y + player->getRadius());
    Vector2i gridPos = worldToGrid(playerCenter);
    
    int mazeWidth = maze->getWidth();
    int mazeHeight = maze->getHeight();
    
    if (gridPos.x < 0 || gridPos.x >= mazeWidth || gridPos.y < 0 || gridPos.y >= mazeHeight)
    {
        return false;
    }
    
    const Cell* centerCell = maze->getCell(gridPos.x, gridPos.y);
    if (!centerCell || centerCell->getType() != CellType::PATH)
    {
        return false;
    }
    
    float playerRadius = player->getRadius();
    vector<Vector2f> checkPoints = {
        Vector2f(x + playerRadius * 0.3f, y + playerRadius * 0.3f),
        Vector2f(x + playerRadius * 1.7f, y + playerRadius * 0.3f),
        Vector2f(x + playerRadius * 0.3f, y + playerRadius * 1.7f),
        Vector2f(x + playerRadius * 1.7f, y + playerRadius * 1.7f)
    };
    
    for (const Vector2f& point : checkPoints)
    {
        Vector2i checkGridPos = worldToGrid(point);
        if (checkGridPos.x < 0 || checkGridPos.x >= mazeWidth || checkGridPos.y < 0 || checkGridPos.y >= mazeHeight)
        {
            continue;
        }
        
        const Cell* cell = maze->getCell(checkGridPos.x, checkGridPos.y);
        if (cell && cell->getType() == CellType::WALL)
        {
            return false;
        }
    }
    
    return true;
}

void Game::drawPath()
{
    if (!showPath || path.empty())
    {
        return;
    }
    
    CircleShape pathPoint(4.0f);
    pathPoint.setFillColor(Color(255, 50, 50, 200));
    pathPoint.setOutlineColor(Color::Red);
    pathPoint.setOutlineThickness(1.0f);
    
    for (const Vector2i& gridPos : path)
    {
        Vector2f worldPos = gridToWorld(gridPos);
        pathPoint.setPosition(worldPos.x + CELL_SIZE / 2.0f - 4.0f, worldPos.y + CELL_SIZE / 2.0f - 4.0f);
        window.draw(pathPoint);
    }
}

void Game::drawCongratulationsScreen()
{
    Vector2u windowSize = window.getSize();
    
    float fadeAlpha = min(1.0f, congratulationsAnimationTime * 2.0f);
    float overlayAlpha = 200.0f * fadeAlpha;
    
    RectangleShape overlay(Vector2f(windowSize.x, windowSize.y));
    overlay.setFillColor(Color(0, 0, 0, (int)(overlayAlpha)));
    window.draw(overlay);
    
    float scale = min(1.0f, congratulationsAnimationTime * 3.0f);
    float bounce = sin(congratulationsAnimationTime * 2.0f) * 0.05f + 1.0f;
    scale *= bounce;
    
    float boxWidth = 1000.0f * scale;
    float boxHeight = 500.0f * scale;
    float boxX = (windowSize.x - boxWidth) / 2.0f;
    float boxY = (windowSize.y - boxHeight) / 2.0f;
    
    RectangleShape outerGlow(Vector2f(boxWidth + 40, boxHeight + 40));
    outerGlow.setFillColor(Color(255, 215, 0, (int)(100 * fadeAlpha)));
    outerGlow.setPosition(boxX - 20, boxY - 20);
    outerGlow.setOutlineThickness(0);
    window.draw(outerGlow);
    
    RectangleShape middleGlow(Vector2f(boxWidth + 20, boxHeight + 20));
    middleGlow.setFillColor(Color(255, 200, 0, (int)(150 * fadeAlpha)));
    middleGlow.setPosition(boxX - 10, boxY - 10);
    middleGlow.setOutlineThickness(0);
    window.draw(middleGlow);
    
    RectangleShape mainBox(Vector2f(boxWidth, boxHeight));
    
    float gradientHeight = boxHeight / 3.0f;
    
    RectangleShape topGradient(Vector2f(boxWidth, gradientHeight));
    topGradient.setFillColor(Color(50, 200, 50, (int)(250 * fadeAlpha)));
    topGradient.setPosition(boxX, boxY);
    window.draw(topGradient);
    
    RectangleShape midGradient(Vector2f(boxWidth, gradientHeight));
    midGradient.setFillColor(Color(30, 180, 30, (int)(250 * fadeAlpha)));
    midGradient.setPosition(boxX, boxY + gradientHeight);
    window.draw(midGradient);
    
    RectangleShape bottomGradient(Vector2f(boxWidth, gradientHeight));
    bottomGradient.setFillColor(Color(20, 150, 20, (int)(250 * fadeAlpha)));
    bottomGradient.setPosition(boxX, boxY + gradientHeight * 2.0f);
    window.draw(bottomGradient);
    
    float outlineThickness = 8.0f + sin(congratulationsAnimationTime * 4.0f) * 2.0f;
    RectangleShape outlineBox(Vector2f(boxWidth, boxHeight));
    outlineBox.setFillColor(Color::Transparent);
    outlineBox.setOutlineColor(Color(0, 255, 100, (int)(255 * fadeAlpha)));
    outlineBox.setOutlineThickness(outlineThickness);
    outlineBox.setPosition(boxX, boxY);
    window.draw(outlineBox);
    
    RectangleShape innerHighlight(Vector2f(boxWidth - 20, 60));
    innerHighlight.setFillColor(Color(255, 255, 255, (int)(50 * fadeAlpha)));
    innerHighlight.setPosition(boxX + 10, boxY + 10);
    window.draw(innerHighlight);
    
    if (fontLoaded)
    {
        float titleScale = min(1.0f, congratulationsAnimationTime * 2.0f);
        float titleY = boxY + 80.0f;
        
        Text titleShadow = congratulationsText;
        titleShadow.setFillColor(Color(0, 100, 0, (int)(200 * fadeAlpha)));
        titleShadow.setCharacterSize((int)(72 * titleScale));
        FloatRect titleShadowBounds = titleShadow.getLocalBounds();
        titleShadow.setPosition(
            (windowSize.x - titleShadowBounds.width) / 2.0f + 4,
            titleY + 4
        );
        window.draw(titleShadow);
        
        Text title = congratulationsText;
        title.setCharacterSize((int)(72 * titleScale));
        title.setFillColor(Color(255, 255, 100, (int)(255 * fadeAlpha)));
        title.setStyle(Text::Bold);
        FloatRect titleBounds = title.getLocalBounds();
        title.setPosition(
            (windowSize.x - titleBounds.width) / 2.0f,
            titleY
        );
        window.draw(title);
        
        Text subtitle;
        subtitle.setFont(font);
        subtitle.setString("You've successfully navigated the maze!");
        subtitle.setCharacterSize((int)(36 * titleScale));
        subtitle.setFillColor(Color(200, 255, 200, (int)(255 * fadeAlpha)));
        FloatRect subtitleBounds = subtitle.getLocalBounds();
        subtitle.setPosition(
            (windowSize.x - subtitleBounds.width) / 2.0f,
            titleY + 120.0f
        );
        window.draw(subtitle);
        
        Text instruction;
        instruction.setFont(font);
        instruction.setString("Press R to return to menu");
        instruction.setCharacterSize((int)(28 * titleScale));
        instruction.setFillColor(Color(255, 255, 200, (int)(220 * fadeAlpha)));
        FloatRect instructionBounds = instruction.getLocalBounds();
        instruction.setPosition(
            (windowSize.x - instructionBounds.width) / 2.0f,
            boxY + boxHeight - 80.0f
        );
        window.draw(instruction);
        
        float particleTime = congratulationsAnimationTime * 2.0f;
        for (int i = 0; i < 8; ++i)
        {
            float angle = (i / 8.0f) * 3.14159f * 2.0f + particleTime;
            float radius = 150.0f + sin(particleTime + i) * 30.0f;
            float px = windowSize.x / 2.0f + cos(angle) * radius;
            float py = windowSize.y / 2.0f + sin(angle) * radius;
            
            CircleShape star(6.0f);
            star.setFillColor(Color(255, 255, 100, (int)(200 * fadeAlpha)));
            star.setPosition(px - 6, py - 6);
            window.draw(star);
        }
    }
    else
    {
        RectangleShape congratsBox(Vector2f(boxWidth, boxHeight));
        congratsBox.setFillColor(Color(0, 200, 0, (int)(240 * fadeAlpha)));
        congratsBox.setOutlineColor(Color(0, 255, 0, (int)(255 * fadeAlpha)));
        congratsBox.setOutlineThickness(10.0f);
        congratsBox.setPosition(boxX, boxY);
        window.draw(congratsBox);
    }
}

void Game::renderMenu()
{
    window.clear(Color(20, 20, 30, 255));
    
    if (menu)
    {
        menu->render();
    }
    
    window.display();
}

void Game::renderGame()
{
    Vector2u windowSize = window.getSize();
    
    window.clear(Color(25, 25, 35, 255));
    
    window.setView(gameView);
    
    if (maze)
    {
        maze->draw(window, blockTexture);
    }
    
    if (showPath)
    {
        drawPath();
    }
    
    if (goal)
    {
        goal->draw(window);
    }
    
    if (player)
    {
        player->draw(window);
    }
    
    window.setView(defaultView);
    
    if (instructionsPopup && !showCongratulations && instructionsPopup->isVisible())
    {
        instructionsPopup->render();
    }
    
    if (showCongratulations)
    {
        drawCongratulationsScreen();
    }
    
    window.display();
}

void Game::render()
{
    if (inMenu)
    {
        renderMenu();
    }
    else if (gameStarted)
    {
        renderGame();
    }
}

void Game::update(float deltaTime)
{
    if (inMenu)
    {
        updateMenu(deltaTime);
    }
    else if (gameStarted)
    {
        updateGame(deltaTime);
    }
}