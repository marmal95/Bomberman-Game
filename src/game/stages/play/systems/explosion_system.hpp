#pragma once

#include "enums/resource_id.hpp"
#include "util/resource_holder.hpp"
#include "util/types.hpp"
#include <SFML/Audio/Sound.hpp>
#include <SFML/System/Vector2.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <list>

struct Map;

class ExplosionSystem
{
  public:
    ExplosionSystem(entt::registry& registry,
                    entt::dispatcher& dispatcher,
                    const ResourceHolder<sf::SoundBuffer, ResourceID>&);
    void update(const TimeDelta);

  private:
    void removeFinishedSounds();

    bool spawnFlameInRow(const Map&, const sf::Vector2i, const int) const;
    bool spawnFlameInCol(const Map&, const sf::Vector2i, const int) const;
    void spawnFlame(const sf::Vector2f) const;

    entt::registry& registry;
    entt::dispatcher& dispatcher;

    const ResourceHolder<sf::SoundBuffer, ResourceID>& sounds;
    std::list<sf::Sound> ongoingSounds;
};