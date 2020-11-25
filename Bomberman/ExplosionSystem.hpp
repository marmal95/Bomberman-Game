#pragma once

#include <entityx/entityx.h>
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include "SpawnBombEvent.hpp"
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"

struct Map;

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
	void spawnFlame(entityx::EntityManager&, const sf::Vector2f) const;
	bool spawnFlameInRow(entityx::EntityManager&, const Map&, entityx::Entity, const sf::Vector2i, const int) const;
	bool spawnFlameInCol(entityx::EntityManager&, const Map&, entityx::Entity, const sf::Vector2i, const int) const;

	std::vector<SpawnBombEvent> spawnBombEvents;
	const ResourceHolder<sf::Texture, ResourceID>& textures;
};

