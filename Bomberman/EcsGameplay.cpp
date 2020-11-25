#include "EcsGameplay.hpp"
#include "MoveSystem.hpp"
#include "DrawSystem.hpp"
#include "AnimateSystem.hpp"
#include "CollisionSystem.hpp"
#include "ExplosionSystem.hpp"
#include "GameplayStage.hpp"
#include "Transformable.hpp"
#include "Drawable.hpp"
#include "Movable.hpp"
#include "Animated.hpp"
#include "Player.hpp"
#include "Collidable.hpp"
#include "Constants.hpp"
#include "SpawnBombEvent.hpp"
#include "Map.hpp"

EcsGameplay::EcsGameplay(const GameplayStage & stage)
    : gameplayStage{ stage }
{
    systems.add<MoveSystem>();
    systems.add<DrawSystem>(gameplayStage.getTextures());
    systems.add<AnimateSystem>();
    systems.add<CollisionSystem>();
    systems.add<ExplosionSystem>(gameplayStage.getTextures());
    systems.configure();

    createMap();
    createPlayer();
}    

bool EcsGameplay::update(const entityx::TimeDelta dt)
{
    systems.update<MoveSystem>(dt);
    systems.update<ExplosionSystem>(dt);
    systems.update<AnimateSystem>(dt);
    systems.update<CollisionSystem>(dt);
    return true;
}

void EcsGameplay::draw(sf::RenderWindow&)
{
    systems.update<DrawSystem>({});
}

void EcsGameplay::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::EventType::KeyReleased
        && event.key.code == sf::Keyboard::Space)
    {
        auto player = entities.entities_with_components<Player>().begin();
        events.emit<SpawnBombEvent>({ *player });
    }
}

void EcsGameplay::createMap()
{
    Map map{};

    for (int i = 0; i < HEIGHT_TILES_NUM; i++)
    {
        for (int j = 0; j < WIDTH_TILES_NUM; j++)
        {
            auto tile = entities.create();
            tile.assign<Transformable>(Transformable{ { 64.f, 64.f }, { j * 64.f, i * 64.f } });

            if (i == 0 || i == 20 || j == 0 || j == 20 || i % 2 == 0 && j % 2 == 0)
            {
                tile.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::SolidBlock));
                tile.assign<Collidable>();
                tile.assign<Tile>(Tile{ TileType::SolidBlock });
                map.tiles[i][j] = { TileType::SolidBlock };
            }
            else
            {
                tile.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::BackgroundTile));
                tile.assign<Tile>(Tile{ TileType::None });
                map.tiles[i][j] = { TileType::None };
            }
        }
    }



    auto mapEntity = entities.create();
    mapEntity.assign<Map>(map);
}

void EcsGameplay::createPlayer()
{
    auto entity = entities.create();
    entity.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::BombermanFront));
    entity.assign<Transformable>(Transformable{ { 30, 30 }, { 300, 300 } });
    entity.assign<Movable>(Movable{ { 4*128, 4*128 }, Direction::None });
    entity.assign<Animated>(Animated{ 64, 128, 8, 1 });
    entity.assign<Collidable>();
    entity.assign<Player>();
    events.emit<MoveChangeEvent>({ entity, Direction::None });
}
