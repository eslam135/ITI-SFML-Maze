#include "Cell.h"
using namespace std;
using namespace sf;

Cell::Cell(int x, int y, float cellSize) : type(CellType::WALL)
    , visited(false)
    , revealed(false)
    , position(x, y)
{
    shape.setSize(Vector2f(cellSize, cellSize));
    shape.setPosition(x * cellSize, y * cellSize);
    shape.setFillColor(Color::Black);
    shape.setOutlineColor(Color::Black);
    shape.setOutlineThickness(1.0f);
}

CellType Cell::getType() const
{
    return type;
}

void Cell::setType(CellType type)
{
    this->type = type;
    updateVisualization(shape.getSize().x);
}

bool Cell::isVisited() const
{
    return visited;
}

void Cell::setVisited(bool visited)
{
    this->visited = visited;
}

bool Cell::isRevealed() const
{
    return revealed;
}

void Cell::setRevealed(bool revealed)
{
    this->revealed = revealed;
}

Vector2i Cell::getPosition() const
{
    return position;
}

void Cell::setColor(const Color& color)
{
    shape.setFillColor(color);
}

RectangleShape& Cell::getShape()
{
    return shape;
}

const RectangleShape& Cell::getShape() const
{
    return shape;
}

void Cell::updateVisualization(float cellSize)
{
    if (type == CellType::WALL)
    {
        shape.setFillColor(Color(40, 40, 50, 255)); 
    }
    else if (type == CellType::PATH)
    {
        shape.setFillColor(Color(240, 240, 240, 255)); 
    }
    else
    {
        shape.setFillColor(Color(100, 100, 100, 255));
    }
}

