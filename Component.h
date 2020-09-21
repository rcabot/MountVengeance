#pragma once

namespace Component {

	struct Bat { float speed; };
	struct Ball {};
	struct Breakable { int breakState = 1; };
	struct BoxCollider { float width, height; };
	struct Position { float x, y; };
	struct FixedSpeedBody { float xVelocity, yVelocity; };
	struct House { };
	struct Goblin { };
	struct Sprite { };
}