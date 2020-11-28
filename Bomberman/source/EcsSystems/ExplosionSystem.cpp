#include "ExplosionSystem.hpp"
#include "Transformable.hpp"
#include "Drawable.hpp"
#include "Animated.hpp"
#include "Collidable.hpp"
#include "Map.hpp"
#include "Bomb.hpp"
#include "Flame.hpp"
#include "Player.hpp"
#include "SpawnFlameEvent.hpp"
#include "SpawnBombEvent.hpp"


void ExplosionSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
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

            for (int i = 1; i <= bomb.range; i++)
            {
                if (!spawnFlameInRow(es, events, map, entity, bombIndex, i))
                    break;
            }

            for (int i = 0; i >= -bomb.range; i--)
            {
                if (!spawnFlameInRow(es, events, map, entity, bombIndex, i))
                    break;
            }

            for (int i = 1; i <= bomb.range; i++)
            {
                if (!spawnFlameInCol(es,events, map, entity, bombIndex, i))
                    break;
            }

            for (int i = 0; i >= -bomb.range; i--)
            {
                if (!spawnFlameInCol(es, events, map, entity, bombIndex, i))
                    break;
            }

            auto& bombOwner = *bomb.spawner.component<Player>();
            ++bombOwner.bombsNum;
            entity.destroy();
        }
    });
}

bool ExplosionSystem::spawnFlameInRow(
    entityx::EntityManager& es, entityx::EventManager& events, const Map& map, entityx::Entity bomb, const sf::Vector2i bombIndex, const int i) const
{
    const auto& transformable = *bomb.component<Transformable>();
    if (auto rowIndex = bombIndex.y + i; rowIndex >= 0 && rowIndex < HEIGHT_TILES_NUM &&
        map.tiles[rowIndex][bombIndex.x].tileType != TileType::SolidBlock)
    {
        const auto isExplodable = map.tiles[rowIndex][bombIndex.x].tileType == TileType::ExplodableBlock;
        spawnFlame(es, events, { transformable.position.x, transformable.position.y + 64.f * i });
        return !isExplodable;
    }
    return false;
}

bool ExplosionSystem::spawnFlameInCol(
    entityx::EntityManager& es, entityx::EventManager& events,const Map& map, entityx::Entity bomb, const sf::Vector2i bombIndex, const int i) const
{
    const auto& transformable = *bomb.component<Transformable>();
    if (auto columnIndex = bombIndex.x - i; columnIndex >= 0 && columnIndex < WIDTH_TILES_NUM &&
        map.tiles[bombIndex.y][columnIndex].tileType != TileType::SolidBlock)
    {
        const auto isExplodable = map.tiles[bombIndex.y][columnIndex].tileType == TileType::ExplodableBlock;
        spawnFlame(es, events, { transformable.position.x - 64.f * i, transformable.position.y });
        return !isExplodable;
    }
    return false;
}

void ExplosionSystem::spawnFlame(entityx::EntityManager& es, entityx::EventManager& events, const sf::Vector2f position) const
{
    events.emit<SpawnFlameEvent>(SpawnFlameEvent{ position });
}