#pragma once

#include <entityx/Entity.h>
#include <optional>
#include "Direction.hpp"

struct Collidable
{
	Collidable(std::optional<entityx::Entity> spawner = std::nullopt)
		: spawner{ spawner }, direction{ Direction::None }
	{}

	std::optional<entityx::Entity> spawner;
	Direction direction;
};