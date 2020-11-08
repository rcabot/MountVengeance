#include "Factories.h"
#include "Component.h"

entt::entity Factory::makeBat(entt::registry& registry, const float sizeX, const float sizeY)
{
	auto bat = registry.create();
	registry.emplace<Component::Bat>(bat, 10.0f);
	registry.emplace<Component::BoxCollider>(bat, 250.0f, 5.0f);
	registry.emplace<Component::Position>(bat, sizeX, sizeY);
	registry.emplace<Component::Sprite>(bat, -1);
	return bat;
}

entt::entity Factory::makeBall(entt::registry& registry, const float posX, const float posY, const float initVelX, const float initVelY)
{
	const float sizeX = 16.0f;
	const float sizeY = 16.0f;
	auto ball = registry.create();
	registry.emplace<Component::Ball>(ball);
	registry.emplace<Component::BoxCollider>(ball, sizeX, sizeY);
	registry.emplace<Component::Position>(ball, posX, posY);
	registry.emplace<Component::FixedSpeedBody>(ball, initVelX, initVelY);
	registry.emplace<Component::Sprite>(ball, -1);
	return ball;
}

entt::entity Factory::makeEnemy(entt::registry& registry, const float bricksSizeX, const float bricksSizeY, const float x, const float y, const bool canShootBalls)
{
	auto enemy = registry.create();
	registry.emplace<Component::Enemy>(enemy, canShootBalls,rand());
	registry.emplace<Component::Breakable>(enemy);
	registry.emplace<Component::BoxCollider>(enemy, bricksSizeX, bricksSizeY,(unsigned int)Component::CollisionLayer::Enemies);
	registry.emplace<Component::Position>(enemy, x, y);
	registry.emplace<Component::Sprite>(enemy, 1);
	return enemy;
}


entt::entity Factory::makeHouse(entt::registry& registry, const float bricksSizeX, const float bricksSizeY, const float x, const float y)
{
	auto house = registry.create();
	registry.emplace<Component::Breakable>(house);
	registry.emplace<Component::House>(house);
	registry.emplace<Component::BoxCollider>(house, bricksSizeX, bricksSizeY);
	registry.emplace<Component::Position>(house, x, y);
	registry.emplace<Component::Sprite>(house, 0);
	return house;
}
