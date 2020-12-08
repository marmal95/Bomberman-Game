#include "SpawnSystem.hpp"
#include "Drawable.hpp"
#include "Collidable.hpp"
#include "Animated.hpp"
#include "Bomb.hpp"
#include "Flame.hpp"
#include "Player.hpp"
#include "Transformable.hpp"
#include "SpawnSystem.hpp"
#include "PowerUp.hpp"
#include "TopLevel.hpp"
#include "Utils.hpp"
#include "Map.hpp"

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
    const auto tileIndexToSpawn = calculateTileIndexForPosition(event.position);
    const auto positionInTile = calculatePositionInTileCenter(tileIndexToSpawn, POWER_UP_SPRITE_SIZE);

    std::uniform_int_distribution<> dist(0, 100);
    const auto powerUpProbability = 25;
    const auto randomProbability = dist(generator);

    if (randomProbability <= powerUpProbability)
    {
        dist = std::uniform_int_distribution<>(0, static_cast<int>(PowerUpType::Count) - 1);
        PowerUpType powerUpType{ dist(generator) };

        auto powerUpEntity = es.create();
        powerUpEntity.assign<PowerUp>(PowerUp{ powerUpType });
        powerUpEntity.assign<Drawable>(getPowerUpTexture(powerUpType));
        powerUpEntity.assign<Collidable>();
        powerUpEntity.assign<Transformable>(Transformable{ toVector2f(POWER_UP_SPRITE_SIZE), positionInTile });
    }
}

void SpawnSystem::spawnBomb(entityx::EntityManager& es, SpawnBombEvent& event) const
{
    auto& map = *(*es.entities_with_components<Map>().begin()).component<Map>();
    auto& player = *event.player.component<Player>();

    const auto& playerPosition = *event.player.component<const Transformable>();
    const auto playerTileIndex = calculateTileIndexForCenterPosition(playerPosition.position, playerPosition.size);
    auto& mapTile = map.tiles[playerTileIndex.x][playerTileIndex.y];

    if (mapTile.hasBomb || !player.bombsNum)
        return;

    auto bomb = es.create();
    bomb.assign<Drawable>(textures.getResource(ResourceID::Bomb));
    bomb.assign<Transformable>(Transformable{ toVector2f(BOMB_SPRITE_SIZE), calculatePositionInTileCenter(playerTileIndex, BOMB_SPRITE_SIZE) });
    bomb.assign<Animated>(BOMB_SPRITE_SIZE, 3, 0.1);
    bomb.assign<Bomb>(Bomb{ event.player, 3, player.bombsRange });
    bomb.assign<Collidable>(Collidable{ event.player });

    --player.bombsNum;
    mapTile.hasBomb = true;
}

void SpawnSystem::spawnFlame(entityx::EntityManager& es, const SpawnFlameEvent& event) const
{
    auto flame = es.create();
    flame.assign<Flame>(Flame{ 2 });
    flame.assign<Transformable>(Transformable{ toVector2f(FLAME_SPRITE_SIZE), event.position });
    flame.assign<Drawable>(textures.getResource(ResourceID::Flame));
    flame.assign<Animated>(FLAME_SPRITE_SIZE, 5, 0.1);
    flame.assign<Collidable>();
}

void SpawnSystem::spawnTile(entityx::EntityManager& es, const SpawnTileEvent& event) const
{
    auto tile = es.create();
    tile.assign<Transformable>(Transformable{ toVector2f(TILE_SIZE), event.position });
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
    case TileType::FinishingGameAnimationBlock:
        tile.assign<TopLevel>();
        tile.assign<Drawable>(textures.getResource(ResourceID::SolidRedBlock));
        break;
    }
}

const sf::Texture& SpawnSystem::getPowerUpTexture(const PowerUpType type) const
{
    switch (type)
    {
    case PowerUpType::Bomb:
        return textures.getResource(ResourceID::BombPowerUp);
    case PowerUpType::Speed:
        return textures.getResource(ResourceID::SpeedPowerUp); 
    case PowerUpType::Flame:
        return textures.getResource(ResourceID::FlamePowerUp);
    default:
        abort(); // should never happen
    }
}
