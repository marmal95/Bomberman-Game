#pragma once

#include "enums/tile_type.hpp"
#include <SFML/System/Vector2.hpp>

struct SpawnTileEvent
{
    sf::Vector2f position;
    TileType tileType;
};