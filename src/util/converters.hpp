#pragma once

#include <SFML/System/Vector2.hpp>

template <typename T> inline auto toVector2f(const sf::Vector2<T>& vector)
{
    return sf::Vector2f{static_cast<float>(vector.x), static_cast<float>(vector.y)};
}

template <typename T> inline auto toVector2i(const sf::Vector2<T>& vector)
{
    return sf::Vector2i{static_cast<int>(vector.x), static_cast<int>(vector.y)};
}