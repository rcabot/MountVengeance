#pragma once

#include "Component.h"
#include "SpriteRenderer.h"
#include <entt/entity/registry.hpp>
#include <SFML/Graphics.hpp>

namespace GameEngine {


	class GameState;
	class Game
	{
	public:
		Game(sf::RenderWindow& w, int FPS);
		void run();
		bool armyDefeated();
		void setupNewGame();
		void spawnBalls(const int& startingBalls);
		void generateGoblinArmy(const float& sizeX, const float& sizeY, const sf::FloatRect& rect, const float& spacing);
		void generateGoblinArmy(const float magnitude);
		void detectWindowClose();
		void moveBat();
		void updatePhysics(const float delta);
		void removeDestroyedBreakables();
		void removeAllBalls();
		void updateEnemies();
		bool villageDestroyed();
	private:

		sf::Transform worldTransform;
		sf::RenderWindow& window;
		SpriteSheetSpriteRenderer spriteRenderer;
		GameState* state;
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
		bool collidersHaveCommonLayer(const Component::BoxCollider& c1, const Component::BoxCollider& c2);
	};
}