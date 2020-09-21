#include "DefendingAgainstGoblinsState.h"
#include "Game.h"
#include "GameStates.h"
GameEngine::GameState* GameEngine::DefendingAgainstGoblinsState::update(Game& game)
{

	game.detectWindowClose();

	// move bat
	game.moveBat();

	// handle fixed velocity bodies
	const int collisionInterval = 5;
	for (float i = 0; i < collisionInterval; i++)
	{
		game.updatePhysics(1.0f / collisionInterval);
	}

	// destroy breakables!
	game.removeDestroyedBreakables();

	// todo: move this to tryTransition();

	if (game.armyDefeated()) {
		return &GameEngine::respiteState;
	}
	if (game.villageDestroyed()) {
		return &GameEngine::failState;
	}
	return &GameEngine::defenceState;
}
