#pragma once


namespace GameEngine {

	class Game;
	class GameState
	{

	public:
		virtual GameState* update(Game& game) = 0;


	};
}

