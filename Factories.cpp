#include "Factories.h"
#include "Component.h"

entt::entity Factory::makeBat(entt::registry& registry, const float sizeX, const float sizeY)
{
	auto bat = registry.create();
	registry.emplace<Component::Bat>(bat, 10.0f);
	registry.emplace<Component::Size>(bat, 250.0f, 5.0f);
	registry.emplace<Component::Position>(bat, sizeX, sizeY);
	return bat;
}

entt::entity Factory::makeBall(entt::registry& registry, const float posX, const float posY, const float initVelX, const float initVelY)
{
	const float sizeX = 5.0f;
	const float sizeY = 5.0f;
	auto ball = registry.create();
	registry.emplace<Component::Ball>(ball);
	registry.emplace<Component::Size>(ball, sizeX, sizeY);
	registry.emplace<Component::Position>(ball, posX, posY);
	registry.emplace<Component::FixedSpeedBody>(ball, initVelX, initVelY);
	return ball;
}

entt::entity Factory::makeBrick(entt::registry& registry, const float bricksSizeX, const float bricksSizeY, const float x, const float y)
{
	auto brick = registry.create();
	registry.emplace<Component::Brick>(brick);
	registry.emplace<Component::Size>(brick, bricksSizeX, bricksSizeY);
	registry.emplace<Component::Position>(brick, x, y);
	return brick;
}
