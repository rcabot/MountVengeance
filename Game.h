#pragma once

#include "Component.h"

#include <entt/entity/registry.hpp>
#include <SFML/Graphics.hpp>

namespace GameEngine {


	class GameState;
	class Game
	{
	public:
		Game(sf::RenderWindow& w, int FPS);
		void run();
		void initialSetup();
		void generateGoblinArmy(const float& sizeX, const float& sizeY, const sf::FloatRect& rect, const float& spacing);
		void detectWindowClose();
		void moveBat();
		void updatePhysics(const float delta);
		void removeDestroyedBreakables();



	private:
		sf::RenderWindow& window;
		GameState& state;
		entt::registry registry;
		const int windowWidth;
		const int windowHeight;
		const double SECONDS_PER_UPDATE;


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



		void drawSceneObjects(entt::registry& registry, sf::RenderWindow& window);
		void renderAll();
		void updateGameState();
	};
}