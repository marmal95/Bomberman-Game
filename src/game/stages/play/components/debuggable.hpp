#pragma once

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

struct Debuggable
{
    sf::Vector2f size;
    sf::Vector2f position;
    sf::Color color{sf::Color{255, 0, 0, 128}};
};