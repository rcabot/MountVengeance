#pragma once

#include <SFML/Graphics.hpp>

namespace Entities {
    struct position {
        float x;
        float y;
    };
    struct speed {
        float x;
        float y;
    };
	class BatRenderer
    {
    private:
        position& position;

        // A RectangleShape object
        sf::RectangleShape batShape;

        float batSpeed = 10.0f;

    public:
        BatRenderer(Entities::position& position);

        sf::FloatRect getPosition();

        sf::RectangleShape getShape();

        void update();

    };


}