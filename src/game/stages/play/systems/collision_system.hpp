#pragma once

#include "enums/direction.hpp"
#include "util/types.hpp"
#include <SFML/System/Vector2.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <optional>

using CollisionRange = sf::Vector2f;

struct CollisionInfo
{
    CollisionRange collisionRange{};
    Direction direction{};
};

struct Collidable;

class CollisionSystem
{
  public:
    CollisionSystem(entt::registry&, entt::dispatcher& dispatcher);
    void update(const TimeDelta);

  private:
    void handlePlayerTilesCollisions(const entt::entity) const;
    void handlePlayerBombsCollisions(const entt::entity) const;
    void handlePlayerPowerUpsCollisions(const entt::entity) const;
    void handlePlayerFlamesCollisions(const entt::entity, const TimeDelta) const;
    void handlePlayerPortalsCollisions(const entt::entity) const;
    void handleTilesFlamesCollisions() const;

    void handleBlockingCollision(const entt::entity, const entt::entity) const;
    void handleFlameCollision(const entt::entity, const entt::entity) const;
    void handlePowerUpCollision(const entt::entity, const entt::entity) const;
    void handleTileFlameCollision(const entt::entity, const entt::entity) const;

    void handlePlayerPortalCollision(const entt::entity, const entt::entity) const;

    bool shouldSkipCollision(const entt::entity, const Collidable&) const;
    std::optional<CollisionInfo> checkCollision(const entt::entity, const entt::entity) const;

    entt::registry& registry;
    entt::dispatcher& dispatcher;
};