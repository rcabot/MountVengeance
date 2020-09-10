// Anything after // is a comment not actual C++ code
// Comments are important and I use them to explain things
// Why not read the comments in this code

// These "include" code from the C++ library and SFML too
#include <sstream>
#include <iostream>
#include <math.h> 
#include <limits>
#include <entt/entity/registry.hpp>
#include <SFML/Graphics.hpp>
#include "Breakout.h"
#define PI 3.14159265f

struct Bat { float speed; };
struct Ball {};
struct Brick { int breakState = 1; };
struct Size { float width, height; };
struct Position { float x, y; };
struct FixedSpeedBody { float xVelocity, yVelocity; };

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

entt::entity makeBat(entt::registry& registry, const float sizeX, const float sizeY) {
    auto bat = registry.create();
    registry.emplace<Bat>(bat, 10.0f);
    registry.emplace<Size>(bat, 250.0f, 5.0f);
    registry.emplace<Position>(bat, sizeX, sizeY);
    return bat;
}

entt::entity makeBall(entt::registry& registry, const float posX, const float posY, const float initVelX, const float initVelY)
{
	const float sizeX = 5.0f;
	const float sizeY = 5.0f;
    auto ball = registry.create();
    registry.emplace<Ball>(ball);
    registry.emplace<Size>(ball, sizeX, sizeY);
    registry.emplace<Position>(ball, posX, posY);
    registry.emplace<FixedSpeedBody>(ball, initVelX, initVelY);
    return ball;
}

entt::entity makeBrick(entt::registry& registry, const float bricksSizeX, const float bricksSizeY, const float x, const float y)
{
    auto brick = registry.create();
    registry.emplace<Brick>(brick);
    registry.emplace<Size>(brick, bricksSizeX, bricksSizeY);
    registry.emplace<Position>(brick, x, y);
    return brick;
}

void moveBat(entt::registry& registry, const int windowWidth)
{
    registry.view<Bat, Position, Size>().each([&](auto entity, Bat& bat, Position& pos, Size& size) {

        sf::FloatRect rect(sf::Vector2f(pos.x, pos.y), sf::Vector2f(size.width, size.height));
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && rect.left > 0)
        {
            // move left...
            pos.x -= bat.speed;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && rect.left + rect.width < windowWidth)
        {
            // move right...
            pos.x += bat.speed;
        }
    });
}

sf::FloatRect reverseRectUntilNonIntersecting(const FixedSpeedBody& b, 
    const sf::FloatRect& bodyRect, const sf::FloatRect& intersectingRect) {
    sf::FloatRect rr = bodyRect;
    const float reverseSpeedScale = 0.1f;
    while (rr.intersects(intersectingRect)) {
        rr.left = rr.left - (b.xVelocity * reverseSpeedScale);
        rr.top = rr.top - (b.yVelocity * reverseSpeedScale);
    }
    return rr;
}

float SweptAABB(
    float vx,
    float vy, 
    const sf::FloatRect b1, 
    const sf::FloatRect b2, 
    float& normalx, 
    float& normaly) {
    float xInvEntry, yInvEntry;
    float xInvExit, yInvExit;

    // find the distance between the objects on the near and far sides for both x and y 
    if (vx > 0.0f)
    {
        xInvEntry = b2.left - (b1.left + b1.width);
        xInvExit = (b2.left + b2.width) - b1.left;
    }
    else
    {
        xInvEntry = (b2.left + b2.width) - b1.left;
        xInvExit = b2.left - (b1.left + b1.width);
    }

    if (vy > 0.0f)
    {
        yInvEntry = b2.top - (b1.top + b1.height);
        yInvExit = (b2.top + b2.height) - b1.top;
    }
    else
    {
        yInvEntry = (b2.top + b2.height) - b1.top;
        yInvExit = b2.top - (b1.top + b1.height);
    }

	// find time of collision and time of leaving for each axis 
    // (if statement is to prevent divide by zero) 
	float xEntry, yEntry;
	float xExit, yExit;

	if (vx == 0.0f)
	{
		xEntry = -std::numeric_limits<float>::infinity();
		xExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		xEntry = xInvEntry / vx;
		xExit = xInvExit / vx;
	}

	if (vy == 0.0f)
	{
		yEntry = -std::numeric_limits<float>::infinity();
		yExit = std::numeric_limits<float>::infinity();
	}
	else
	{
		yEntry = yInvEntry / vy;
		yExit = yInvExit / vy;
	}
    
    // find the earliest/latest times of collision
    float entryTime = std::max(xEntry, yEntry);
	float exitTime = std::min(xExit, yExit);

	// if there was no collision
	if (entryTime > exitTime || xEntry < 0.0f && yEntry < 0.0f || xEntry > 1.0f || yEntry > 1.0f)
	{
		normalx = 0.0f;
		normaly = 0.0f;
		return 1.0f;
	}
	else // if there was a collision 
	{
		// calculate normal of collided surface
		if (xEntry > yEntry)
		{
			if (xInvEntry < 0.0f)
			{
				normalx = 1.0f;
				normaly = 0.0f;
			}
			else
			{
				normalx = -1.0f;
				normaly = 0.0f;
			}
		}
		else
		{
			if (yInvEntry < 0.0f)
			{
				normalx = 0.0f;
				normaly = 1.0f;
			}
			else
			{
				normalx = 0.0f;
				normaly = -1.0f;
			}
		} // return the time of collisionreturn entryTime; 
	}
    return entryTime;
}

sf::FloatRect GetSweptBroadphaseBox(const sf::FloatRect& b, const float vx, const float vy)
{
    sf::FloatRect broadphasebox;
	broadphasebox.left = vx > 0 ? b.left : b.left + vx;
	broadphasebox.top = vy > 0 ? b.top : b.top + vy;
	broadphasebox.width = vx > 0 ? vx + b.width : b.width - vx;
	broadphasebox.height = vy > 0 ? vy + b.height : b.height - vy;

	return broadphasebox;
}

void updatePhysics(entt::registry& registry, const int windowHeight, const int windowWidth, const float delta)
{
	auto bodyView = registry.view<FixedSpeedBody, Position, Size>();
	auto brickView = registry.view<Brick, Size, Position>();
	auto batView = registry.view<Bat, Position, Size>();
    for (auto bodyEntity : bodyView) {

		auto& vel = bodyView.get<FixedSpeedBody>(bodyEntity);
		auto& pos = bodyView.get<Position>(bodyEntity);
		auto& size = bodyView.get<Size>(bodyEntity);
        

		sf::FloatRect rect(sf::Vector2f(pos.x, pos.y), sf::Vector2f(size.width, size.height));
		sf::FloatRect broadphasebox = GetSweptBroadphaseBox(rect,vel.xVelocity*delta,vel.yVelocity * delta);
		
        for (auto batEntity : batView) {

			auto& batPosition = batView.get<Position>(batEntity);
			auto& batSize = batView.get<Size>(batEntity);
			sf::FloatRect batRect(sf::Vector2f(batPosition.x, batPosition.y), sf::Vector2f(batSize.width, batSize.height));
			// Has the ball hit the bat?
			if (broadphasebox.intersects(batRect))
			{
                // Hit detected so reverse the ball
				float normalx, normaly;
				float collisiontime = SweptAABB(vel.xVelocity * delta, vel.yVelocity * delta, rect, batRect, normalx, normaly);

				if (normalx == 0.0f && normaly == 0.0f) continue;

				pos.x += vel.xVelocity * collisiontime * delta;
				pos.y += vel.yVelocity * collisiontime * delta;
				float remainingtime = 1.0f - collisiontime;

				if (abs(normalx) > 0.0001f) vel.xVelocity *= -1;
				if (abs(normaly) > 0.0001f) vel.yVelocity *= -1;


				// alter the angle of the ball trajectory depending on the position on the bat
				float speed = hypot(vel.xVelocity, vel.yVelocity);
				float angle = Remap(batRect.left, batRect.left + batRect.width, PI / 8.0f, PI * 7.0f / 8.0f, rect.left - rect.width / 2.0f);
				float newXVelocity = -cos(-angle);
				float newYVelocity = sin(-angle);
				bool negativeY = vel.yVelocity < 0;
				vel.yVelocity = negativeY ? newYVelocity * speed : -newYVelocity * speed;
				vel.xVelocity = newXVelocity * speed;

				pos.x += vel.xVelocity * remainingtime * delta;
				pos.y += vel.yVelocity * remainingtime * delta;
                return;
			}
        }

		// Has the ball hit any brick?
        for (auto brickEntity : brickView) {

			auto& b = brickView.get<Brick>(brickEntity);
			auto& brickSize = brickView.get<Size>(brickEntity);
			auto& brickPosition = brickView.get<Position>(brickEntity);

			sf::FloatRect brickRect(sf::Vector2f(brickPosition.x, brickPosition.y), sf::Vector2f(brickSize.width, brickSize.height));

			if (broadphasebox.intersects(brickRect))
			{
				// Hit detected so reverse the ball
				float normalx, normaly;
				float collisiontime = SweptAABB(vel.xVelocity * delta, vel.yVelocity * delta, rect,brickRect, normalx, normaly);

                if (normalx == 0.0f && normaly == 0.0f) continue;

				pos.x += vel.xVelocity * collisiontime * delta;
				pos.y += vel.yVelocity * collisiontime * delta;
				float remainingtime = 1.0f - collisiontime;

				if (abs(normalx) > 0.0001f) vel.xVelocity *= -1;
				if (abs(normaly) > 0.0001f) vel.yVelocity *= -1;

				pos.x += vel.xVelocity * remainingtime * delta;
				pos.y += vel.yVelocity * remainingtime * delta;

				b.breakState--;
                return;
			}
        }

		pos.x += vel.xVelocity * delta;
		pos.y += vel.yVelocity * delta;

		// Handle ball hitting the bottom
		if (rect.top > windowHeight)
		{
			// reverse the ball direction
			pos.y -= (vel.yVelocity * 2 * delta);
			vel.yVelocity *= -1;
		}

		// Handle ball hitting top
		if (rect.top < 0)
		{
			pos.y -= (vel.yVelocity * 2 * delta);
			vel.yVelocity *= -1;
		}

		// Handle ball hitting sides
		if (rect.left < 0 || rect.left + size.width > windowWidth)
		{
			pos.x -= (vel.xVelocity * 2 * delta);
			vel.xVelocity *= -1;
		}
    }


}

void removeDestroyedBricks(entt::registry& registry)
{
    registry.view<Brick>().each([&](auto entity, Brick& b) {
        if (b.breakState < 1) {
            // entt does allow destruction within loop (but only current entity)
            registry.destroy(entity);
        }
    });
}

void render(entt::registry& registry, sf::RenderWindow& window)
{
    registry.view<Size, Position>().each([&](auto entity, Size& size, Position& pos) {
        sf::RectangleShape shape;
        shape.setPosition(pos.x, pos.y);
        shape.setSize(sf::Vector2f(size.width, size.height));
        window.draw(shape);
    });
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
    auto bat = makeBat(registry,windowWidth / 2.0f, windowHeight - 100.0f);

    // balls
    const int startingBalls = 3;
    for (int i = 0; i < startingBalls; i++)
    {
        makeBall(registry, i * 50.0f + windowWidth / 2.0f, windowHeight / 2.0f,5.0f,5.0f);
    }

	// bricks
	const float bricksSizeX = 70.0f;
	const float bricksSizeY = 30.0f;
	const float spacing = 5.0f;
	const float brickstartX = bricksSizeX, brickstartY = bricksSizeY;
	const float birckendX = windowWidth - bricksSizeX, brickendY = windowHeight / 2.0f;
    for (float x = brickstartX; x < birckendX; x += bricksSizeX+spacing)
    {
        for (float y = brickstartY; y < brickendY; y += bricksSizeY+spacing)
        {
            makeBrick(registry, bricksSizeX, bricksSizeY, x, y);
        }
    }

    // houses
	const float houseSizeX = 50.0f;
	const float houseSizeY = 50.0f;
	const float housespacing = 10.0f;
	const float housestartX = houseSizeX;
	const float houseendX = windowWidth - houseSizeX;
	const float houseY = windowHeight - houseSizeY;
	for (float x = brickstartX; x < birckendX; x += bricksSizeX + housespacing)
	{
		makeBrick(registry, houseSizeX, houseSizeY, x, houseY);
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
        moveBat(registry, windowWidth);

        // handle fixed velocity bodies
        const int collisionInterval = 5;
        for (float i = 0; i < collisionInterval; i++)
		{
			updatePhysics(registry, windowHeight, windowWidth, 1.0f / collisionInterval);
        }
        
        // destroy bricks!
        removeDestroyedBricks(registry);


        // Update the HUD text


        // Clear everything from the last run of the while loop
        window.clear();
        
        // Draw our game scene here
        render(registry, window);
        

        // Draw our score


        // Show everything we just drew
        window.display();
    }// This is the end of the "while" loop

    return 0;
}