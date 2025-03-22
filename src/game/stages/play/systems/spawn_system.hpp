#pragma once

#include "util/types.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

struct SpawnTileEvent;
struct SpawnBombEvent;
struct SpawnFlameEvent;
struct SpawnPowerUpEvent;
struct SpawnPortalEvent;
class EntityCreator;

class SpawnSystem
{
  public:
    SpawnSystem(entt::registry&, entt::dispatcher&, EntityCreator&);
    void update(const TimeDelta);

  private:
    void handleSpawnTileEvent(const SpawnTileEvent&);
    void handleSpawnBombEvent(const SpawnBombEvent&);
    void handleSpawnFlameEvent(const SpawnFlameEvent&);
    void handleSpawnPowerUpEvent(const SpawnPowerUpEvent&);
    void handleSpawnPortalEvent(const SpawnPortalEvent&);

    entt::registry& registry;
    EntityCreator& entityCreator;
};