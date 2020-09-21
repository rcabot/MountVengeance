#pragma once
#include "GameState.h"

namespace GameEngine {

	class FailState : public GameState
	{

	public:
		GameState* update(Game& game) override;
	};
}

