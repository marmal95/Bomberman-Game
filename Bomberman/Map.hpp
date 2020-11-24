#pragma once

#include "Tile.hpp"
#include "Constants.hpp"
#include <array>

struct Map
{
    std::array<std::array<Tile, WIDTH_TILES_NUM>, HEIGHT_TILES_NUM> tiles;
};