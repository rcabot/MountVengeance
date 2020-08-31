#include "Bat.h"

Entities::BatRenderer::BatRenderer(Entities::position& p) : position{ p }
{
    batShape.setSize(sf::Vector2f(50, 5));
    batShape.setPosition(position.x,position.y);
}

sf::FloatRect Entities::BatRenderer::getPosition()
{
    return batShape.getGlobalBounds();
}

sf::RectangleShape Entities::BatRenderer::getShape()
{
    return batShape;
}

void Entities::BatRenderer::update()
{
    batShape.setPosition(position.x,position.y);
}