#include "MapCreator.hpp"
#include "Map.hpp"
#include "SpawnTileEvent.hpp"
#include "SpawnPortalEvent.hpp"
#include "Utils.hpp"
#include "Transformable.hpp"
#include "Player.hpp"
#include <random>

MapCreator::MapCreator(entityx::EntityManager& entityManager, entityx::EventManager& eventManager)
    : entityManager{ entityManager }, eventManager{ eventManager }, blankTilesIndexes{}, map{}
{}

void MapCreator::createMap()
{
    createSolidBlocks();
    createPortals();
    createExplodableBlocks();

    auto mapEntity = entityManager.create();
    mapEntity.assign<Map>(map);
}

void MapCreator::createSolidBlocks()
{
    for (int i = 0; i < HEIGHT_TILES_NUM; i++)
    {
        for (int j = 0; j < WIDTH_TILES_NUM; j++)
        {
            SpawnTileEvent event{};
            event.position = calculatePositionForTileIndex<int>({ i, j });

            if (i == 0 || i == 20 || j == 0 || j == 20 || (i % 2 == 0 && j % 2 == 0))
            {
                event.tileType = TileType::SolidBlock;
                map.tiles[i][j] = { TileType::SolidBlock };
            }
            else
            {
                event.tileType = TileType::None;
                map.tiles[i][j] = { TileType::None };
                blankTilesIndexes.push_back({ i, j });
            }

            eventManager.emit<SpawnTileEvent>(std::move(event));
        }
    }
}

void MapCreator::createPortals()
{
    sf::Vector2i firstPortalIndex{ 5, 5 };
    sf::Vector2i secondPortalIndex{ HEIGHT_TILES_NUM - 6, WIDTH_TILES_NUM - 6 };

    blankTilesIndexes.erase(std::remove(std::begin(blankTilesIndexes), std::end(blankTilesIndexes), firstPortalIndex), std::end(blankTilesIndexes));
    blankTilesIndexes.erase(std::remove(std::begin(blankTilesIndexes), std::end(blankTilesIndexes), secondPortalIndex), std::end(blankTilesIndexes));

    eventManager.emit<SpawnPortalEvent>(SpawnPortalEvent{ calculatePositionForTileIndex(firstPortalIndex) });
    eventManager.emit<SpawnPortalEvent>(SpawnPortalEvent{ calculatePositionForTileIndex(secondPortalIndex) });
}


void MapCreator::createExplodableBlocks()
{
    const auto& bombermanPosition = (*entityManager.entities_with_components<Bomberman>().begin()).component<Transformable>()->position;
    const auto& creepPosition = (*entityManager.entities_with_components<Creep>().begin()).component<Transformable>()->position;

    auto rd = std::random_device{};
    auto rng = std::default_random_engine{ rd() };
    std::shuffle(std::begin(blankTilesIndexes), std::end(blankTilesIndexes), rng);

    const auto bombermanPositionIndex = calculateTileIndexForPosition(bombermanPosition);
    const auto creepPositionIndex = calculateTileIndexForPosition(creepPosition);

    const auto splitPos = static_cast<uint32_t>(blankTilesIndexes.size() * (70 / 100.f));
    blankTilesIndexes.erase(std::begin(blankTilesIndexes) + splitPos), std::end(blankTilesIndexes);

    for (const auto& blankTileIndex : blankTilesIndexes)
    {
        if ((std::abs(blankTileIndex.x - bombermanPositionIndex.x) >= DISTANCE_TO_EXPLODABLE_BLOCKS ||
            std::abs(blankTileIndex.y - bombermanPositionIndex.y) >= DISTANCE_TO_EXPLODABLE_BLOCKS) &&
            (std::abs(blankTileIndex.x - creepPositionIndex.x) >= DISTANCE_TO_EXPLODABLE_BLOCKS ||
                std::abs(blankTileIndex.y - creepPositionIndex.y) >= DISTANCE_TO_EXPLODABLE_BLOCKS))
        {
            eventManager.emit<SpawnTileEvent>(SpawnTileEvent{ calculatePositionForTileIndex(blankTileIndex), TileType::ExplodableBlock });
            map.tiles[blankTileIndex.x][blankTileIndex.y] = { TileType::ExplodableBlock };
        }
    }
}