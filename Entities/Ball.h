#pragma once
#include <SFML/Graphics.hpp>

namespace Entities {

	class Ball
	{
        private:
            sf::Vector2f position;

            // A RectangleShape object called ref
            sf::RectangleShape ballShape;

            float xVelocity = 10.0f;
            float yVelocity = 10.0f;

        public:
            Ball(float startX, float startY);

            sf::FloatRect getPosition();

            sf::RectangleShape getShape();

            float getXVelocity();

            void reboundSides();

            void reboundBatOrTop();

            void hitBottom();

            void update();
	};
}

