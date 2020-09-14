#pragma once

#include "Component.h"
#include <entt/entity/registry.hpp>
#include <SFML/Graphics.hpp>

namespace Game {


	class Game
	{
	public:
		Game(sf::RenderWindow& w) : window(w){}
		void run();
	private:
		sf::RenderWindow& window;
		entt::registry registry;

		void moveBat(entt::registry& registry, const int windowWidth);

		sf::FloatRect reverseRectUntilNonIntersecting(const Component::FixedSpeedBody& b,
			const sf::FloatRect& bodyRect, const sf::FloatRect& intersectingRect);

		float SweptAABB(
			float vx,
			float vy,
			const sf::FloatRect b1,
			const sf::FloatRect b2,
			float& normalx,
			float& normaly);

		sf::FloatRect GetSweptBroadphaseBox(const sf::FloatRect& b, const float vx, const float vy);

		void updatePhysics(entt::registry& registry, const int windowHeight, const int windowWidth, const float delta);

		void removeDestroyedBreakables(entt::registry& registry);

		void render(entt::registry& registry, sf::RenderWindow& window);
	};
}

