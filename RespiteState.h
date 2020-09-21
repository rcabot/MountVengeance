#pragma once
#include "GameState.h"
namespace GameEngine{

	class RespiteState : public GameState
	{
	public:
		GameState* update(Game& game) override;
	};
}

