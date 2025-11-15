#pragma once

#include "Cell.h"
#include <vector>
#include <stack>
#include <random>
#include <memory>
using namespace std;
using namespace sf;

class Maze
{
public:
    enum class Algorithm
    {
        DFS,
        PRIM
    };
    
private:
    int mazeWidth;
    int mazeHeight;
    Algorithm algorithm;
    
    vector<vector<unique_ptr<Cell>>> cells; //dynamic alloc STOP OVERCOMPLICATING WITH USING DOUBLE POINTERS !!!!!!!!!!!!!!!!
    float cellSize;
    bool generationComplete;
    
    //DFS DATA
    stack<Vector2i> m_stack;
    Vector2i currentCell;
    
    //PRIM DATA
    struct Edge
    {
        Vector2i from;
        Vector2i to;
        
        Edge(Vector2i f, Vector2i t) : from(f), to(t){}
    };
    
    vector<Edge> m_frontier;
    vector<Vector2i> m_inMaze;
    //C++ RNG 
    mt19937 m_rng;
    
    vector<Vector2i> getNeighbors(int x, int y) const;
    void removeWall(Cell& current, Cell& next);
    bool isValidCell(int x, int y) const;
    

    //Only accessible from inside the class "Don't overuse them"
    //Only reset and generate using the main reset and generate not indie fxns otherwise it goes kaboom
    void generateDFSStep();
    void resetDFS();
    
    void generatePrimStep();
    void resetPrim();
    void addToFrontier(int x, int y);

public:
    Maze(int width, int height, float cellSize, Algorithm algorithm = Algorithm::DFS);
    
    void generateStep();
    void generateComplete();
    void reset();
    void setAlgorithm(Algorithm algorithm);
    
    int getWidth() const;
    int getHeight() const;
    float getCellSize() const;
    
    Cell* getCell(int x, int y);
    const Cell* getCell(int x, int y) const; //readonly
    
    bool isGenerationComplete() const;
    
    void draw(RenderWindow& window, const Texture& blockTexture) const;
};

