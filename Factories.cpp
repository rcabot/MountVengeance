#include "Factories.h"
#include "Component.h"

entt::entity Factory::makeBat(entt::registry& registry, const float sizeX, const float sizeY)
{
	auto bat = registry.create();
	registry.emplace<Component::Bat>(bat, 10.0f);
	registry.emplace<Component::BoxCollider>(bat, 250.0f, 5.0f);
	registry.emplace<Component::Position>(bat, sizeX, sizeY);
	return bat;
}

entt::entity Factory::makeBall(entt::registry& registry, const float posX, const float posY, const float initVelX, const float initVelY)
{
	const float sizeX = 5.0f;
	const float sizeY = 5.0f;
	auto ball = registry.create();
	registry.emplace<Component::Ball>(ball);
	registry.emplace<Component::BoxCollider>(ball, sizeX, sizeY);
	registry.emplace<Component::Position>(ball, posX, posY);
	registry.emplace<Component::FixedSpeedBody>(ball, initVelX, initVelY);
	return ball;
}

entt::entity Factory::makeGoblin(entt::registry& registry, const float bricksSizeX, const float bricksSizeY, const float x, const float y)
{
	auto goblin = registry.create();
	registry.emplace<Component::Breakable>(goblin);
	registry.emplace<Component::BoxCollider>(goblin, bricksSizeX, bricksSizeY);
	registry.emplace<Component::Position>(goblin, x, y);
	return goblin;
}


entt::entity Factory::makeHouse(entt::registry& registry, const float bricksSizeX, const float bricksSizeY, const float x, const float y)
{
	auto house = registry.create();
	registry.emplace<Component::Breakable>(house);
	registry.emplace<Component::House>(house);
	registry.emplace<Component::BoxCollider>(house, bricksSizeX, bricksSizeY);
	registry.emplace<Component::Position>(house, x, y);
	return house;
}
