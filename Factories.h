#pragma once


#include <entt/entity/registry.hpp>
namespace Factory {

	entt::entity makeBat(entt::registry& registry, const float sizeX, const float sizeY);

	entt::entity makeBall(entt::registry& registry, const float posX, const float posY, const float initVelX, const float initVelY);

	entt::entity makeEnemy(entt::registry& registry, const float bricksSizeX, const float bricksSizeY, const float x, const float y, const bool canShootBalls);

	entt::entity makeHouse(entt::registry& registry, const float bricksSizeX, const float bricksSizeY, const float x, const float y);
}
