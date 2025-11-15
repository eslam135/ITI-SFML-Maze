#include "Maze.h"
#include <algorithm>
#include <chrono>
#include <queue>
using namespace std;
using namespace sf;

Maze::Maze(int width, int height, float cellSize, Algorithm algorithm) : mazeWidth(width)
    , mazeHeight(height)
    , algorithm(algorithm)
    , cellSize(cellSize)
    , generationComplete(false)
    , currentCell(1, 1)
    , m_rng(chrono::steady_clock::now().time_since_epoch().count())
{
    reset();
}

void Maze::reset()
{
    cells.clear();
    cells.resize(mazeHeight);
    
    //Resizing then resetting, NEEDS TO BE CALLED FROM HERE!!!

    for (int y = 0; y < mazeHeight; y++)
    {
        cells[y].resize(mazeWidth);
        for (int x = 0; x < mazeWidth; x++)
        {
            cells[y][x] = make_unique<Cell>(x, y, cellSize);
            cells[y][x]->setType(CellType::WALL);
            cells[y][x]->setVisited(false);
            cells[y][x]->setRevealed(false);
        }
    }
    
    if (algorithm == Algorithm::DFS)
    {
        resetDFS();
    }
    else
    {
        resetPrim();
    }
}

void Maze::resetDFS()
{
    currentCell = Vector2i(1, 1);
    Cell* startCell = getCell(1, 1);
    if (startCell)
    {
        startCell->setType(CellType::PATH);
        startCell->setVisited(true);
        startCell->setRevealed(true);
    }
    
    m_stack = stack<Vector2i>();
    m_stack.push(currentCell);
    generationComplete = false;
}

void Maze::resetPrim()
{
    int startX = 1;
    int startY = 1;
    
    Vector2i start(startX, startY);
    
    Cell* startCell = getCell(startX, startY);
    if (startCell)
    {
        startCell->setType(CellType::PATH);
        startCell->setVisited(true);
        startCell->setRevealed(true);
    }
    
    m_inMaze.clear();
    m_inMaze.push_back(start);
    
    m_frontier.clear();
    addToFrontier(startX, startY);
    
    generationComplete = false;
}

void Maze::addToFrontier(int x, int y)
{
    int directions[4][2] = {{0, -2}, {2, 0}, {0, 2}, {-2, 0}};
    
    for (int i = 0; i < 4; i++)
    {
        int nx = x + directions[i][0];
        int ny = y + directions[i][1];
        
        if (isValidCell(nx, ny))
        {
            Cell* cell = getCell(nx, ny);
            if (cell && !cell->isVisited())
            {
                bool inFrontier = false;
                for (Edge& edge : m_frontier)
                {
                    if (edge.to.x == nx && edge.to.y == ny)
                    {
                        inFrontier = true;
                        break;
                    }
                }
                
                bool b_inMaze = false;
                for (Vector2i& pos : m_inMaze)
                {
                    if (pos.x == nx && pos.y == ny)
                    {
                        b_inMaze = true;
                        break;
                    }
                }
                
                if (!inFrontier && !b_inMaze)
                {
                    m_frontier.push_back(Edge(Vector2i(x, y), Vector2i(nx, ny)));
                }
            }
        }
    }
}

vector<Vector2i> Maze::getNeighbors(int x, int y) const
{
    vector<Vector2i> neighbors;

    int directions[4][2] = {{0, -2}, {2, 0}, {0, 2}, {-2, 0}}; // Step size of 2 to make sure that there's always a wall between different cells regarding of the current states to give it a prper maze feeling.
    
    for (int i = 0; i < 4; i++)
    {
        int nx = x + directions[i][0];
        int ny = y + directions[i][1];
        
        if (isValidCell(nx, ny)) // Double Checking validation to avoid unneeded stack calls.
        {
            const Cell* cell = getCell(nx, ny);
            if (cell && !cell->isVisited())
            {
                neighbors.push_back(Vector2i(nx, ny));
            }
        }
    }
    
    return neighbors;
}


bool Maze::isValidCell(int x, int y) const
{
    return x >= 0 && x < mazeWidth && y >= 0 && y < mazeHeight;
}

void Maze::removeWall(Cell& current, Cell& next)
{
    Vector2i currentPos = current.getPosition();
    Vector2i nextPos = next.getPosition();
    
    int midX = (currentPos.x + nextPos.x) / 2;
    int midY = (currentPos.y + nextPos.y) / 2;
    
    if (isValidCell(midX, midY) && getCell(midX, midY))
    {
        getCell(midX, midY)->setType(CellType::PATH);
        getCell(midX, midY)->setRevealed(true);
    }
}

void Maze::generateDFSStep()
{
    if (generationComplete)
    {
        return;
    }
    
    if (!m_stack.empty())
    {
        currentCell = m_stack.top();
        Cell* current = getCell(currentCell.x, currentCell.y);
        
        if (!current)
        {
            generationComplete = true;
            return;
        }
        
        vector<Vector2i> neighbors = getNeighbors(currentCell.x, currentCell.y);
        
        if (!neighbors.empty())
        {
            uniform_int_distribution<int> dist(0, neighbors.size() - 1);
            Vector2i next = neighbors[dist(m_rng)];
            
            Cell* nextCell = getCell(next.x, next.y);
            if (nextCell)
            {
                removeWall(*current, *nextCell);
                
                nextCell->setType(CellType::PATH);
                nextCell->setVisited(true);
                nextCell->setRevealed(true);
                
                m_stack.push(next);
            }
        }
        else
        {
            m_stack.pop();
        }
    }
    else
    {
        generationComplete = true;
    }
}

void Maze::generatePrimStep()
{
    if (generationComplete)
    {
        return;
    }
    
    if (m_frontier.empty())
    {
        generationComplete = true;
        return;
    }
    
    uniform_int_distribution<int> dist(0, (m_frontier.size()) - 1);
    int edgeIndex = dist(m_rng);
    Edge selectedEdge = m_frontier[edgeIndex];
    
    Vector2i to = selectedEdge.to;
    Vector2i from = selectedEdge.from;
    
    Cell* toCell = getCell(to.x, to.y);
    Cell* fromCell = getCell(from.x, from.y);
    
    m_frontier.erase(m_frontier.begin() + edgeIndex);
    
    if (toCell && !toCell->isVisited() && fromCell)
    {
        toCell->setType(CellType::PATH);
        toCell->setVisited(true);
        toCell->setRevealed(true);
        
        removeWall(*fromCell, *toCell);
        
        m_inMaze.push_back(to);
        
        addToFrontier(to.x, to.y);
    }
    
    m_frontier.erase(
        remove_if(m_frontier.begin(), m_frontier.end(),
            [this](const Edge& e) {
                Cell* cell = getCell(e.to.x, e.to.y);
                return cell && cell->isVisited();
            }),
        m_frontier.end()
    );
    
    if (m_frontier.empty())
    {
        generationComplete = true;
    }
}

void Maze::generateStep()
{
    if (algorithm == Algorithm::DFS)
    {
        generateDFSStep();
    }
    else
    {
        generatePrimStep();
    }
}

void Maze::generateComplete()
{
    while (!generationComplete)
    {
        generateStep();
    }
}

void Maze::setAlgorithm(Algorithm algorithm)
{
    this->algorithm = algorithm;
    reset();
}

int Maze::getWidth() const
{
    return mazeWidth;
}

int Maze::getHeight() const
{
    return mazeHeight;
}

float Maze::getCellSize() const
{
    return cellSize;
}

Cell* Maze::getCell(int x, int y)
{
    if (!isValidCell(x, y))
    {
        return nullptr;
    }
    return cells[y][x].get();
}

const Cell* Maze::getCell(int x, int y) const
{
    if (!isValidCell(x, y))
    {
        return nullptr;
    }
    return cells[y][x].get();
}

bool Maze::isGenerationComplete() const
{
    return generationComplete;
}

void Maze::draw(RenderWindow& window, const Texture& blockTexture) const
{
    for (int y = 0; y < mazeHeight; y++)
    {
        for (int x = 0; x < mazeWidth; x++)
        {
            const Cell* cell = getCell(x, y);
            if (cell)
            {
                if (cell->getType() == CellType::WALL)
                {
                    Sprite sprite(blockTexture);
                    sprite.setPosition(cell->getShape().getPosition());
                    Vector2u textureSize = blockTexture.getSize();
                    if (textureSize.x > 0 && textureSize.y > 0)
                    {
                        sprite.setScale(cellSize / textureSize.x, cellSize / textureSize.y);
                    }
                    window.draw(sprite);
                }
                else if (cell->isRevealed())
                {
                    window.draw(cell->getShape());
                }
                else //Base Case if the cell is not visited or not wall for some reason just draw a block
                {
                    Sprite sprite(blockTexture);
                    sprite.setPosition(cell->getShape().getPosition());
                    Vector2u textureSize = blockTexture.getSize();
                    if (textureSize.x > 0 && textureSize.y > 0)
                    {
                        sprite.setScale(cellSize / textureSize.x, cellSize / textureSize.y);
                    }
                    window.draw(sprite);
                }
            }
        }
    }
}
