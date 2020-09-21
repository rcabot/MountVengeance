#include "SpriteRenderer.h"

bool SpriteSheetSpriteRenderer::load(const std::string& tilesetPath)
{
	// load the tileset texture
	if (!tileset.loadFromFile(tilesetPath))
		return false;

	// resize the vertex array to fit
	vertices.setPrimitiveType(sf::Quads);
	vertices.resize(4);

	

	return true;
}

void SpriteSheetSpriteRenderer::grabSprite(sf::Vector2u tileSize, sf::Vector2u tileScale, int tileNumber)
{
	// find its position in the tileset texture
	int tu = tileNumber % (tileset.getSize().x / tileSize.x);
	int tv = tileNumber / (tileset.getSize().x / tileSize.x);

	// get a pointer to the current tile's quad
	sf::Vertex* quad = &vertices[0];

	// define its 4 corners
	quad[0].position = sf::Vector2f(0, 0);
	quad[1].position = sf::Vector2f(tileSize.x * tileScale.x, 0);
	quad[2].position = sf::Vector2f(tileSize.x * tileScale.x, tileSize.y * tileScale.y);
	quad[3].position = sf::Vector2f(0, tileSize.y * tileScale.y);

	// define its 4 texture coordinates
	quad[0].texCoords = sf::Vector2f(tu * tileSize.x, tv * tileSize.y);
	quad[1].texCoords = sf::Vector2f((tu + 1) * tileSize.x, tv * tileSize.y);
	quad[2].texCoords = sf::Vector2f((tu + 1) * tileSize.x, (tv + 1) * tileSize.y);
	quad[3].texCoords = sf::Vector2f(tu * tileSize.x, (tv + 1) * tileSize.y);
}

void SpriteSheetSpriteRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	// apply the transform
	states.transform *= getTransform();

	// apply the tileset texture
	states.texture = &tileset;

	// draw the vertex array
	target.draw(vertices, states);
}
