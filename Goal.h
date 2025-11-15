#pragma once

#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;


class Goal
{
private:
    Vector2f position;
    float radius; //12px
    CircleShape shape;

public:
    Goal(float x, float y, float radius);
    
    Vector2f getPosition() const;
    void setPosition(float x, float y);
    void setPosition(const Vector2f& position);
    
    float getRadius() const;
    Vector2f getCenter() const;
    
    void draw(RenderWindow& window) const;
    
    FloatRect getBounds() const;
    bool intersects(const FloatRect& bounds) const;
};

