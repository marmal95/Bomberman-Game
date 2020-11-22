#pragma once

#include "MoveChangeEvent.hpp"
#include <entityx/entityx.h>
#include <vector>

class CollisionSystem : public entityx::System<CollisionSystem>, public entityx::Receiver<CollisionSystem>
{
public:
	void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;
	void configure(entityx::EventManager&) override;
	void receive(const MoveChangeEvent&);

private:
	void handleMoveChangeEvents();
	void checkCollision(entityx::EventManager& events, entityx::Entity, entityx::Entity) const;

	std::vector<MoveChangeEvent> moveChangeEvents;
};

