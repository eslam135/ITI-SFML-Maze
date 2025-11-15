#include "PathFinder.h"
#include <algorithm>
using namespace std;
using namespace sf;




vector<Vector2i> PathFinder::getValidNeighbors(const Maze& maze, const Vector2i& pos) const
{
    vector<Vector2i> neighbors;
    
    int directions[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}}; //Direct neighbots not like DFS and Prim
    
    for (int i = 0; i < 4; ++i)
    {
        int nx = pos.x + directions[i][0];
        int ny = pos.y + directions[i][1];
        
        const Cell* cell = maze.getCell(nx, ny);
        if (cell && cell->getType() == CellType::PATH)
        {
            neighbors.push_back(Vector2i(nx, ny));
        }
    }
    
    return neighbors;
}

vector<Vector2i> PathFinder::reconstructPath(PathNode* node) const
{
    vector<Vector2i> path;
    PathNode* current = node;
    
    while (current != nullptr)
    {
        path.push_back(current->position);
        current = current->parent;
    }
    
    reverse(path.begin(), path.end());
    return path;
}

vector<Vector2i> PathFinder::findPath(const Maze& maze, const Vector2i& start, const Vector2i& goal) const
{
    vector<Vector2i> path;
    
    if (!maze.getCell(start.x, start.y) || maze.getCell(start.x, start.y)->getType() != CellType::PATH)
    {
        return path; 
    }
    
    if (!maze.getCell(goal.x, goal.y) || maze.getCell(goal.x, goal.y)->getType() != CellType::PATH)
    {
        return path;
    }
    
    queue<PathNode*> queue;
    unordered_set<Vector2i, Vector2iHash> visited;
    vector<PathNode*> nodes; 
    
    PathNode* startNode = new PathNode(start);
    queue.push(startNode);
    nodes.push_back(startNode);
    visited.insert(start);
    
    while (!queue.empty())
    {
        PathNode* current = queue.front();
        queue.pop();
        
        if (current->position == goal)
        {
            path = reconstructPath(current);
            
            for (PathNode* node : nodes)
            {
                delete node;
            }
            
            return path;
        }
        
        vector<Vector2i> neighbors = getValidNeighbors(maze, current->position);
        
        for (const Vector2i& neighbor : neighbors)
        {
            if (visited.find(neighbor) == visited.end())
            {
                visited.insert(neighbor);
                PathNode* neighborNode = new PathNode(neighbor, current, current->distance + 1);
                queue.push(neighborNode);
                nodes.push_back(neighborNode);
            }
        }
    }
    
    for (PathNode* node : nodes)
    {
        delete node;
    }
    
    return path;
}

