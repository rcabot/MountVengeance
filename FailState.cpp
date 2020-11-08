#include "FailState.h"
#include "GameStates.h"
#include "Game.h"
#include <iostream>
namespace GameEngine{

	GameState* GameEngine::FailState::update(Game& game)
	{
		game.detectWindowClose();
		std::cout << "you've failed!" << "\n"; 
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
			//restart!
			game.setupNewGame();
			return &GameEngine::respiteState;
		}
		return &GameEngine::failState;
	}
}
