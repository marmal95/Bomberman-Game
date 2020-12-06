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
#include "FinishGameEvent.hpp"
#include "Map.hpp"
#include "ExitStage.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <random>
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

EcsGameplay::EcsGameplay(const GameplayStage& stage)
    : gameplayStage{ stage }, gameStatus{ GameStatus::Running }
{
    systems.add<SpawnSystem>(gameplayStage.getTextures());
    systems.add<MoveSystem>();
    systems.add<CollisionSystem>();
    systems.add<ExplosionSystem>(gameplayStage.getSounds());
    systems.add<AnimateSystem>();
    systems.add<DrawSystem>(gameplayStage.getTextures());
    systems.configure();
    events.subscribe<GameFinishedEvent>(*this);

    createBomberman();
    createCreep();
    createMap();
}

void EcsGameplay::receive(const GameFinishedEvent&)
{
    const auto bomberman = (*entities.entities_with_components<Player, Bomberman>().begin()).component<Player>();
    const auto creep = (*entities.entities_with_components<Player, Creep>().begin()).component<Player>();

    GameStatus finalGameStatus{};
    if (bomberman->health <= 0 && creep->health <= 0)
        finalGameStatus = GameStatus::Draw;
    else if (bomberman->health > 0)
        finalGameStatus = GameStatus::BombermanWon;
    else if (creep->health > 0)
        finalGameStatus = GameStatus::CreepWon;
    GameStage::changeStage(std::make_unique<ExitStage>(finalGameStatus));
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
        auto bomberman = entities.entities_with_components<Bomberman>().begin();
        auto creep = entities.entities_with_components<Creep>().begin();

        if (event.key.code == sf::Keyboard::RControl && (*bomberman).has_component<Movable>())
            events.emit<SpawnBombEvent>({ *bomberman });

        if (event.key.code == sf::Keyboard::LControl && (*creep).has_component<Movable>())
            events.emit<SpawnBombEvent>({ *creep });
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

    const auto bombermanPositionIndex = calculateTileIndexForPosition(bombermanPosition);
    const auto creepPositionIndex = calculateTileIndexForPosition(creepPosition);

    const auto splitPos = static_cast<uint32_t>(blankTilesIndexes.size() * (70 / 100.f));
    blankTilesIndexes.erase(std::begin(blankTilesIndexes) + splitPos, std::end(blankTilesIndexes));

    for (const auto& blankTileIndex : blankTilesIndexes)
    {
        if ((std::abs(blankTileIndex.x - bombermanPositionIndex.x) >= DISTANCE_TO_EXPLODABLE_BLOCKS ||
            std::abs(blankTileIndex.y - bombermanPositionIndex.y) >= DISTANCE_TO_EXPLODABLE_BLOCKS) &&
            (std::abs(blankTileIndex.x - creepPositionIndex.x) >= DISTANCE_TO_EXPLODABLE_BLOCKS ||
                std::abs(blankTileIndex.y - creepPositionIndex.y) >= DISTANCE_TO_EXPLODABLE_BLOCKS))
        {
            events.emit<SpawnTileEvent>(SpawnTileEvent{ calculatePositionForTileIndex(blankTileIndex), TileType::ExplodableBlock });
            map.tiles[blankTileIndex.x][blankTileIndex.y] = { TileType::ExplodableBlock };
        }
    }
}

void EcsGameplay::createBomberman()
{
    auto entity = entities.create();
    entity.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::BombermanFront));
    entity.assign<Transformable>(Transformable{ toVector2f(BOMBERMAN_SIZE), calculatePositionInTileCenter({ 1, 1 }, BOMBERMAN_SIZE) });
    entity.assign<Movable>(Movable{ toVector2f(PLAYER_INITIAL_SPEED), Direction::None });
    entity.assign<Animated>(Animated{ { BOMBERMAN_SPRITE_SIZE.x, BOMBERMAN_SPRITE_SIZE.y }, 8, PLAYER_INITIAL_ANIM_SPEED });
    entity.assign<Collidable>();
    entity.assign<Player>();
    entity.assign<Bomberman>();
    events.emit<MoveChangeEvent>({ entity, Direction::None });
}

void EcsGameplay::createCreep()
{
    auto entity = entities.create();
    entity.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::CreepFront));
    entity.assign<Transformable>(Transformable{ toVector2f(CREEP_SIZE), calculatePositionInTileCenter({ 19, 19 }, CREEP_SIZE) });
    entity.assign<Movable>(Movable{ toVector2f(PLAYER_INITIAL_SPEED), Direction::None });
    entity.assign<Animated>(Animated{ { CREEP_SPRITE_SIZE.x, CREEP_SPRITE_SIZE.y }, 6, PLAYER_INITIAL_ANIM_SPEED });
    entity.assign<Collidable>();
    entity.assign<Player>();
    entity.assign<Creep>();
    events.emit<MoveChangeEvent>({ entity, Direction::None });
}

void EcsGameplay::checkIsGameOver()
{
    if (gameStatus != GameStatus::Running)
        return;

    const auto bomberman = (*entities.entities_with_components<Player, Bomberman>().begin()).component<Player>();
    const auto creep = (*entities.entities_with_components<Player, Creep>().begin()).component<Player>();

    if (!bomberman->health || !creep->health)
    {
        events.emit<FinishGameEvent>();
        gameStatus = GameStatus::Finishing;
    }
}
