#include "Player.h"
#include <cmath>
#include <iostream>

using namespace std;
using namespace sf;


Player::Player(float x, float y, float cellSize) : position(x, y)
    , radius(cellSize * 0.3f)
    , cellSize(cellSize)
    , isMoving(false)
    , usingIdle(true)
    , animationTime(0.0f)
    , frameTime(0.1f)
    , currentFrame(0)
    , frameCount(0)
    , frameWidth(0)
    , frameHeight(0)
    , facingRight(true)
{
    if (!idleTexture.loadFromFile("assets/Soldier-Idle.png"))
    {
        cerr << "Failed to load Soldier-Idle.png" << endl;
    }
    
    if (!walkTexture.loadFromFile("assets/Soldier-Walk.png"))
    {
        cerr << "Failed to load Soldier-Walk.png" << endl;
    }
    
    Vector2u idleSize = idleTexture.getSize();
    Vector2u walkSize = walkTexture.getSize();
    
    if (idleSize.x > idleSize.y) // if not square "our case" consider that there are multiple frames in the sheet
    {
        frameHeight = idleSize.y;
        frameWidth = frameHeight;
        frameCount = idleSize.x / frameWidth;
    }
    else //base case
    {
        frameWidth = idleSize.x;
        frameHeight = idleSize.y;
        frameCount = 1;
    }
    
    sprite.setTexture(idleTexture);
    sprite.setTextureRect(IntRect(0, 0, frameWidth, frameHeight)); // Make it 0,0 as we're not rotating
    
    float scale = (32 * 6.f) / (float)(frameHeight); // SCALING 
    sprite.setScale(scale, scale); //Scale it equally in both dimensions
    
    sprite.setOrigin(frameWidth / 2.0f, frameHeight / 2.0f);
    
    sprite.setPosition(position.x + radius, position.y + radius);
}

Vector2f Player::getPosition() const
{
    return position;
}

void Player::setPosition(float x, float y)
{
    position.x = x;
    position.y = y;
    sprite.setPosition(position.x + radius, position.y + radius);
}

void Player::setPosition(const Vector2f& position)
{
    this->position = position;
    sprite.setPosition(this->position.x + radius, this->position.y + radius);
}

float Player::getRadius() const
{
    return radius;
}

Vector2f Player::getCenter() const
{
    return Vector2f(
        position.x + radius,
        position.y + radius
    );
}

void Player::setMoving(bool moving)
{
    isMoving = moving;
    
    if (isMoving && usingIdle)
    {
        usingIdle = false;
        sprite.setTexture(walkTexture);
        currentFrame = 0;
        animationTime = 0.0f;
        
        Vector2u walkSize = walkTexture.getSize();
        if (walkSize.x > walkSize.y)
        {
            frameHeight = walkSize.y;
            frameWidth = frameHeight;
            frameCount = walkSize.x / frameWidth;
        }
        else
        {
            frameWidth = walkSize.x;
            frameHeight = walkSize.y;
            frameCount = 1;
        }
        updateSpriteFrame();
    }
    else if (!isMoving && !usingIdle)
    {
        usingIdle = true;
        sprite.setTexture(idleTexture);
        currentFrame = 0;
        animationTime = 0.0f;
        
        Vector2u idleSize = idleTexture.getSize();
        if (idleSize.x > idleSize.y)
        {
            frameHeight = idleSize.y;
            frameWidth = frameHeight;
            frameCount = idleSize.x / frameWidth;
        }
        else
        {
            frameWidth = idleSize.x;
            frameHeight = idleSize.y;
            frameCount = 1;
        }
        updateSpriteFrame();
    } // base case is that is currently using idle while not moving no need to manually handle it
}

void Player::setFacingRight(bool facingRight) //Flip on the X-Axis  
{
    if (this->facingRight != facingRight)
    {
        this->facingRight = facingRight;
        
        Vector2f currentScale = sprite.getScale();
        float scaleY = currentScale.y;
        float scaleX = abs(currentScale.x);
        
        if (this->facingRight)
        {
            sprite.setScale(scaleX, scaleY);
        }
        else
        {
            sprite.setScale(-scaleX, scaleY);
        }
    }
}

void Player::update(float deltaTime)
{
    updateAnimation(deltaTime);
    sprite.setPosition(position.x + radius, position.y + radius);
}

void Player::updateAnimation(float deltaTime)
{
    if (frameCount <= 1)
    {
        return;
    }
    
    animationTime += deltaTime;
    
    if (animationTime >= frameTime)
    {
        animationTime = 0.0f;
        currentFrame = (currentFrame + 1) % frameCount;
        updateSpriteFrame();
    }
}

void Player::updateSpriteFrame()
{
    if (frameWidth > 0 && frameHeight > 0)
    {
        int frameX = currentFrame * frameWidth;
        sprite.setTextureRect(IntRect(frameX, 0, frameWidth, frameHeight));
    }
}

void Player::draw(RenderWindow& window) const
{
    window.draw(sprite);
}

FloatRect Player::getBounds() const
{
    FloatRect spriteBounds = sprite.getGlobalBounds();
    return spriteBounds;
}

