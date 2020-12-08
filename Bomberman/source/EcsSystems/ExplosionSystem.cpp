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
#include "Utils.hpp"


ExplosionSystem::ExplosionSystem(const ResourceHolder<sf::SoundBuffer, ResourceID>& sounds)
    : sounds{ sounds }
{}

void ExplosionSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
    removeFinishedSounds();

    auto& map = *(*es.entities_with_components<Map>().begin()).component<Map>();

    es.each<Flame>([&](entityx::Entity entity, Flame& flame) {
        flame.timeToVanish -= dt;

        if (flame.timeToVanish <= 0)
            entity.destroy();
    });

    es.each<Bomb, Transformable>([&](entityx::Entity entity, Bomb& bomb, Transformable& transformable) {
        bomb.timeToExplode -= dt;

        if (bomb.timeToExplode <= 0)
        {
            const sf::Vector2i bombIndex = calculateTileIndexForPosition(transformable.position);

            for (int i = 1; i <= bomb.range; i++)
                if (!spawnFlameInRow(es, events, map, entity, bombIndex, i))
                    break;

            for (int i = 0; i >= -bomb.range; i--)
                if (!spawnFlameInRow(es, events, map, entity, bombIndex, i))
                    break;

            for (int i = 1; i <= bomb.range; i++)
                if (!spawnFlameInCol(es,events, map, entity, bombIndex, i))
                    break;

            for (int i = 0; i >= -bomb.range; i--)
                if (!spawnFlameInCol(es, events, map, entity, bombIndex, i))
                    break;

            auto& bombOwner = *bomb.spawner.component<Player>();
            ++bombOwner.bombsNum;
            entity.destroy();
            map.tiles[bombIndex.x][bombIndex.y].hasBomb = false;

            ongoingSounds.emplace_back(sounds.getResource(ResourceID::BombSound));
            ongoingSounds.back().play();
        }
    });
}

void ExplosionSystem::removeFinishedSounds()
{
    ongoingSounds.remove_if([](auto& snd) { return snd.getStatus() == sf::Sound::Stopped; });
}

bool ExplosionSystem::spawnFlameInRow(
    entityx::EntityManager& es, entityx::EventManager& events, const Map& map, entityx::Entity bomb, const sf::Vector2i bombIndex, const int i) const
{
    if (auto columnIndex = bombIndex.y + i; columnIndex >= 0 && columnIndex < WIDTH_TILES_NUM &&
        map.tiles[bombIndex.x][columnIndex].tileType != TileType::SolidBlock)
    {
        const auto isExplodable = map.tiles[bombIndex.x][columnIndex].tileType == TileType::ExplodableBlock;
        spawnFlame(es, events, calculatePositionInTileCenter(sf::Vector2i{ bombIndex.x, columnIndex }, FLAME_SPRITE_SIZE));
        return !isExplodable;
    }
    return false;
}

bool ExplosionSystem::spawnFlameInCol(
    entityx::EntityManager& es, entityx::EventManager& events,const Map& map, entityx::Entity bomb, const sf::Vector2i bombIndex, const int i) const
{
    if (auto rowIndex = bombIndex.x - i; rowIndex >= 0 && rowIndex < HEIGHT_TILES_NUM &&
        map.tiles[rowIndex][bombIndex.y].tileType != TileType::SolidBlock)
    {
        const auto isExplodable = map.tiles[rowIndex][bombIndex.y].tileType == TileType::ExplodableBlock;
        spawnFlame(es, events, calculatePositionInTileCenter(sf::Vector2i{ rowIndex, bombIndex.y }, FLAME_SPRITE_SIZE));
        return !isExplodable;
    }
    return false;
}

void ExplosionSystem::spawnFlame(entityx::EntityManager& es, entityx::EventManager& events, const sf::Vector2f position) const
{
    events.emit<SpawnFlameEvent>(SpawnFlameEvent{ position });
}
