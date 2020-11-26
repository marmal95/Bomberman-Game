#include "SpawnSystem.hpp"
#include "Drawable.hpp"
#include "Collidable.hpp"
#include "Animated.hpp"
#include "Bomb.hpp"
#include "Flame.hpp"
#include "Player.hpp"
#include "Transformable.hpp"
#include "SpawnSystem.hpp"

SpawnSystem::SpawnSystem(const ResourceHolder<sf::Texture, ResourceID>& textures)
    : textures{ textures }, generator{ std::random_device{}() }, spawnPowerUpEvents{}, spawnBombEvents{}, spawnFlameEvents{}, spawnTilesEvents{}
{}

void SpawnSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
    handleSpawnBombEvents(es);
    handleSpawnFlameEvents(es);
    handleSpawnPowerUpEvents(es);
    handleSpawnTileEvents(es);
}

void SpawnSystem::configure(entityx::EventManager& events)
{
    events.subscribe<SpawnPowerUpEvent>(*this);
    events.subscribe<SpawnBombEvent>(*this);
    events.subscribe<SpawnFlameEvent>(*this);
    events.subscribe<SpawnTileEvent>(*this);
}

void SpawnSystem::receive(const SpawnPowerUpEvent& event)
{
    spawnPowerUpEvents.push_back(event);
}

void SpawnSystem::receive(const SpawnBombEvent& event)
{
    spawnBombEvents.push_back(event);
}

void SpawnSystem::receive(const SpawnFlameEvent& event)
{
    spawnFlameEvents.push_back(event);
}

void SpawnSystem::receive(const SpawnTileEvent& event)
{
    spawnTilesEvents.push_back(event);
}

void SpawnSystem::handleSpawnPowerUpEvents(entityx::EntityManager& es)
{
    for (const auto& event : spawnPowerUpEvents)
        spawnPowerUp(es, event);
    spawnPowerUpEvents.clear();
}

void SpawnSystem::handleSpawnBombEvents(entityx::EntityManager& es)
{
    for (auto& event : spawnBombEvents)
    {
        auto& player = *event.player.component<Player>();
        if (player.bombsNum)
        {
            spawnBomb(es, event);
            --player.bombsNum;
        }
    }
    spawnBombEvents.clear();
}

void SpawnSystem::handleSpawnFlameEvents(entityx::EntityManager& es)
{
    for (const auto& event : spawnFlameEvents)
        spawnFlame(es, event);
    spawnFlameEvents.clear();
}

void SpawnSystem::handleSpawnTileEvents(entityx::EntityManager& es)
{
    for (const auto& event : spawnTilesEvents)
        spawnTile(es, event);
    spawnTilesEvents.clear();
}

void SpawnSystem::spawnPowerUp(entityx::EntityManager& es, const SpawnPowerUpEvent& event)
{
    std::uniform_int_distribution<> dist(0, 100);
    const int powerUpProbability = 25;

    if (dist(generator) <= 25)
    {
        dist = std::uniform_int_distribution<>(0, static_cast<int>(PowerUpType::Count));

        auto powerUpEntity = es.create();
        powerUpEntity.assign<Drawable>(getPowerUpTexture(PowerUpType{ dist(generator) }));
        powerUpEntity.assign<Collidable>();
        powerUpEntity.assign<Transformable>(Transformable{ { 32, 32 }, { event.position.x + 16, event.position.y + 16 } });
    }
}

void SpawnSystem::spawnBomb(entityx::EntityManager& es, const SpawnBombEvent& event) const
{
    auto playerPosition = event.player.component<const Transformable>()->position;
    playerPosition.x = static_cast<int>(playerPosition.x + 15) / 64 * 64 + 8;
    playerPosition.y = static_cast<int>(playerPosition.y + 15) / 64 * 64 + 8;

    auto bomb = es.create();
    bomb.assign<Drawable>(textures.getResource(ResourceID::Bomb));
    bomb.assign<Transformable>(Transformable{ { 48, 48 }, playerPosition });
    bomb.assign<Animated>(48, 48, 3, 0.1);
    bomb.assign<Bomb>(Bomb{ event.player, 3, 4 });
    bomb.assign<Collidable>(Collidable{ event.player });
}

void SpawnSystem::spawnFlame(entityx::EntityManager& es, const SpawnFlameEvent& event) const
{
    auto flame = es.create();
    flame.assign<Flame>(Flame{ 2 });
    flame.assign<Transformable>(Transformable{ { 48, 48 }, event.position });
    flame.assign<Drawable>(textures.getResource(ResourceID::Flame));
    flame.assign<Animated>(48, 48, 5, 0.1);
    flame.assign<Collidable>();
}

void SpawnSystem::spawnTile(entityx::EntityManager& es, const SpawnTileEvent& event) const
{
    auto tile = es.create();
    tile.assign<Transformable>(Transformable{ { 64.f, 64.f }, event.position });
    tile.assign<Tile>(Tile{ event.tileType });

    switch (event.tileType)
    {
    case TileType::None:
        tile.assign<Drawable>(textures.getResource(ResourceID::BackgroundTile));
        break;
    case TileType::SolidBlock:
        tile.assign<Collidable>();
        tile.assign<Drawable>(textures.getResource(ResourceID::SolidBlock));
        break;
    case TileType::ExplodableBlock:
        tile.assign<Collidable>();
        tile.assign<Drawable>(textures.getResource(ResourceID::ExplodableBlock));
        break;
    }
}

const sf::Texture& SpawnSystem::getPowerUpTexture(const PowerUpType type) const
{
    ResourceID resourceId;

    switch (type)
    {
    case PowerUpType::Bomb:
        return textures.getResource(ResourceID::BombPowerUp);
    case PowerUpType::Speed:
        return textures.getResource(ResourceID::SpeedPowerUp); 
    case PowerUpType::Flame:
        return textures.getResource(ResourceID::FlamePowerUp);
    }
}
