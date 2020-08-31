// Anything after // is a comment not actual C++ code
// Comments are important and I use them to explain things
// Why not read the comments in this code

// These "include" code from the C++ library and SFML too
#include <sstream>
#include <iostream>
#include <entt/entity/registry.hpp>
#include <SFML/Graphics.hpp>

struct Bat { float speed; };
struct Size { float width, height; };
struct Position { float x, y; };
struct FixedVelocityBody { float xVelocity, yVelocity; };
struct Ball {};

int main()
{
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int FPS = 60; //The desired FPS. (The number of updates each second).
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Breakout");
    window.setFramerateLimit(FPS);
    entt::registry registry;

    int score = 0;
    int lives = 3;

    // bat
    auto bat = registry.create();
    registry.emplace<Bat>(bat,10.0f);
    registry.emplace<Size>(bat, 50.0f, 5.0f);
    registry.emplace<Position>(bat, windowWidth / 2.0f, windowHeight - 20.0f);

    // ball
    auto ball = registry.create();
    registry.emplace<Ball>(ball);
    registry.emplace<Size>(ball,5.0f,5.0f);
    registry.emplace<Position>(ball, windowWidth / 2.0f, 1.0f);
    registry.emplace<FixedVelocityBody>(ball,5.0f,5.0f); 

    // bricks

    // font for hud

    // create hud

    // This "while" loop goes round and round- perhaps forever
    while (window.isOpen())
    {
        
        // The next 6 lines of code detect if the window is closed
        // And then shuts down the program
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                // Someone closed the window- bye
                window.close();
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            // quit...
            window.close();
        }

        // move bat
        registry.view<Bat,Position,Size>().each([&](auto entity, Bat& bat, Position& pos, Size& size) {

            sf::FloatRect rect(sf::Vector2f(pos.x, pos.y), sf::Vector2f(size.width, size.height));
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && rect.left > 0)
            {
                // move left...
                pos.x -= bat.speed;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && rect.left+rect.width < windowWidth)
            {
                // move right...
                pos.x += bat.speed;
            }
        });

        auto& batSize = registry.get<Size>(bat);
        auto& batPosition = registry.get<Position>(bat);
        sf::FloatRect batRect(sf::Vector2f(batPosition.x, batPosition.y), sf::Vector2f(batSize.width, batSize.height));

        
        // handle fixed velocity bodies
        registry.view<FixedVelocityBody, Position,Size>().each([&](auto entity, FixedVelocityBody& vel, Position& pos, Size& size) {
            

            sf::FloatRect rect(sf::Vector2f(pos.x,pos.y), sf::Vector2f(size.width,size.height));

            pos.x += vel.xVelocity;
            pos.y += vel.yVelocity;

            // Handle ball hitting the bottom
            if (rect.top > windowHeight)
            {
                // reverse the ball direction
                pos.y -= (vel.yVelocity * 2);
                vel.yVelocity *= -1;
            }

            // Handle ball hitting top
            if (rect.top < 0)
            {
                pos.y -= (vel.yVelocity * 2);
                vel.yVelocity *= -1;
            }

            // Handle ball hitting sides
            if (rect.left < 0 || rect.left + size.width > windowWidth)
            {
                pos.x -= (vel.xVelocity * 2);
                vel.xVelocity *= -1;
            }

            // Has the ball hit the bat?
            if (rect.intersects(batRect))
            {
                // Hit detected so reverse the ball
                pos.y -= (vel.yVelocity * 2);
                vel.yVelocity *= -1;
            }
        });
        



        // Update the HUD text


        // Clear everything from the last run of the while loop
        window.clear();
        
        // Draw our game scene here
        registry.view<Size, Position>().each([&](auto entity, Size& size, Position& pos) {
            sf::RectangleShape shape;
            shape.setPosition(pos.x, pos.y);
            shape.setSize(sf::Vector2f(size.width, size.height));
            window.draw(shape);
        });
        

        // Draw our score


        // Show everything we just drew
        window.display();
    }// This is the end of the "while" loop

    return 0;
}