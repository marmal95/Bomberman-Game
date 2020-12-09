#pragma once

#include <entityx/Entity.h>
#include <vector>
#include "Direction.hpp"

struct Collidable
{
	std::vector<entityx::Entity> skipCollisionEntities;
	Direction direction;
};