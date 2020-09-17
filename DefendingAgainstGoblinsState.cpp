#include "DefendingAgainstGoblinsState.h"
#include "Game.h"
GameEngine::GameState& GameEngine::DefendingAgainstGoblinsState::update(Game& game)
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

	return *this;
}
