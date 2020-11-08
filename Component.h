#pragma once
#include <entt/entity/registry.hpp>

namespace Component {

	enum CollisionLayer {
		Default = 1 << 0,
		Enemies = 1 << 1
	};

	struct Bat { float speed; };
	struct Ball {};
	struct Breakable { int breakState = 1; };
	struct BoxCollider { 
		float width, height; 
		unsigned int layer = CollisionLayer::Default;
	};
	struct Position { float x, y; };
	struct FixedSpeedBody { float xVelocity, yVelocity; };
	struct House { };
	struct Enemy { 
		bool shootsBalls = false;
		int randInt;
		float shootBallTimer = 0.0f;
		float shootBallAtTime = 10.0f;
	};
	struct Sprite { int index = -1; };

}