#pragma once

#include <entityx/entityx.h>
#include <SFML/Graphics/Texture.hpp>
#include <vector>
#include <random>
#include "SpawnPowerUpEvent.hpp"
#include "SpawnBombEvent.hpp"
#include "SpawnFlameEvent.hpp"
#include "SpawnPortalEvent.hpp"
#include "SpawnTileEvent.hpp"
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"
#include "PowerUpType.hpp"

class SpawnSystem : public entityx::System<SpawnSystem>, public entityx::Receiver<SpawnSystem>
{
public:
	SpawnSystem(const ResourceHolder<sf::Texture, ResourceID>&);
	void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;
	void configure(entityx::EventManager&) override;

	void receive(const SpawnTileEvent&);
	void receive(const SpawnBombEvent&);
	void receive(const SpawnFlameEvent&);
	void receive(const SpawnPowerUpEvent&);
	void receive(const SpawnPortalEvent&);

private:
	void handleSpawnTileEvents(entityx::EntityManager&);
	void handleSpawnBombEvents(entityx::EntityManager&);
	void handleSpawnFlameEvents(entityx::EntityManager&);
	void handleSpawnPowerUpEvents(entityx::EntityManager&);
	void handleSpawnPortalEvents(entityx::EntityManager&);

	void spawnTile(entityx::EntityManager&, const SpawnTileEvent&) const;
	void spawnBomb(entityx::EntityManager&, SpawnBombEvent&) const;
	void spawnFlame(entityx::EntityManager&, const SpawnFlameEvent&) const;
	void spawnPowerUp(entityx::EntityManager&, const SpawnPowerUpEvent&);
	void spawnPortal(entityx::EntityManager&, const SpawnPortalEvent&) const;

	const sf::Texture& getPowerUpTexture(const PowerUpType) const;

	const ResourceHolder<sf::Texture, ResourceID>& textures;
	std::mt19937 generator;

	std::vector<SpawnTileEvent> spawnTilesEvents;
	std::vector<SpawnBombEvent> spawnBombEvents;
	std::vector<SpawnFlameEvent> spawnFlameEvents;
	std::vector<SpawnPowerUpEvent> spawnPowerUpEvents;
	std::vector<SpawnPortalEvent> spawnPortalEvents;
};

