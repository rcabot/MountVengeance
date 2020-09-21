// Anything after // is a comment not actual C++ code
// Comments are important and I use them to explain things
// Why not read the comments in this code

// These "include" code from the C++ library and SFML too

#include <SFML/Graphics.hpp>
#include "Game.h"



int main()
{
    const int windowWidth = 640;
    const int windowHeight = 840;
    const int FPS = 60; //The desired FPS. (The number of updates each second).
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Breakout");

	GameEngine::Game game{ window, 60 };

	game.run();

    return 0;
}