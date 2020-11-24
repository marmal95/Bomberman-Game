#pragma once

#include "MoveChangeEvent.hpp"
#include "Collidable.hpp"
#include <entityx/entityx.h>
#include <vector>
#include <optional>

using CollisionRange = sf::Vector2f;

struct CollisionInfo
{
	CollisionRange collisionRange{};
	Direction direction{};
};

class CollisionSystem : public entityx::System<CollisionSystem>, public entityx::Receiver<CollisionSystem>
{
public:
	void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;
	void configure(entityx::EventManager&) override;
	void receive(const MoveChangeEvent&);

private:
	void handleMoveChangeEvents();
	void handleBlockingCollision(entityx::Entity, entityx::Entity) const;
	void handleFlameCollision(entityx::Entity, entityx::Entity) const;
	bool shouldSkipCollision(const entityx::Entity&, const Collidable&) const;
	std::optional<CollisionInfo> checkCollision(entityx::Entity, entityx::Entity) const;

	std::vector<MoveChangeEvent> moveChangeEvents;
};

