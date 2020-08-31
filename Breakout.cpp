// Anything after // is a comment not actual C++ code
// Comments are important and I use them to explain things
// Why not read the comments in this code

// These "include" code from the C++ library and SFML too
#include <sstream>
#include <iostream>
#include <math.h> 
#include <entt/entity/registry.hpp>
#include <SFML/Graphics.hpp>
#define PI 3.14159265f

struct Bat { float speed; };
struct Ball {};
struct Brick { int breakState = 1; };
struct Size { float width, height; };
struct Position { float x, y; };
struct FixedVelocityBody { float xVelocity, yVelocity; };

float InvLerp(float a, float b, float v) {
    return (v - a) / (b - a);
}

float Lerp(float a, float b, float t) {
    return (1.0f - t) * a + b * t;
}

float Remap(float iMin, float iMax, float oMin, float oMax, float v) {
    float t = InvLerp(iMin, iMax, v);
    return Lerp(oMin, oMax, t);
}

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
    registry.emplace<Size>(bat, 250.0f, 5.0f);
    registry.emplace<Position>(bat, windowWidth / 2.0f, windowHeight - 20.0f);

    // balls
    const int startingBalls = 3;
    for (int i = 0; i < startingBalls; i++)
    {
        auto ball = registry.create();
        registry.emplace<Ball>(ball);
        registry.emplace<Size>(ball, 5.0f, 5.0f);
        registry.emplace<Position>(ball, i * 50.0f + windowWidth / 2.0f, windowHeight / 2.0f);
        registry.emplace<FixedVelocityBody>(ball, 5.0f, 5.0f);
    }

    // bricks
    const float bricksSizeX = 40.0f;
    const float bricksSizeY = 10.0f;
    const float spacing = 5.0f;
    for (float x = bricksSizeX; x < windowWidth-bricksSizeX; x += bricksSizeX+spacing)
    {
        for (float y = bricksSizeY; y < windowHeight/2.0f; y += bricksSizeY+spacing)
        {
            auto brick = registry.create();
            registry.emplace<Brick>(brick);
            registry.emplace<Size>(brick, bricksSizeX, bricksSizeY);
            registry.emplace<Position>(brick, x, y);
        }
    }

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
                pos.y -= (vel.yVelocity * 3);
                vel.yVelocity *= -1;

                // alter the angle of the ball trajectory depending on the position on the bat
                float speed = hypot(vel.xVelocity, vel.yVelocity);
                float angle = Remap(batRect.left,batRect.left+batRect.width,PI/8.0f, PI*7.0f/8.0f,rect.left-rect.width/2.0f);
                float newXVelocity = -cos(-angle);
                float newYVelocity = sin(-angle);
                
                bool negativeY = vel.yVelocity < 0;
                vel.yVelocity = negativeY ? newYVelocity * speed : -newYVelocity * speed;
                vel.xVelocity = newXVelocity * speed;
            }

            // Has the ball hit any brick?

            registry.view<Brick, Size, Position>().each([&](auto entity, Brick& b, Size& brickSize, Position& brickPosition) {

                sf::FloatRect brickRect(sf::Vector2f(brickPosition.x, brickPosition.y), sf::Vector2f(brickSize.width, brickSize.height));
                if (rect.intersects(brickRect))
                {
                    // Hit detected so reverse the ball
                    pos.y -= (vel.yVelocity * 2);
                    vel.yVelocity *= -1;
                    b.breakState--;
                }
            });
        });
        
        // destroy bricks!
        registry.view<Brick>().each([&](auto entity, Brick& b) {
            if (b.breakState < 1) {
                // entt does allow destruction within loop (but only current entity)
                registry.destroy(entity);
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