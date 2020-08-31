#include "Ball.h"

Entities::Ball::Ball(float startX, float startY)
{
    position.x = startX;
    position.y = startY;

    ballShape.setSize(sf::Vector2f(10, 10));
    ballShape.setPosition(position);
}

sf::FloatRect Entities::Ball::getPosition()
{
    return ballShape.getGlobalBounds();
}

sf::RectangleShape Entities::Ball::getShape()
{
    return ballShape;
}

float Entities::Ball::getXVelocity()
{
    return xVelocity;
}

void Entities::Ball::reboundSides()
{
    xVelocity = -xVelocity;
}

void Entities::Ball::reboundBatOrTop()
{
    position.y -= (yVelocity * 1);
    yVelocity = -yVelocity;

}

void Entities::Ball::hitBottom()
{
    position.y = 1;
    position.x = 500;
}

void Entities::Ball::update()
{
    // Update the ball position variables
    position.y += yVelocity;
    position.x += xVelocity;

    // Move the ball and the bat
    ballShape.setPosition(position);
}