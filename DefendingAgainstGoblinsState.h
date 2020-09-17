#pragma once
#include "GameState.h"
namespace GameEngine {
	class DefendingAgainstGoblinsState : public GameState
	{
	public:
		GameState& update(Game& game) override;
	};
}

