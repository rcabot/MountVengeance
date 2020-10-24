#pragma once
#include <entt/entity/registry.hpp>

namespace Component {

	struct Bat { float speed; };
	struct Ball {};
	struct Breakable { int breakState = 1; };
	struct BoxCollider { float width, height; };
	struct Position { float x, y; };
	struct FixedSpeedBody { float xVelocity, yVelocity; };
	struct House { };
	struct Enemy { 
		bool shootsBalls = false;
		float shootBallTimer = 0.0f;
		float shootBallAtTime = 10.0f;
	};
	struct Sprite { int index = -1; };

}