#pragma once

#include <entityx/entityx.h>
#include "Movable.hpp"

struct MoveChangeEvent
{
    entityx::Entity entity;
    Direction direction;
};