#pragma once

#include <SFML/System/Vector2.hpp>

enum class Direction
{
	Down,
	Left,
	Right,
	Up,
	None,
};

struct Movable
{
	sf::Vector2f velocity;
	Direction moveDirection;
};