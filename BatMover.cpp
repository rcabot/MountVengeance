#include "BatMover.h"

Entities::BatMover::BatMover(Entities::position& p, Entities::speed& s) : batPosition(p),batSpeed(s)
{
}

void Entities::BatMover::update()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        // move left...
        batPosition.x -= batSpeed.x;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        // move right...
        batPosition.x += batSpeed.x;
    }
}
