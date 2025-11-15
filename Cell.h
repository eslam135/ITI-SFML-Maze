#pragma once

#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

enum class CellType
{
    WALL,
    PATH,
    VISITED
};

class Cell
{
private:

    CellType type;
    
    bool visited;
    bool revealed;
    
    Vector2i position; 
    
    RectangleShape shape; // Visual representation of the current cell

public:
    Cell(int x, int y, float cellSize);
    
    CellType getType() const;
    void setType(CellType type);
    
    bool isVisited() const;
    void setVisited(bool visited);
    
    bool isRevealed() const;
    void setRevealed(bool revealed);
    
    Vector2i getPosition() const;
    
    void setColor(const Color& color);
    RectangleShape& getShape();
    const RectangleShape& getShape() const;
    
    void updateVisualization(float cellSize);
};

