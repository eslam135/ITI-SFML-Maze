#pragma once

#include "Maze.h"
#include <queue>
#include <vector>
#include <map>
#include <unordered_set>
#include <functional>
using namespace std;
using namespace sf;



struct PathNode
{
    Vector2i position;
    PathNode* parent;
    int distance;
    
    PathNode(Vector2i pos, PathNode* p = nullptr, int dist = 0)
        : position(pos), parent(p), distance(dist) {}
};

class PathFinder
{
private:
    vector<Vector2i> reconstructPath(PathNode* node) const;
    vector<Vector2i> getValidNeighbors(const Maze& maze, const Vector2i& pos) const;
    
    struct Vector2iHash
    {
        size_t operator()(const Vector2i& v) const
        {
            return hash<int>()(v.x) ^ (hash<int>()(v.y) << 1);
        }
    };

public:
    vector<Vector2i> findPath(const Maze& maze, const Vector2i& start, const Vector2i& goal) const;
};

