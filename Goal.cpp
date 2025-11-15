#include "Goal.h"
#include <cmath>
using namespace std;
using namespace sf;


Goal::Goal(float x, float y, float radius)
    : position(x, y)
    , radius(radius)
{
    shape.setRadius(this->radius);
    shape.setFillColor(Color(255, 215, 0, 255));
    shape.setOutlineColor(Color(255, 140, 0, 255));
    shape.setOutlineThickness(3.0f);
    shape.setPosition(x, y);
}

Vector2f Goal::getPosition() const
{
    return position;
}

void Goal::setPosition(float x, float y)
{
    position.x = x;
    position.y = y;
    shape.setPosition(x, y);
}

void Goal::setPosition(const Vector2f& position)
{
    this->position = position;
    shape.setPosition(position);
}

float Goal::getRadius() const
{
    return radius;
}

Vector2f Goal::getCenter() const
{
    return Vector2f(
        position.x + radius,
        position.y + radius
    );
}

void Goal::draw(RenderWindow& window) const
{
    window.draw(shape);
}

FloatRect Goal::getBounds() const
{
    return FloatRect(position.x, position.y, radius * 2.0f, radius * 2.0f);
}

bool Goal::intersects(const FloatRect& bounds) const
{
    FloatRect goalBounds = getBounds();
    return goalBounds.intersects(bounds);
}

