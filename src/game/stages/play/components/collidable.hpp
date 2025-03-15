#pragma once

#include "enums/direction.hpp"
#include <entt/entity/entity.hpp>
#include <vector>

struct Collidable
{
    std::vector<entt::entity> skipCollisionEntities;
    Direction direction;
};