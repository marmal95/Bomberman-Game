#pragma once

#include "util/types.hpp"
#include <entt/entity/entity.hpp>

struct Bomb
{
    entt::entity spawner;
    TimeDelta timeToExplode;
    int range;
};