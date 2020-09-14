#include "Game.h"
#include "Factories.h"
#include "Utils.h"
#include <sstream>
#include <math.h> 
#include <limits>
#include <iostream>
#define PI 3.14159265f

void Game::Game::run()
{
	const int windowWidth = window.getSize().x;
	const int windowHeight = window.getSize().y;
	// bat
	auto bat = Factory::makeBat(registry, windowWidth / 2.0f, windowHeight - 100.0f);

	// balls
	const int startingBalls = 2;
	for (int i = 0; i < startingBalls; i++)
	{
		Factory::makeBall(registry, i * 50.0f + windowWidth / 2.0f, windowHeight / 2.0f, 5.0f, 5.0f);
	}

	// bricks
	const float bricksSizeX = 70.0f;
	const float bricksSizeY = 30.0f;
	const float spacing = 0.0f;
	const float brickstartX = bricksSizeX, brickstartY = bricksSizeY;
	const float birckendX = windowWidth - bricksSizeX, brickendY = windowHeight / 2.0f;
	for (float x = brickstartX; x < birckendX; x += bricksSizeX + spacing)
	{
		for (float y = brickstartY; y < brickendY; y += bricksSizeY + spacing)
		{
			Factory::makeGoblin(registry, bricksSizeX, bricksSizeY, x, y);
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
		Factory::makeGoblin(registry, houseSizeX, houseSizeY, x, houseY);
	}

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

		// destroy breakables!
		removeDestroyedBreakables(registry);


		// Clear everything from the last run of the while loop
		window.clear();

		// Draw our game scene here
		render(registry, window);


		// Draw our score


		// Show everything we just drew
		window.display();
	}
}

void Game::Game::moveBat(entt::registry& registry, const int windowWidth)
{
	registry.view<Component::Bat, Component::Position, Component::BoxCollider>().each([&](auto entity, Component::Bat& bat, Component::Position& pos, Component::BoxCollider& size) {

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

sf::FloatRect Game::Game::reverseRectUntilNonIntersecting(const Component::FixedSpeedBody& b, const sf::FloatRect& bodyRect, const sf::FloatRect& intersectingRect)
{
	sf::FloatRect rr = bodyRect;
	const float reverseSpeedScale = 0.1f;
	while (rr.intersects(intersectingRect)) {
		rr.left = rr.left - (b.xVelocity * reverseSpeedScale);
		rr.top = rr.top - (b.yVelocity * reverseSpeedScale);
	}
	return rr;
}

float Game::Game::SweptAABB(float vx, float vy, const sf::FloatRect b1, const sf::FloatRect b2, float& normalx, float& normaly)
{
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

sf::FloatRect Game::Game::GetSweptBroadphaseBox(const sf::FloatRect& b, const float vx, const float vy)

{
	sf::FloatRect broadphasebox;
	broadphasebox.left = vx > 0 ? b.left : b.left + vx;
	broadphasebox.top = vy > 0 ? b.top : b.top + vy;
	broadphasebox.width = vx > 0 ? vx + b.width : b.width - vx;
	broadphasebox.height = vy > 0 ? vy + b.height : b.height - vy;

	return broadphasebox;
}

void Game::Game::updatePhysics(entt::registry& registry, const int windowHeight, const int windowWidth, const float delta)

{
	auto bodyView = registry.view<Component::FixedSpeedBody, Component::Position, Component::BoxCollider>();
	auto colliderView = registry.view<Component::BoxCollider>();
	for (auto bodyEntity : bodyView) {

		auto& vel = bodyView.get<Component::FixedSpeedBody>(bodyEntity);
		auto& pos = bodyView.get<Component::Position>(bodyEntity);
		auto& size = bodyView.get<Component::BoxCollider>(bodyEntity);


		sf::FloatRect rect(sf::Vector2f(pos.x, pos.y), sf::Vector2f(size.width, size.height));
		sf::FloatRect broadphasebox = GetSweptBroadphaseBox(rect, vel.xVelocity * delta, vel.yVelocity * delta);

		float shortestCollisionTime = 2.0f;
		bool collided = false;
		float bestnormalx, bestnormaly;
		entt::entity collidedEntity = entt::null;

		for (auto colliderEntity : colliderView) {
			if (registry.any<Component::FixedSpeedBody>(colliderEntity)) continue;
			auto& colliderPosition = registry.get<Component::Position>(colliderEntity);
			auto& collider = colliderView.get<Component::BoxCollider>(colliderEntity);
			sf::FloatRect batRect(sf::Vector2f(colliderPosition.x, colliderPosition.y), sf::Vector2f(collider.width, collider.height));
			// Has the ball hit the bat?
			if (broadphasebox.intersects(batRect))
			{
				// Hit detected so reverse the ball
				float normalx, normaly;
				float collisiontime = SweptAABB(vel.xVelocity * delta, vel.yVelocity * delta, rect, batRect, normalx, normaly);

				if (normalx == 0.0f && normaly == 0.0f) continue;

				if (collisiontime < shortestCollisionTime) {
					shortestCollisionTime = collisiontime;
					collided = true;
					bestnormalx = normalx;
					bestnormaly = normaly;
					collidedEntity = colliderEntity;
				}
			}
		}

		if (collided) {
			//move ball up to collision point
			pos.x += vel.xVelocity * shortestCollisionTime * delta;
			pos.y += vel.yVelocity * shortestCollisionTime * delta;
			float remainingtime = 1.0f - shortestCollisionTime;

			// implement collision response
			if (abs(bestnormalx) > 0.0001f) vel.xVelocity *= -1;
			if (abs(bestnormaly) > 0.0001f) vel.yVelocity *= -1;


			if (collidedEntity != entt::null && registry.any<Component::Bat>(collidedEntity)) {
				auto& batSize = registry.get<Component::BoxCollider>(collidedEntity);
				auto& batPosition = registry.get<Component::Position>(collidedEntity);
				sf::FloatRect batRect(sf::Vector2f(batPosition.x, batPosition.y), sf::Vector2f(batSize.width, batSize.height));

				// alter the angle of the ball trajectory depending on the position on the bat
				float speed = hypot(vel.xVelocity, vel.yVelocity);
				float angle = Utils::Remap(batRect.left, batRect.left + batRect.width, PI / 8.0f, PI * 7.0f / 8.0f, rect.left - rect.width / 2.0f);
				float newXVelocity = -cos(-angle);
				float newYVelocity = sin(-angle);
				bool negativeY = vel.yVelocity < 0;
				vel.yVelocity = negativeY ? newYVelocity * speed : -newYVelocity * speed;
				vel.xVelocity = newXVelocity * speed;

			}

			// apply remaining movement
			pos.x += vel.xVelocity * remainingtime * delta;
			pos.y += vel.yVelocity * remainingtime * delta;

			if (collidedEntity != entt::null && registry.any<Component::Breakable>(collidedEntity)) {
				auto& breakable = registry.get<Component::Breakable>(collidedEntity);
				breakable.breakState--;
			}
			return;
		}


		// no entity collisions, so move the ball!
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

void Game::Game::removeDestroyedBreakables(entt::registry& registry)

{
	registry.view<Component::Breakable>().each([&](auto entity, Component::Breakable& b) {
		if (b.breakState < 1) {
			// entt does allow destruction within loop (but only current entity)
			registry.destroy(entity);
		}
	});
}

void Game::Game::render(entt::registry& registry, sf::RenderWindow& window)

{
	registry.view<Component::BoxCollider, Component::Position>().each([&](auto entity, Component::BoxCollider& size, Component::Position& pos) {
		sf::RectangleShape shape;
		shape.setPosition(pos.x, pos.y);
		shape.setSize(sf::Vector2f(size.width, size.height));
		window.draw(shape);
		});
}
