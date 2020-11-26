#pragma once

#include <entityx/entityx.h>
#include <SFML/Graphics/Texture.hpp>
#include <vector>
#include "SpawnPowerUpEvent.hpp"
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"

class PowerUpSystem : public entityx::System<PowerUpSystem>, public entityx::Receiver<PowerUpSystem>
{
public:
	PowerUpSystem(const ResourceHolder<sf::Texture, ResourceID>&);
	void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;
	void configure(entityx::EventManager&) override;
	void receive(const SpawnPowerUpEvent&);

private:
	void handleSpawnPowerUpEvents(entityx::EntityManager&);
	const ResourceHolder<sf::Texture, ResourceID>& textures;
	std::vector<SpawnPowerUpEvent> spawnPowerUpEvents;
};

