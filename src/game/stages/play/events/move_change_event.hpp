#pragma once

#include "enums/direction.hpp"
#include <entt/entity/entity.hpp>

struct MoveChangeEvent
{
    entt::entity entity;
    Direction direction;
};