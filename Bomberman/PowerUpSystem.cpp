#include "PowerUpSystem.hpp"
#include "Drawable.hpp"
#include "Collidable.hpp"
#include "Transformable.hpp"

PowerUpSystem::PowerUpSystem(const ResourceHolder<sf::Texture, ResourceID>& textures)
    : textures{ textures }, spawnPowerUpEvents{}
{}

void PowerUpSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
    handleSpawnPowerUpEvents(es);
}

void PowerUpSystem::configure(entityx::EventManager& events)
{
    events.subscribe<SpawnPowerUpEvent>(*this);
}

void PowerUpSystem::receive(const SpawnPowerUpEvent& event)
{
    spawnPowerUpEvents.push_back(event);
}

void PowerUpSystem::handleSpawnPowerUpEvents(entityx::EntityManager& es)
{
    for (const auto& event : spawnPowerUpEvents)
    {
        auto powerUpEntity = es.create();
        powerUpEntity.assign<Drawable>(textures.getResource(ResourceID::SpeedPowerUp));
        powerUpEntity.assign<Collidable>();
        powerUpEntity.assign<Transformable>(Transformable{ { 32, 32 }, { event.position.x + 16, event.position.y + 16 } });
    }
    spawnPowerUpEvents.clear();
}
