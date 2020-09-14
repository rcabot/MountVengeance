// Anything after // is a comment not actual C++ code
// Comments are important and I use them to explain things
// Why not read the comments in this code

// These "include" code from the C++ library and SFML too

#include <SFML/Graphics.hpp>
#include "Game.h"



int main()
{
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int FPS = 60; //The desired FPS. (The number of updates each second).
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Breakout");

	//todo GET RID
    window.setFramerateLimit(FPS);

	Game::Game game{ window };

	game.run();

    return 0;
}