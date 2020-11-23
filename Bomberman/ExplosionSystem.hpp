#pragma once

#include <entityx/entityx.h>
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include "SpawnBombEvent.hpp"
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"


class ExplosionSystem : public entityx::System<ExplosionSystem>, public entityx::Receiver<ExplosionSystem>
{
public:
	ExplosionSystem(const ResourceHolder<sf::Texture, ResourceID>&);
	void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;
	void configure(entityx::EventManager&) override;
	void receive(const SpawnBombEvent&);

private:
	void handleSpawnBombEvents(entityx::EntityManager&);
	void spawnBomb(entityx::EntityManager&, SpawnBombEvent&);

	std::vector<SpawnBombEvent> spawnBombEvents;
	const ResourceHolder<sf::Texture, ResourceID>& textures;
};

