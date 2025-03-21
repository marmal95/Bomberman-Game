#pragma once

#include "util/types.hpp"
#include <SFML/System/Vector2.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

struct Collidable;

class CollisionSystem
{
  public:
    CollisionSystem(entt::registry&, entt::dispatcher& dispatcher);
    void update(const TimeDelta);

  private:
    void handlePlayerPowerUpsCollisions(const entt::entity) const;
    void handlePlayerFlamesCollisions(const entt::entity, const TimeDelta) const;
    void handlePlayerPortalsCollisions(const entt::entity) const;
    void handleTilesFlamesCollisions() const;

    void handleFlameCollision(const entt::entity, const entt::entity) const;
    void handlePowerUpCollision(const entt::entity, const entt::entity) const;
    void handleTileFlameCollision(const entt::entity, const entt::entity) const;

    void handlePlayerPortalCollision(const entt::entity, const entt::entity) const;

    bool shouldSkipCollision(const entt::entity, const Collidable&) const;

    entt::registry& registry;
    entt::dispatcher& dispatcher;
};