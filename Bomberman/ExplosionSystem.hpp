#pragma once

#include <entityx/entityx.h>
#include <SFML/System/Vector2.hpp>
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"

struct Map;

class ExplosionSystem : public entityx::System<ExplosionSystem>
{
public:
    void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;

private:
    bool spawnFlameInRow(entityx::EntityManager&, entityx::EventManager&, const Map&, entityx::Entity, const sf::Vector2i, const int) const;
    bool spawnFlameInCol(entityx::EntityManager&, entityx::EventManager&, const Map&, entityx::Entity, const sf::Vector2i, const int) const;
    void spawnFlame(entityx::EntityManager&, entityx::EventManager&, const sf::Vector2f) const;
};

