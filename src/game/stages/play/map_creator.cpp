#include "game/stages/play/map_creator.hpp"
#include "game/stages/play/components/player.hpp"
#include "game/stages/play/components/transformable.hpp"
#include "game/stages/play/events/spawn_portal_event.hpp"
#include "game/stages/play/events/spawn_tile_event.hpp"
#include "util/tile_calculator.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <random>

MapCreator::MapCreator(entt::registry& registry, entt::dispatcher& dispatcher)
    : registry{registry}, dispatcher{dispatcher}, blankTilesIndexes{}, map{}
{
}

void MapCreator::createMap()
{
    createSolidBlocks();
    createPortals();
    createExplodableBlocks();

    auto entity = registry.create();
    registry.emplace<Map>(entity, map);
}

void MapCreator::createSolidBlocks()
{
    for (int i = 0; i < HEIGHT_TILES_NUM; i++)
    {
        for (int j = 0; j < WIDTH_TILES_NUM; j++)
        {
            SpawnTileEvent event{};
            event.position = calculatePositionForTileIndex<int>({i, j});

            if (i == 0 || i == 20 || j == 0 || j == 20 || (i % 2 == 0 && j % 2 == 0))
            {
                event.tileType = TileType::SolidBlock;
                map.tiles[i][j] = {TileType::SolidBlock, false};
            }
            else
            {
                event.tileType = TileType::None;
                map.tiles[i][j] = {TileType::None, false};
                blankTilesIndexes.push_back({i, j});
            }

            dispatcher.trigger<SpawnTileEvent>(std::move(event));
        }
    }
}

void MapCreator::createPortals()
{
    sf::Vector2i firstPortalIndex{5, 5};
    sf::Vector2i secondPortalIndex{HEIGHT_TILES_NUM - 6, WIDTH_TILES_NUM - 6};

    blankTilesIndexes.erase(std::remove(std::begin(blankTilesIndexes), std::end(blankTilesIndexes), firstPortalIndex),
                            std::end(blankTilesIndexes));
    blankTilesIndexes.erase(std::remove(std::begin(blankTilesIndexes), std::end(blankTilesIndexes), secondPortalIndex),
                            std::end(blankTilesIndexes));

    dispatcher.trigger<SpawnPortalEvent>(SpawnPortalEvent{calculatePositionForTileIndex(firstPortalIndex)});
    dispatcher.trigger<SpawnPortalEvent>(SpawnPortalEvent{calculatePositionForTileIndex(secondPortalIndex)});
}

void MapCreator::createExplodableBlocks()
{
    const auto bombermanEntity = registry.view<Bomberman>().front();
    const auto bombermanPosition = registry.try_get<Transformable>(bombermanEntity)->position;

    const auto creepEntity = registry.view<Creep>().front();
    const auto creepPosition = registry.try_get<Transformable>(creepEntity)->position;

    auto rd = std::random_device{};
    auto rng = std::default_random_engine{rd()};
    std::shuffle(std::begin(blankTilesIndexes), std::end(blankTilesIndexes), rng);

    const auto bombermanPositionIndex = calculateTileIndexForPosition(bombermanPosition);
    const auto creepPositionIndex = calculateTileIndexForPosition(creepPosition);

    const auto splitPos = static_cast<uint32_t>(blankTilesIndexes.size() * (60 / 100.f));
    blankTilesIndexes.erase(std::begin(blankTilesIndexes) + splitPos, std::end(blankTilesIndexes));

    for (const auto blankTileIndex : blankTilesIndexes)
    {
        if ((std::abs(blankTileIndex.x - bombermanPositionIndex.x) >= DISTANCE_TO_EXPLODABLE_BLOCKS ||
             std::abs(blankTileIndex.y - bombermanPositionIndex.y) >= DISTANCE_TO_EXPLODABLE_BLOCKS) &&
            (std::abs(blankTileIndex.x - creepPositionIndex.x) >= DISTANCE_TO_EXPLODABLE_BLOCKS ||
             std::abs(blankTileIndex.y - creepPositionIndex.y) >= DISTANCE_TO_EXPLODABLE_BLOCKS))
        {
            dispatcher.trigger<SpawnTileEvent>(
                SpawnTileEvent{calculatePositionForTileIndex(blankTileIndex), TileType::ExplodableBlock});
            map.tiles[blankTileIndex.x][blankTileIndex.y] = {TileType::ExplodableBlock, false};
        }
    }
}