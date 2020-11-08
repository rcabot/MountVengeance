#include "Game.h"
#include "Factories.h"
#include "Utils.h"
#include "GameState.h"
#include "GameStates.h"
#include <sstream>
#include <math.h> 
#include <limits>
#include <iostream>
#include <chrono>
#include <ctime>
#include <random>
#define PI 3.14159265f


const double dt = 0.016;

GameEngine::Game::Game(sf::RenderWindow& w, int FPS) :
	window(w),
	windowWidth(window.getSize().x),
	windowHeight(window.getSize().y),
	SECONDS_PER_UPDATE(1.0f / FPS),
	state(&GameEngine::respiteState) 
{ 
	if (!spriteRenderer.load("spritesheet.png")) {
		std::cout << "file not found...\n";
	}
}
void GameEngine::Game::setCurrentMaxBalls(int amt)
{
	maxBalls = amt;
}

int GameEngine::Game::getBallShootingEnemyCount()
{
	int count = 0;
	registry.view<Component::Enemy>().each([&](auto entity, Component::Enemy& enemy) {
		if (enemy.shootsBalls)
		{
			count++;
		}
	});
	return count;
}

void GameEngine::Game::run()
{
	setupNewGame();

	double t = 0.0;

	auto currentTime = std::chrono::system_clock::now();
	std::cout << currentTime.time_since_epoch().count() << "\n";
	double accumulator = 0.0;

	// This "while" loop goes round and round- perhaps forever
	while (window.isOpen())
	{
		auto newTime = std::chrono::system_clock::now();
		std::chrono::duration<double> frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime.count();

		while (accumulator >= dt) {
			updateGameState();

			accumulator -= dt;
			t += dt;
		}

		renderAll();
	}
}

bool GameEngine::Game::armyDefeated()
{
	auto army = registry.view<Component::Enemy>();
	/*int count = 0;
	for (auto entity : army)
	{
		count++;
	}
	std::cout << count << "\n";*/
	return army.empty();
}

void GameEngine::Game::setupNewGame()
{
	registry.clear();
	// bat
	auto bat = Factory::makeBat(registry, windowWidth / 2.0f, windowHeight - 100.0f);

	// houses
	const float houseSizeX = 64.0f;
	const float houseSizeY = 64.0f;
	const float housespacing = 10.0f;
	const float housestartX = houseSizeX;
	const float houseendX = windowWidth - houseSizeX;
	const float houseY = windowHeight - houseSizeY;
	for (float x = housestartX; x < houseendX; x += houseSizeX + housespacing)
	{
		Factory::makeHouse(registry, houseSizeX, houseSizeY, x, houseY);
	}
}

void GameEngine::Game::progressEnemyBallSpawnTimers()
{
	float t = 0.5f;
	registry.view<Component::Enemy>().each([&](auto entity, Component::Enemy& enemy) {
		if(enemy.shootsBalls)
		{
			enemy.shootBallTimer = enemy.shootBallAtTime - t;
			t += 1.5f;
		}
	});
}

void GameEngine::Game::generateGoblinArmy(const float& sizeX, const float& sizeY, const sf::FloatRect& rect, const float& spacing)
{
	const int skipColumnInterval = 2;
	int columnCount = 0;
	for (float x = rect.left; x < rect.left+rect.width; x += sizeX + spacing)
	{
		if (columnCount % skipColumnInterval == 0) {
			++columnCount;
			continue;
		}
		for (float y = rect.top; y < rect.top+rect.height; y += sizeY + spacing)
		{
			Factory::makeEnemy(registry, sizeX, sizeY, x, y,false);
		}
		++columnCount;
	}
	auto army = registry.view<Component::Enemy>();
	//unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	//std::shuffle(army.begin(), army.end(), std::default_random_engine(seed));

	const int numberOfBallShooters = 2;
	int ballShootersSpawned = 0;

	for (auto& enemyEntity : army)
	{
		auto& position = registry.get<Component::Position>(enemyEntity);
		auto& box = registry.get<Component::BoxCollider>(enemyEntity);
		auto& enemy = registry.get<Component::Enemy>(enemyEntity);
		auto& sprite = registry.get<Component::Sprite>(enemyEntity);
		bool shootsBalls = ballShootersSpawned < numberOfBallShooters;
		enemy.shootsBalls = shootsBalls;
		if (shootsBalls) {
			sprite.index = -1;
			ballShootersSpawned++;
		}
	}
}

void GameEngine::Game::generateGoblinArmy(const float magnitude)
{
	const float bricksSizeX = 64.0f;
	const float bricksSizeY = 32.0f;
	const float spacing = 0.0f;
	const float brickstartX = bricksSizeX;
	const float brickstartY = bricksSizeY;
	const float brickendX = windowWidth - bricksSizeX; 
	const float brickendY = windowHeight / 2.0f;
	sf::FloatRect goblinArmRect(brickstartX, brickstartY, windowWidth - bricksSizeX * 2.0f, (windowHeight / 2.0f) - bricksSizeY * 2.0f);
	generateGoblinArmy(bricksSizeX, bricksSizeY, goblinArmRect, spacing);
}

void GameEngine::Game::detectWindowClose()
{
	sf::Event event;
	while (window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			// Someone closed the window- bye
			window.close();
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		// quit...
		window.close();
	}
}

void GameEngine::Game::renderAll()
{
	// Clear everything from the last run of the while loop
	window.clear();

	// Draw our game scene here
	drawSceneObjects(registry, window);


	// Draw our score


	// Show everything we just drew
	window.display();
}

void GameEngine::Game::updateGameState()
{
	state = state->update(*this);
}

bool GameEngine::Game::collidersHaveCommonLayer(const Component::BoxCollider& c1, const Component::BoxCollider& c2)
{
	return (c1.layer & c2.layer) != 0;
}

bool GameEngine::Game::villageDestroyed()
{
	auto village = registry.view<Component::House>();
	return village.empty();
}

void GameEngine::Game::moveBat()
{
	registry.view<Component::Bat, Component::Position, Component::BoxCollider>().each([&](auto entity, Component::Bat& bat, Component::Position& pos, Component::BoxCollider& size) {

		sf::FloatRect rect(sf::Vector2f(pos.x, pos.y), sf::Vector2f(size.width, size.height));
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && rect.left > 0)
		{
			// move left...
			pos.x -= bat.speed;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && rect.left + rect.width < windowWidth)
		{
			// move right...
			pos.x += bat.speed;
		}
	});
}

sf::FloatRect GameEngine::Game::reverseRectUntilNonIntersecting(const Component::FixedSpeedBody& b, const sf::FloatRect& bodyRect, const sf::FloatRect& intersectingRect)
{
	sf::FloatRect rr = bodyRect;
	const float reverseSpeedScale = 0.1f;
	while (rr.intersects(intersectingRect)) {
		rr.left = rr.left - (b.xVelocity * reverseSpeedScale);
		rr.top = rr.top - (b.yVelocity * reverseSpeedScale);
	}
	return rr;
}

float GameEngine::Game::SweptAABB(float vx, float vy, const sf::FloatRect b1, const sf::FloatRect b2, float& normalx, float& normaly)
{
	float xInvEntry, yInvEntry;
	float xInvExit, yInvExit;

	// find the distance between the objects on the near and far sides for both x and y 
	if (vx > 0.0f)
	{
		xInvEntry = b2.left - (b1.left + b1.width);
		xInvExit = (b2.left + b2.width) - b1.left;
	}
	else
	{
		xInvEntry = (b2.left + b2.width) - b1.left;
		xInvExit = b2.left - (b1.left + b1.width);
	}

	if (vy > 0.0f)
	{
		yInvEntry = b2.top - (b1.top + b1.height);
		yInvExit = (b2.top + b2.height) - b1.top;
	}
	else
	{
		yInvEntry = (b2.top + b2.height) - b1.top;
		yInvExit = b2.top - (b1.top + b1.height);
	}

	// find time of collision and time of leaving for each axis 
	// (if statement is to prevent divide by zero) 
	float xEntry, yEntry;
	float xExit, yExit;

	if (vx == 0.0f)
	{
		xEntry = -std::numeric_limits<float>::infinity();
		xExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		xEntry = xInvEntry / vx;
		xExit = xInvExit / vx;
	}

	if (vy == 0.0f)
	{
		yEntry = -std::numeric_limits<float>::infinity();
		yExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		yEntry = yInvEntry / vy;
		yExit = yInvExit / vy;
	}

	// find the earliest/latest times of collision
	float entryTime = std::max(xEntry, yEntry);
	float exitTime = std::min(xExit, yExit);

	// if there was no collision
	if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f)
	{
		normalx = 0.0f;
		normaly = 0.0f;
		return 1.0f;
	}
	else // if there was a collision 
	{
		// calculate normal of collided surface
		if (xEntry > yEntry)
		{
			if (xInvEntry < 0.0f)
			{
				normalx = 1.0f;
				normaly = 0.0f;
			}
			else
			{
				normalx = -1.0f;
				normaly = 0.0f;
			}
		}
		else
		{
			if (yInvEntry < 0.0f)
			{
				normalx = 0.0f;
				normaly = 1.0f;
			}
			else
			{
				normalx = 0.0f;
				normaly = -1.0f;
			}
		} // return the time of collisionreturn entryTime; 
	}
	return entryTime;
}

sf::FloatRect GameEngine::Game::GetSweptBroadphaseBox(const sf::FloatRect& b, const float vx, const float vy)

{
	sf::FloatRect broadphasebox;
	broadphasebox.left = vx > 0 ? b.left : b.left + vx;
	broadphasebox.top = vy > 0 ? b.top : b.top + vy;
	broadphasebox.width = vx > 0 ? vx + b.width : b.width - vx;
	broadphasebox.height = vy > 0 ? vy + b.height : b.height - vy;

	return broadphasebox;
}

void GameEngine::Game::updatePhysics(const float delta)

{
	auto bodyView = registry.view<Component::FixedSpeedBody, Component::Position, Component::BoxCollider>();
	bool tooManyBalls = bodyView.size() > maxBalls;
	auto colliderView = registry.view<Component::BoxCollider>();
	for (auto bodyEntity : bodyView) {

		auto& vel = bodyView.get<Component::FixedSpeedBody>(bodyEntity);
		auto& pos = bodyView.get<Component::Position>(bodyEntity);
		auto& bodyCollider = bodyView.get<Component::BoxCollider>(bodyEntity);


		sf::FloatRect rect(sf::Vector2f(pos.x, pos.y), sf::Vector2f(bodyCollider.width, bodyCollider.height));
		sf::FloatRect broadphasebox = GetSweptBroadphaseBox(rect, vel.xVelocity * delta, vel.yVelocity * delta);

		float shortestCollisionTime = 2.0f;
		bool collided = false;
		float bestnormalx, bestnormaly;
		entt::entity collidedEntity = entt::null;

		for (auto colliderEntity : colliderView) {
			if (registry.any<Component::FixedSpeedBody>(colliderEntity)) continue; // if this is another body, ignore
			auto& colliderPosition = registry.get<Component::Position>(colliderEntity);
			auto& collider = colliderView.get<Component::BoxCollider>(colliderEntity);

			if (!collidersHaveCommonLayer(collider,bodyCollider)) continue;
			sf::FloatRect batRect(sf::Vector2f(colliderPosition.x, colliderPosition.y), sf::Vector2f(collider.width, collider.height));
			// Has the ball hit the bat?
			if (broadphasebox.intersects(batRect))
			{
				// Hit detected so reverse the ball
				float normalx, normaly;
				float collisiontime = SweptAABB(vel.xVelocity * delta, vel.yVelocity * delta, rect, batRect, normalx, normaly);

				if (normalx == 0.0f && normaly == 0.0f) continue;

				if (collisiontime < shortestCollisionTime) {
					shortestCollisionTime = collisiontime;
					collided = true;
					bestnormalx = normalx;
					bestnormaly = normaly;
					collidedEntity = colliderEntity;
				}
			}
		}

		if (collided) {
			//move ball up to collision point
			pos.x += vel.xVelocity * shortestCollisionTime * delta;
			pos.y += vel.yVelocity * shortestCollisionTime * delta;
			float remainingtime = 1.0f - shortestCollisionTime;

			// implement collision response
			if (abs(bestnormalx) > 0.0001f) vel.xVelocity *= -1;
			if (abs(bestnormaly) > 0.0001f) vel.yVelocity *= -1;

			//todo move functionality into boxcollider (collision response function)
			if (collidedEntity != entt::null && registry.any<Component::Bat>(collidedEntity)) {
				auto& batSize = registry.get<Component::BoxCollider>(collidedEntity);
				auto& batPosition = registry.get<Component::Position>(collidedEntity);
				sf::FloatRect batRect(sf::Vector2f(batPosition.x, batPosition.y), sf::Vector2f(batSize.width, batSize.height));

				// alter the angle of the ball trajectory depending on the position on the bat
				float speed = hypot(vel.xVelocity, vel.yVelocity);
				float angle = Utils::Remap(batRect.left, batRect.left + batRect.width, PI / 8.0f, PI * 7.0f / 8.0f, rect.left - rect.width / 2.0f);
				float newXVelocity = -cos(-angle);
				float newYVelocity = sin(-angle);
				bool negativeY = vel.yVelocity < 0;
				vel.yVelocity = negativeY ? newYVelocity * speed : -newYVelocity * speed;
				vel.xVelocity = newXVelocity * speed;

			}

			// apply remaining movement
			pos.x += vel.xVelocity * remainingtime * delta;
			pos.y += vel.yVelocity * remainingtime * delta;

			// todo move this functionality into boxcollider (oncollide)
			if (collidedEntity != entt::null) {

				if (registry.any<Component::Breakable>(collidedEntity)) {
					auto& breakable = registry.get<Component::Breakable>(collidedEntity);
					breakable.breakState--;
				}
				if (registry.any<Component::Bat>(collidedEntity)) {
					bodyCollider.layer = Component::CollisionLayer::Default + Component::CollisionLayer::Enemies;
				}
			}
			if (tooManyBalls)
			{
				registry.destroy(bodyEntity);
			}
			return;
		}

		bool collidedWithWall = false;

		// no entity collisions, so move the ball!
		pos.x += vel.xVelocity * delta;
		pos.y += vel.yVelocity * delta;

		// Handle ball hitting the bottom
		if (rect.top > windowHeight)
		{
			// reverse the ball direction
			pos.y -= (vel.yVelocity * 2 * delta);
			vel.yVelocity *= -1;
			collidedWithWall = true;
		}

		// Handle ball hitting top
		if (rect.top < 0)
		{
			pos.y -= (vel.yVelocity * 2 * delta);
			vel.yVelocity *= -1;
			collidedWithWall = true;
		}

		// Handle ball hitting sides
		if (rect.left < 0 || rect.left + bodyCollider.width > windowWidth)
		{
			pos.x -= (vel.xVelocity * 2 * delta);
			vel.xVelocity *= -1;
			collidedWithWall = true;
		}


		if (collidedWithWall&&tooManyBalls)
		{
			registry.destroy(bodyEntity);
		}
	}


}

void GameEngine::Game::removeDestroyedBreakables()

{
	registry.view<Component::Breakable>().each([&](auto entity, Component::Breakable& b) {
		if (b.breakState < 1) {
			// entt does allow destruction within loop (but only current entity)
			registry.destroy(entity);
		}
	});
}

void GameEngine::Game::removeAllBalls()
{
	registry.view<Component::Ball>().each([&](auto entity) {
		registry.destroy(entity);
	});
}

void GameEngine::Game::updateEnemies()
{

	registry.view<Component::Enemy, Component::Position>().each([&](auto entity,Component::Enemy& enemy, Component::Position& pos) {
		if (enemy.shootsBalls) enemy.shootBallTimer += dt;
		if (enemy.shootBallTimer > enemy.shootBallAtTime) {
			Factory::makeBall(registry, pos.x, pos.y, 0.0f, 7.0f);
			enemy.shootBallTimer = 0.0f;
		}
	});
}

void GameEngine::Game::drawSceneObjects(entt::registry& registry, sf::RenderWindow& window)
{
	registry.view<Component::BoxCollider, Component::Position, Component::Sprite>().each([&](auto entity, Component::BoxCollider& size, Component::Position& pos, Component::Sprite& sprite) {
		if (sprite.index == -1) {
			sf::RectangleShape shape;
			shape.setFillColor(sf::Color(255, 0, 255, 255));
			shape.setPosition(pos.x, pos.y);
			shape.setSize(sf::Vector2f(size.width, size.height));
			window.draw(shape);
		}
		else {
			spriteRenderer.grabSprite(sf::Vector2u(64, 64), sf::Vector2u(1, 1), sprite.index);
			spriteRenderer.setPosition(pos.x, pos.y);
			spriteRenderer.draw(window, worldTransform);
		}
		
	});
}
