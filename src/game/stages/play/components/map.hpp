#pragma once

#include "game/stages/play/components/tile.hpp"
#include "util/constants.hpp"
#include <array>

struct Map
{
    std::array<std::array<Tile, WIDTH_TILES_NUM>, HEIGHT_TILES_NUM> tiles;
};