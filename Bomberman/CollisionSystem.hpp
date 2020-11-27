#pragma once

#include "Collidable.hpp"
#include <SFML/System/Vector2.hpp>
#include <entityx/entityx.h>
#include <vector>
#include <optional>

using CollisionRange = sf::Vector2f;

struct CollisionInfo
{
	CollisionRange collisionRange{};
	Direction direction{};
};

class CollisionSystem : public entityx::System<CollisionSystem>
{
public:
	void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;

private:
	void handlePlayerTilesCollisions(entityx::Entity, entityx::EntityManager&) const;
	void handlePlayerBombsCollisions(entityx::Entity, entityx::EntityManager&) const;
	void handlePlayerPowerUpsCollisions(entityx::Entity, entityx::EntityManager&) const;
	void handlePlayerFlamesCollisions(entityx::Entity, entityx::EntityManager&, const entityx::TimeDelta) const;
	void handleTilesFlamesCollisions(entityx::EntityManager&, entityx::EventManager&) const;

	void handleBlockingCollision(entityx::Entity, entityx::Entity) const;
	void handleFlameCollision(entityx::Entity, entityx::Entity, const entityx::TimeDelta) const;
	void handlePowerUpCollision(entityx::Entity, entityx::Entity) const;
	void handleTileFlameCollision(entityx::Entity, entityx::Entity, entityx::EntityManager&, entityx::EventManager&) const;

	bool shouldSkipCollision(const entityx::Entity&, const Collidable&) const;
	std::optional<CollisionInfo> checkCollision(entityx::Entity, entityx::Entity) const;
};

