#pragma once

#include <entityx/entityx.h>
#include <SFML/System/Vector2.hpp>
#include <SFML/Audio/Sound.hpp>
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"
#include <list>

struct Map;

class ExplosionSystem : public entityx::System<ExplosionSystem>
{
public:
    ExplosionSystem(const ResourceHolder<sf::SoundBuffer, ResourceID>&);
    void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;

private:
    void removeFinishedSounds();

    bool spawnFlameInRow(entityx::EntityManager&, entityx::EventManager&, const Map&, entityx::Entity, const sf::Vector2i, const int) const;
    bool spawnFlameInCol(entityx::EntityManager&, entityx::EventManager&, const Map&, entityx::Entity, const sf::Vector2i, const int) const;
    void spawnFlame(entityx::EntityManager&, entityx::EventManager&, const sf::Vector2f) const;

    const ResourceHolder<sf::SoundBuffer, ResourceID>& sounds;
    std::list<sf::Sound> ongoingSounds;
};

