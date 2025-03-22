#pragma once

#include "game/stages/play/components/tile.hpp"
#include "util/constants.hpp"
#include "util/types.hpp"
#include <array>

struct Map
{
    auto& getTile(const TileIndex& index)
    {
        return tiles[static_cast<std::size_t>(index.x)][static_cast<std::size_t>(index.y)];
    }

    const auto& getTile(const TileIndex& index) const
    {
        return tiles[static_cast<std::size_t>(index.x)][static_cast<std::size_t>(index.y)];
    }

  private:
    std::array<std::array<Tile, WIDTH_TILES_NUM>, HEIGHT_TILES_NUM> tiles;
};