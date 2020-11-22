#include "EcsGameplay.hpp"
#include "MoveSystem.hpp"
#include "DrawSystem.hpp"
#include "AnimateSystem.hpp"
#include "CollisionSystem.hpp"
#include "GameplayStage.hpp"
#include "Transformable.hpp"
#include "Drawable.hpp"
#include "Movable.hpp"
#include "Animated.hpp"
#include "Player.hpp"
#include "Collidable.hpp"
#include "Constants.hpp"

EcsGameplay::EcsGameplay(const GameplayStage& stage)
    : gameplayStage{ stage }
{
    systems.add<MoveSystem>();
    systems.add<DrawSystem>(gameplayStage.getTextures());
    systems.add<AnimateSystem>();
    systems.add<CollisionSystem>();
    systems.configure();

    createMap();
    createPlayer();
}

bool EcsGameplay::update(const entityx::TimeDelta dt)
{
    systems.update<MoveSystem>(dt);
    systems.update<AnimateSystem>(dt);
    systems.update<CollisionSystem>(dt);
    return true;
}

void EcsGameplay::draw(sf::RenderWindow&)
{
    systems.update<DrawSystem>({});
}

void EcsGameplay::handleEvent(sf::Event&)
{
}

void EcsGameplay::createMap()
{
    for (int i = 0; i < HEIGHT_TILES_NUM; i++)
    {
        for (int j = 0; j < WIDTH_TILES_NUM; j++)
        {
            if (i == 0 || i == 20 || j == 0 || j == 20 || i % 2 == 0 && j % 2 == 0)
            {
                auto tile = entities.create();
                tile.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::SolidBlock));
                tile.assign<Transformable>(Transformable{ { 64.f, 64.f },{ j * 64.f,{ i * 64.f } } });
                tile.assign<Collidable>();
            }
            else
            {
                auto tile = entities.create();
                tile.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::BackgroundTile));
                tile.assign<Transformable>(Transformable{ { 64.f, 64.f },{ j * 64.f,{ i * 64.f } } });
            }
        }
    }
}

void EcsGameplay::createPlayer()
{
    auto entity = entities.create();
    entity.assign<Drawable>(gameplayStage.getTextures().getResource(ResourceID::BombermanFront));
    entity.assign<Transformable>(Transformable{ { 30, 30 }, { 300, 300 } });
    entity.assign<Movable>(Movable{ { 128, 128 }, Direction::None });
    entity.assign<Animated>(Animated{ 64, 128, 8, 1 });
    entity.assign<Collidable>();
    entity.assign<Player>();
}
