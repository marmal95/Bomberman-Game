#pragma once

#include <SFML/System/Vector2.hpp>

enum class Direction
{
	None,
	Down,
	Left,
	Right,
	Up
};

struct Movable
{
	sf::Vector2f velocity;
	Direction moveDirection;
};