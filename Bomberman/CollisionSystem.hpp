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
	void handleBlockingCollision(entityx::Entity, entityx::Entity) const;
	void handleFlameCollision(entityx::Entity, entityx::Entity) const;
	void handlePowerUpCollision(entityx::Entity, entityx::Entity) const;

	bool shouldSkipCollision(const entityx::Entity&, const Collidable&) const;
	std::optional<CollisionInfo> checkCollision(entityx::Entity, entityx::Entity) const;
};

