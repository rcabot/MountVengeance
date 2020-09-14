#pragma once

namespace Component {

	struct Bat { float speed; };
	struct Ball {};
	struct Brick { int breakState = 1; };
	struct Size { float width, height; };
	struct Position { float x, y; };
	struct FixedSpeedBody { float xVelocity, yVelocity; };
}