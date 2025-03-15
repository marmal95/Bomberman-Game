#pragma once

#include "enums/direction.hpp"
#include <SFML/System/Vector2.hpp>

struct Movable
{
    sf::Vector2f velocity;
    Direction moveDirection;
};