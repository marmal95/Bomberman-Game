#pragma once

#include <SFML/System/Vector2.hpp>
#include "Tile.hpp"

struct SpawnTileEvent
{
    sf::Vector2f position;
    TileType tileType;
};