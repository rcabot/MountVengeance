#include "RespiteState.h"
#include "GameStates.h"
#include "Game.h"

GameEngine::GameState* GameEngine::RespiteState::update(Game& game)
{
	game.removeAllBalls();
	game.generateGoblinArmy(0.0f);
	game.spawnBalls(1);
	return &GameEngine::defenceState;
}
