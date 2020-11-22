#include "ExplosionSystem.hpp"
#include "Transformable.hpp"
#include "Drawable.hpp"
#include "Animated.hpp"
#include "Explodable.hpp"

ExplosionSystem::ExplosionSystem(const ResourceHolder<sf::Texture, ResourceID>& textures)
    : textures{ textures }
{}

void ExplosionSystem::update(entityx::EntityManager& es, entityx::EventManager&, entityx::TimeDelta dt)
{
    handleSpawnBombEvents(es);

    es.each<Explodable>([&](entityx::Entity entity, Explodable& explodable) {
        explodable.timeToExplode -= dt;

        if (explodable.timeToExplode <= 0)
        {
            // TODO: Handle
            std::cout << "BOOM";
            entity.destroy();
        }
    });
}

void ExplosionSystem::configure(entityx::EventManager& events)
{
    events.subscribe<SpawnBombEvent>(*this);
}

void ExplosionSystem::receive(const SpawnBombEvent& event)
{
    spawnBombEvents.push_back(event);
}

void ExplosionSystem::handleSpawnBombEvents(entityx::EntityManager& es)
{
    for (auto& event : spawnBombEvents)
    {
        auto bomb = es.create();
        auto playerPosition = event.player.component<Transformable>()->position;
        playerPosition.x = static_cast<int>(playerPosition.x) / 64 * 64;
        playerPosition.y = static_cast<int>(playerPosition.y) / 64 * 64;
        bomb.assign<Transformable>(Transformable{ { 48, 48 }, playerPosition });
        bomb.assign<Drawable>(textures.getResource(ResourceID::Bomb));
        bomb.assign<Animated>(48, 48, 3, 0.1);
        bomb.assign<Explodable>(Explodable{ 3 });
    }
    spawnBombEvents.clear();
}
