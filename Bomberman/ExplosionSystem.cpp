#include "ExplosionSystem.hpp"
#include "Transformable.hpp"
#include "Drawable.hpp"
#include "Animated.hpp"
#include "Collidable.hpp"
#include "Map.hpp"
#include "Bomb.hpp"
#include "Flame.hpp"
#include "Player.hpp"

ExplosionSystem::ExplosionSystem(const ResourceHolder<sf::Texture, ResourceID>& textures)
    : textures{ textures }
{}

void ExplosionSystem::update(entityx::EntityManager& es, entityx::EventManager&, entityx::TimeDelta dt)
{
    handleSpawnBombEvents(es);

    const auto& map = *(*es.entities_with_components<Map>().begin()).component<Map>();
  
    es.each<Flame>([&](entityx::Entity entity, Flame& flame) {
        flame.timeToVanish -= dt;

        if (flame.timeToVanish <= 0)
            entity.destroy();
    });

    es.each<Bomb, Transformable>([&](entityx::Entity entity, Bomb& bomb, Transformable& transformable) {
        bomb.timeToExplode -= dt;

        if (bomb.timeToExplode <= 0)
        {
            const sf::Vector2i bombIndex = { static_cast<int>(transformable.position.x) / 64,
                                             static_cast<int>(transformable.position.y) / 64 };

            for (int i = 0; i < bomb.range; i++)
            {
                if (!spawnFlameInRow(es, map, entity, bombIndex, i))
                    break;
            }

            for (int i = 0; i > -bomb.range; i--)
            {
                if (!spawnFlameInRow(es, map, entity, bombIndex, i))
                    break;
            }

            for (int i = 0; i < bomb.range; i++)
            {
                if (!spawnFlameInCol(es, map, entity, bombIndex, i))
                    break;
            }

            for (int i = 0; i > -bomb.range; i--)
            {
                if (!spawnFlameInCol(es, map, entity, bombIndex, i))
                    break;
            }

            auto& bombOwner = *bomb.spawner.component<Player>();
            ++bombOwner.bombsNum;
            entity.destroy();
        }
    });
}

void ExplosionSystem::configure(entityx::EventManager& events)
{
    events.subscribe<SpawnBombEvent>(*this);
}

void ExplosionSystem::receive(const SpawnBombEvent& event)
{
    spawnBombEvents.push_back(event);
}

void ExplosionSystem::handleSpawnBombEvents(entityx::EntityManager& es)
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

void ExplosionSystem::spawnBomb(entityx::EntityManager& es, SpawnBombEvent& event)
{
    auto playerPosition = event.player.component<Transformable>()->position;
    playerPosition.x = static_cast<int>(playerPosition.x + 15) / 64 * 64 + 8;
    playerPosition.y = static_cast<int>(playerPosition.y + 15) / 64 * 64 + 8;

    auto bomb = es.create();
    bomb.assign<Drawable>(textures.getResource(ResourceID::Bomb));
    bomb.assign<Transformable>(Transformable{ { 48, 48 }, playerPosition });
    bomb.assign<Animated>(48, 48, 3, 0.1);
    bomb.assign<Bomb>(Bomb{ event.player, 3, 4 });
    bomb.assign<Collidable>(Collidable{ event.player });
}

void ExplosionSystem::spawnFlame(entityx::EntityManager& es, const sf::Vector2f position) const
{
    auto flame = es.create();
    flame.assign<Flame>(Flame{ 2 });
    flame.assign<Transformable>(Transformable{ { 48, 48 }, position });
    flame.assign<Drawable>(textures.getResource(ResourceID::Flame));
    flame.assign<Animated>(48, 48, 5, 0.1);
    flame.assign<Collidable>();
}

bool ExplosionSystem::spawnFlameInRow(entityx::EntityManager& es,const Map& map, entityx::Entity bomb, const sf::Vector2i bombIndex, const int i) const
{
    const auto& transformable = *bomb.component<Transformable>();
    if (auto rowIndex = bombIndex.y + i; rowIndex >= 0 && rowIndex < HEIGHT_TILES_NUM &&
        map.tiles[rowIndex][bombIndex.x].tileType != TileType::SolidBlock)
    {
        spawnFlame(es, { transformable.position.x, transformable.position.y + 64.f * i });
        return true;
    }
    return false;
}

bool ExplosionSystem::spawnFlameInCol(entityx::EntityManager& es, const Map& map, entityx::Entity bomb, const sf::Vector2i bombIndex, const int i) const
{
    const auto& transformable = *bomb.component<Transformable>();
    if (auto columnIndex = bombIndex.x - i; columnIndex >= 0 && columnIndex < WIDTH_TILES_NUM &&
        map.tiles[bombIndex.y][columnIndex].tileType != TileType::SolidBlock)
    {
        spawnFlame(es, { transformable.position.x - 64.f * i, transformable.position.y });
        return true;
    }
    return false;
}
