#include "EcsGameplay.hpp"
#include "MoveSystem.hpp"
#include "DrawSystem.hpp"
#include "AnimateSystem.hpp"
#include "CollisionSystem.hpp"
#include "ExplosionSystem.hpp"
#include "SpawnSystem.hpp"
#include "GameplayStage.hpp"
#include "Transformable.hpp"
#include "Drawable.hpp"
#include "Movable.hpp"
#include "Animated.hpp"
#include "Player.hpp"
#include "Collidable.hpp"
#include "Constants.hpp"
#include "SpawnBombEvent.hpp"
#include "SpawnTileEvent.hpp"
#include "Map.hpp"
#include "GameResult.hpp"
#include "ExitStage.hpp"
#include <algorithm>
#include <random>
#include <memory>

EcsGameplay::EcsGameplay(const GameplayStage & stage)
    : gameplayStage{ stage }
{
    systems.add<SpawnSystem>(gameplayStage.getTextures());
    systems.add<MoveSystem>();
    systems.add<CollisionSystem>();
    systems.add<ExplosionSystem>();
    systems.add<AnimateSystem>();
    systems.add<DrawSystem>(gameplayStage.getTextures());
    systems.configure();

    createBomberman();
    createCreep();
    createMap();
}    

bool EcsGameplay::update(const entityx::TimeDelta dt)
{
    systems.update<SpawnSystem>(dt);
    systems.update<MoveSystem>(dt);
    systems.update<CollisionSystem>(dt);
    systems.update<ExplosionSystem>(dt);
    systems.update<AnimateSystem>(dt);
    checkIsGameOver();
    return true;
}

void EcsGameplay::draw(sf::RenderWindow&)
{
    systems.update<DrawSystem>({});
}

void EcsGameplay::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::EventType::KeyReleased)
    {
        if (event.key.code == sf::Keyboard::RControl)
        {
            auto player = entities.entities_with_components<Bomberman>().begin();
            events.emit<SpawnBombEvent>({ *player });
        }

        if (event.key.code == sf::Keyboard::LControl)
        {
            auto player = entities.entities_with_components<Creep>().begin();
            events.emit<SpawnBombEvent>({ *player });
        }
    }
}

void EcsGameplay::createMap()
{
    Map map{};
    std::vector<sf::Vector2i> blankTilesIndexes;

    for (int i = 0; i < HEIGHT_TILES_NUM; i++)
    {
        for (int j = 0; j < WIDTH_TILES_NUM; j++)
        {
            SpawnTileEvent event{};
            event.position = { j * 64.f, i * 64.f };

            if (i == 0 || i == 20 || j == 0 || j == 20 || i % 2 == 0 && j % 2 == 0)
            {
                event.tileType = TileType::SolidBlock;
                map.tiles[i][j] = { TileType::SolidBlock };
            }
            else
            {
                event.tileType = TileType::None;
                map.tiles[i][j] = { TileType::None };
                blankTilesIndexes.emplace_back(sf::Vector2i{ i, j });
            }

            events.emit<SpawnTileEvent>(std::move(event));
        }
    }

    createExplodableBlocks(map, blankTilesIndexes);

    auto mapEntity = entities.create();
    mapEntity.assign<Map>(map);
}

void EcsGameplay::createExplodableBlocks(Map& map, std::vector<sf::Vector2i>& blankTilesIndexes)
{
    const auto& bombermanPosition = (*entities.entities_with_components<Bomberman>().begin()).component<Transformable>()->position;
    const auto& creepPosition = (*entities.entities_with_components<Creep>().begin()).component<Transformable>()->position;

    auto rd = std::random_device{};
    auto rng = std::default_random_engine{ rd() };
    std::shuffle(std::begin(blankTilesIndexes), std::end(blankTilesIndexes), rng);

    auto bombermanPositionIndex = sf::Vector2i{ static_cast<int>(bombermanPosition.y / 64), static_cast<int>(bombermanPosition.x / 64) };
    auto creepPositionIndex = sf::Vector2i{ static_cast<int>(creepPosition.y) / 64, static_cast<int>(creepPosition.x) / 64 };
    auto splitPos = static_cast<uint32_t>(blankTilesIndexes.size() * (70 / 100.f));
    blankTilesIndexes.erase(std::begin(blankTilesIndexes) + splitPos, std::end(blankTilesIndexes));

    for (const auto& blankTileIndex : blankTilesIndexes)
    {
        if ((std::abs(blankTileIndex.x - bombermanPositionIndex.x) >= 2 ||
            std::abs(blankTileIndex.y - bombermanPositionIndex.y) >= 2) &&
            (std::abs(blankTileIndex.x - creepPositionIndex.x) >= 2 ||
            std::abs(blankTileIndex.y - creepPositionIndex.y) >= 2))
        {
            events.emit<SpawnTileEvent>(SpawnTileEvent{ { blankTileIndex.y * 64.f, blankTileIndex.x * 64.f }, TileType::ExplodableBlock });
            map.tiles[blankTileIndex.x][blankTileIndex.y] = { TileType::ExplodableBlock };
        }
    }
}

void EcsGameplay::createBomberman()
{
    auto entity = entities.create();
    entity.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::BombermanFront));
    entity.assign<Transformable>(Transformable{ { 30, 30 }, { 81, 81 } });
    entity.assign<Movable>(Movable{ { 64, 64 }, Direction::None });
    entity.assign<Animated>(Animated{ 64, 128, 8, 1 });
    entity.assign<Collidable>();
    entity.assign<Player>();
    entity.assign<Bomberman>();
    events.emit<MoveChangeEvent>({ entity, Direction::None });
}

void EcsGameplay::createCreep()
{
    auto entity = entities.create();
    entity.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::CreepFront));
    entity.assign<Transformable>(Transformable{ { 30, 30 }, { 1216 + 17, 1280 - 64 + 17 } });
    entity.assign<Movable>(Movable{ { 64, 64 }, Direction::None });
    entity.assign<Animated>(Animated{ 64, 64, 6, 1 });
    entity.assign<Collidable>();
    entity.assign<Player>();
    entity.assign<Creep>();
    events.emit<MoveChangeEvent>({ entity, Direction::None });
}

void EcsGameplay::checkIsGameOver()
{
    GameResult gameResult{};
    const auto bomberman = (*entities.entities_with_components<Player, Bomberman>().begin()).component<Player>();
    const auto creep = (*entities.entities_with_components<Player, Creep>().begin()).component<Player>();

    if (!bomberman->health && !creep->health)
        gameResult = GameResult::Draw;
    else if (!bomberman->health)
        gameResult = GameResult::CreepWon;
    else if (!creep->health)
        gameResult = GameResult::BombermanWon;
    else return;

    GameStage::changeStage(std::make_unique<ExitStage>(gameResult));
}
