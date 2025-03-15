#include "game/stages/play/systems/spawn_system.hpp"
#include "game/stages/play/components/animated.hpp"
#include "game/stages/play/components/bomb.hpp"
#include "game/stages/play/components/collidable.hpp"
#include "game/stages/play/components/drawable.hpp"
#include "game/stages/play/components/flame.hpp"
#include "game/stages/play/components/map.hpp"
#include "game/stages/play/components/player.hpp"
#include "game/stages/play/components/portal.hpp"
#include "game/stages/play/components/power_up.hpp"
#include "game/stages/play/components/tile.hpp"
#include "game/stages/play/components/transformable.hpp"
#include "game/stages/play/components/z_index.hpp"
#include "game/stages/play/events/spawn_bomb_event.hpp"
#include "game/stages/play/events/spawn_flame_event.hpp"
#include "game/stages/play/events/spawn_portal_event.hpp"
#include "game/stages/play/events/spawn_power_up_event.hpp"
#include "game/stages/play/events/spawn_tile_event.hpp"
#include "util/constants.hpp"
#include "util/converters.hpp"
#include "util/tile_calculator.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

SpawnSystem::SpawnSystem(entt::registry& registry,
                         entt::dispatcher& dispatcher,
                         const ResourceHolder<sf::Texture, ResourceID>& textures)
    : registry{registry}, textures{textures}
{
    dispatcher.sink<SpawnTileEvent>().connect<&SpawnSystem::handleSpawnTileEvent>(this);
    dispatcher.sink<SpawnBombEvent>().connect<&SpawnSystem::handleSpawnBombEvent>(this);
    dispatcher.sink<SpawnFlameEvent>().connect<&SpawnSystem::handleSpawnFlameEvent>(this);
    dispatcher.sink<SpawnPowerUpEvent>().connect<&SpawnSystem::handleSpawnPowerUpEvent>(this);
    dispatcher.sink<SpawnPortalEvent>().connect<&SpawnSystem::handleSpawnPortalEvent>(this);
}

void SpawnSystem::update(const TimeDelta)
{
}

void SpawnSystem::handleSpawnTileEvent(const SpawnTileEvent& event)
{
    auto tile = registry.create();
    registry.emplace<Transformable>(tile, Transformable{toVector2f(TILE_SIZE), event.position});
    registry.emplace<Tile>(tile, Tile{.tileType = event.tileType, .hasBomb = false});

    switch (event.tileType)
    {
    case TileType::None:
        registry.emplace<ZIndex>(tile, GROUND_Z_INDEX);
        registry.emplace<Drawable>(tile, textures.getResource(ResourceID::BackgroundTile));
        break;
    case TileType::SolidBlock:
        registry.emplace<ZIndex>(tile, BLOCKS_Z_INDEX);
        registry.emplace<Collidable>(tile);
        registry.emplace<Drawable>(tile, textures.getResource(ResourceID::SolidBlock));
        break;
    case TileType::ExplodableBlock:
        registry.emplace<ZIndex>(tile, BLOCKS_Z_INDEX);
        registry.emplace<Collidable>(tile);
        registry.emplace<Drawable>(tile, textures.getResource(ResourceID::ExplodableBlock));
        break;
    case TileType::FinishingGameAnimationBlock:
        registry.emplace<ZIndex>(tile, TOP_LEVEL_Z_INDEX);
        registry.emplace<Drawable>(tile, textures.getResource(ResourceID::SolidRedBlock));
        break;
    }
}

void SpawnSystem::handleSpawnBombEvent(const SpawnBombEvent& event)
{
    const auto map = registry.view<Map>().front();
    auto& mapComponent = registry.get<Map>(map);

    auto& player = registry.get<Player>(event.player);
    const auto& playerPosition = registry.get<Transformable>(event.player);
    const auto playerTileIndex =
        calculateTileIndexForCenterPosition(playerPosition.position, playerPosition.size);
    auto& mapTile = mapComponent.tiles[playerTileIndex.x][playerTileIndex.y];

    if (mapTile.hasBomb || !player.bombsNum)
        return;

    auto bomb = registry.create();
    registry.emplace<Drawable>(bomb, textures.getResource(ResourceID::Bomb));
    registry.emplace<Transformable>(
        bomb,
        Transformable{toVector2f(BOMB_SPRITE_SIZE),
                      calculatePositionInTileCenter(playerTileIndex, BOMB_SPRITE_SIZE)});
    registry.emplace<Animated>(bomb, BOMB_SPRITE_SIZE, 3, 0.1);
    registry.emplace<Bomb>(bomb, Bomb{event.player, 3, player.bombsRange});
    registry.emplace<ZIndex>(bomb, BOMB_Z_INDEX);

    Collidable collidable{};
    registry.view<Player>().each([&collidable](const entt::entity entity, Player&) {
        collidable.skipCollisionEntities.push_back(entity);
    });
    registry.emplace<Collidable>(bomb, std::move(collidable));

    --player.bombsNum;
    mapTile.hasBomb = true;
}

void SpawnSystem::handleSpawnFlameEvent(const SpawnFlameEvent& event)
{
    auto flame = registry.create();
    registry.emplace<Flame>(flame, Flame{2});
    registry.emplace<Transformable>(flame,
                                    Transformable{toVector2f(FLAME_SPRITE_SIZE), event.position});
    registry.emplace<Drawable>(flame, textures.getResource(ResourceID::Flame));
    registry.emplace<Animated>(flame, FLAME_SPRITE_SIZE, 5, 0.1);
    registry.emplace<Collidable>(flame);
    registry.emplace<ZIndex>(flame, FLAME_Z_INDEX);
}

void SpawnSystem::handleSpawnPowerUpEvent(const SpawnPowerUpEvent& event)
{
    const auto tileIndexToSpawn = calculateTileIndexForPosition(event.position);
    const auto positionInTile =
        calculatePositionInTileCenter(tileIndexToSpawn, POWER_UP_SPRITE_SIZE);

    std::uniform_int_distribution<> dist(0, 100);
    const auto powerUpProbability = 25;
    const auto randomProbability = dist(generator);

    if (randomProbability <= powerUpProbability)
    {
        dist = std::uniform_int_distribution<>(0, static_cast<int>(PowerUpType::Count) - 1);
        PowerUpType powerUpType{dist(generator)};

        auto powerUp = registry.create();
        registry.emplace<PowerUp>(powerUp, PowerUp{powerUpType});
        registry.emplace<Drawable>(powerUp, getPowerUpTexture(powerUpType));
        registry.emplace<Collidable>(powerUp);
        registry.emplace<ZIndex>(powerUp, ADDON_Z_INDEX);
        registry.emplace<Transformable>(
            powerUp, Transformable{toVector2f(POWER_UP_SPRITE_SIZE), positionInTile});
    }
}

void SpawnSystem::handleSpawnPortalEvent(const SpawnPortalEvent& event)
{
    auto portal = registry.create();
    registry.emplace<Transformable>(portal,
                                    Transformable{toVector2f(PORTAL_SPRITE_SIZES), event.position});
    registry.emplace<Drawable>(portal, textures.getResource(ResourceID::Portal));
    registry.emplace<Collidable>(portal);
    registry.emplace<Portal>(portal);
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