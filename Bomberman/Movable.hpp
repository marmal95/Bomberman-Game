#pragma once

#include <SFML/System/Vector2.hpp>
#include "Direction.hpp"

struct Movable
{
	sf::Vector2f velocity;
	Direction moveDirection;
};