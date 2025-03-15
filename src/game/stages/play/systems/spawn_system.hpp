#pragma once

#include "enums/power_up_type.hpp"
#include "enums/resource_id.hpp"
#include "util/resource_holder.hpp"
#include "util/types.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <random>

struct SpawnTileEvent;
struct SpawnBombEvent;
struct SpawnFlameEvent;
struct SpawnPowerUpEvent;
struct SpawnPortalEvent;

class SpawnSystem
{
  public:
    explicit SpawnSystem(entt::registry&,
                         entt::dispatcher& dispatcher,
                         const ResourceHolder<sf::Texture, ResourceID>& textures);
    void update(const TimeDelta);

  private:
    void handleSpawnTileEvent(const SpawnTileEvent&);
    void handleSpawnBombEvent(const SpawnBombEvent&);
    void handleSpawnFlameEvent(const SpawnFlameEvent&);
    void handleSpawnPowerUpEvent(const SpawnPowerUpEvent&);
    void handleSpawnPortalEvent(const SpawnPortalEvent&);
    const sf::Texture& getPowerUpTexture(const PowerUpType) const;

    entt::registry& registry;
    const ResourceHolder<sf::Texture, ResourceID>& textures;
    std::mt19937 generator;
};