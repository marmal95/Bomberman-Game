#pragma once

#include "enums/direction.hpp"
#include <SFML/System/Vector2.hpp>
#include <optional>

struct Transformable;

using CollisionRange = sf::Vector2f;

struct Collision
{
    CollisionRange collisionRange{};
    Direction direction{};
};

std::optional<Collision> detectCollision(const Transformable& firstTransformable,
                                         const Transformable& secondTransformable);
