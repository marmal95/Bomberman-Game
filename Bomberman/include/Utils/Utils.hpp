#pragma once

#include <SFML/System/Vector2.hpp>
#include "Constants.hpp"

template<typename T>
inline auto toVector2f(const sf::Vector2<T>& vector)
{
    return sf::Vector2f{ static_cast<float>(vector.x), static_cast<float>(vector.y) };
}

template<typename T>
inline auto toVector2i(const sf::Vector2<T>& vector)
{
    return sf::Vector2i{ static_cast<int>(vector.x), static_cast<int>(vector.y) };
}

template<typename T>
inline auto calculatePositionForTileIndex(const sf::Vector2<T>& index)
{
    return sf::Vector2f{ static_cast<float>(index.y * TILE_SIZE.x),
                         static_cast<float>(index.x * TILE_SIZE.y) };
}

inline auto calculateTileIndexForPosition(const sf::Vector2f& position)
{
    return sf::Vector2i{ static_cast<int>(position.y) / static_cast<int>(TILE_SIZE.y),
                         static_cast<int>(position.x) / static_cast<int>(TILE_SIZE.x) };
}

inline auto calculateTileIndexForCenterPosition(const sf::Vector2f& position, const sf::Vector2f& entitySize)
{
    return calculateTileIndexForPosition({ position.x + entitySize.x / 2, position.y + entitySize.y / 2, });
}

inline auto calculatePositionInTileCenter(const sf::Vector2i& tileIndex, const sf::Vector2i& entitySize)
{
    const auto tilePosition = calculatePositionForTileIndex(tileIndex);
    return sf::Vector2f{ tilePosition.x + (TILE_SIZE.x - entitySize.x) / 2,
                         tilePosition.y + (TILE_SIZE.y - entitySize.y) / 2 };
}