#include "game/stages/play/systems/spawn_system.hpp"
#include "game/stages/play/components/map.hpp"
#include "game/stages/play/components/player.hpp"
#include "game/stages/play/components/tile.hpp"
#include "game/stages/play/components/transformable.hpp"
#include "game/stages/play/entity_creator.hpp"
#include "game/stages/play/events/spawn_bomb_event.hpp"
#include "game/stages/play/events/spawn_flame_event.hpp"
#include "game/stages/play/events/spawn_portal_event.hpp"
#include "game/stages/play/events/spawn_power_up_event.hpp"
#include "game/stages/play/events/spawn_tile_event.hpp"
#include "util/tile_calculator.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

SpawnSystem::SpawnSystem(entt::registry& registry,
                         entt::dispatcher& dispatcher,
                         const ResourceHolder<sf::Texture, ResourceID>& textures)
    : registry{registry}, textures{textures}, entityCreator{registry, textures}
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
    switch (event.tileType)
    {
    case TileType::None:
        entityCreator.createEmptyTile(event);
        break;
    case TileType::SolidBlock:
        entityCreator.createSolidBlock(event);
        break;
    case TileType::ExplodableBlock:
        entityCreator.createExplodableBlock(event);
        break;
    case TileType::FinishingGameAnimationBlock:
        entityCreator.createFinishAnimationBlock(event);
        break;
    }
}

void SpawnSystem::handleSpawnBombEvent(const SpawnBombEvent& event)
{
    const auto map = registry.view<Map>().front();
    auto& mapComponent = registry.get<Map>(map);

    auto& player = registry.get<Player>(event.player);
    const auto& playerPosition = registry.get<Transformable>(event.player);
    const auto playerTileIndex = calculateTileIndexForCenterPosition(playerPosition.position, playerPosition.size);
    auto& mapTile = mapComponent.tiles[playerTileIndex.x][playerTileIndex.y];

    if (mapTile.hasBomb || !player.bombsNum)
        return;

    entityCreator.createBomb(event);

    --player.bombsNum;
    mapTile.hasBomb = true;
}

void SpawnSystem::handleSpawnFlameEvent(const SpawnFlameEvent& event)
{
    entityCreator.createFlame(event);
}

void SpawnSystem::handleSpawnPowerUpEvent(const SpawnPowerUpEvent& event)
{
    entityCreator.createPowerUp(event);
}

void SpawnSystem::handleSpawnPortalEvent(const SpawnPortalEvent& event)
{
    entityCreator.createPortal(event);
}