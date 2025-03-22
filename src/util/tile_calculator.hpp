#pragma once

#include "util/constants.hpp"
#include "util/types.hpp"
#include <SFML/System/Vector2.hpp>
#include <cstddef>

inline auto calculatePositionForTileIndex(const TileIndex& index)
{
    return sf::Vector2f{static_cast<float>(index.x * TILE_SIZE.x), static_cast<float>(index.y * TILE_SIZE.y)};
}

inline auto calculateTileIndexForPosition(const sf::Vector2f& position)
{
    return TileIndex{static_cast<int>(position.x) / static_cast<int>(TILE_SIZE.x),
                     static_cast<int>(position.y) / static_cast<int>(TILE_SIZE.y)};
}

inline auto calculateTileIndexForCenterPosition(const sf::Vector2f& position, const sf::Vector2f& entitySize)
{
    return calculateTileIndexForPosition({
        position.x + entitySize.x / 2,
        position.y + entitySize.y / 2,
    });
}

inline auto calculatePositionInTileCenter(const TileIndex& tileIndex, const sf::Vector2i& entitySize)
{
    const auto tilePosition = calculatePositionForTileIndex(tileIndex);
    return sf::Vector2f{tilePosition.x + static_cast<float>((TILE_SIZE.x - entitySize.x) / 2),
                        tilePosition.y + static_cast<float>((TILE_SIZE.y - entitySize.y) / 2)};
}