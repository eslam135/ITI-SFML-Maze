#pragma once

#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;


class Player
{
private:
    Vector2f position;
    float radius;
    float cellSize;
    
    Texture idleTexture;
    Texture walkTexture;
    Sprite sprite;
    
    bool isMoving;
    bool usingIdle;

    float animationTime; //
    float frameTime; //0.1f; edit it later fi needed

    int currentFrame;
    int frameCount;

    int frameWidth;
    int frameHeight;
    
    bool facingRight;
    
    void updateAnimation(float deltaTime);
    void updateSpriteFrame();

public:
    Player(float x, float y, float cellSize);
    
    Vector2f getPosition() const;
    void setPosition(float x, float y);
    void setPosition(const Vector2f& position);
    
    float getRadius() const;
    Vector2f getCenter() const;
    
    void update(float deltaTime);
    void setMoving(bool moving);
    void setFacingRight(bool facingRight);
    
    void draw(RenderWindow& window) const;
    
    FloatRect getBounds() const;
};

