#pragma once

#include <entt/entity/entity.hpp>
#include <vector>

enum CollidableType
{
    NonBlocking,
    Blocking
};

struct Collidable
{
    CollidableType type;
    std::vector<entt::entity> skipCollisionEntities;
};
