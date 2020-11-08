#include "RespiteState.h"
#include "GameStates.h"
#include "Game.h"

GameEngine::GameState* GameEngine::RespiteState::update(Game& game)
{
	game.incrementLevel();
	game.removeAllBalls();
	game.generateGoblinArmy(0.0f);
	game.setCurrentMaxBalls(game.getBallShootingEnemyCount());
	game.progressEnemyBallSpawnTimers();
	return &GameEngine::defenceState;
}
