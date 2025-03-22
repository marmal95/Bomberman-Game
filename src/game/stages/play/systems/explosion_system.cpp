#include "game/stages/play/systems/explosion_system.hpp"
#include "game/stages/play/components/bomb.hpp"
#include "game/stages/play/components/flame.hpp"
#include "game/stages/play/components/map.hpp"
#include "game/stages/play/components/player.hpp"
#include "game/stages/play/components/transformable.hpp"
#include "game/stages/play/events/spawn_flame_event.hpp"
#include "util/tile_calculator.hpp"
#include "util/types.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

ExplosionSystem::ExplosionSystem(entt::registry& registry,
                                 entt::dispatcher& dispatcher,
                                 const ResourceHolder<sf::SoundBuffer, ResourceID>& sounds)
    : registry{registry}, dispatcher{dispatcher}, sounds{sounds}, ongoingSounds{}
{
}

void ExplosionSystem::update(const TimeDelta dt)
{
    removeFinishedSounds();

    registry.view<Flame>().each([&](const entt::entity entity, Flame& flame) {
        flame.timeToVanish -= dt;

        if (flame.timeToVanish <= 0)
        {
            registry.destroy(entity);
        }
    });

    const auto map = registry.view<Map>().front();
    auto& mapComponent = registry.get<Map>(map);

    registry.view<Bomb, Transformable>().each(
        [&](const entt::entity bombEntity, Bomb& bomb, const Transformable& transformable) {
            bomb.timeToExplode -= dt;

            if (bomb.timeToExplode <= 0)
            {
                const auto bombIndex = calculateTileIndexForPosition(transformable.position);

                for (int i = 1; i <= bomb.range; i++)
                    if (!spawnFlameInRow(mapComponent, bombIndex, i))
                        break;

                for (int i = 0; i >= -bomb.range; i--)
                    if (!spawnFlameInRow(mapComponent, bombIndex, i))
                        break;

                for (int i = 1; i <= bomb.range; i++)
                    if (!spawnFlameInCol(mapComponent, bombIndex, i))
                        break;

                for (int i = 0; i >= -bomb.range; i--)
                    if (!spawnFlameInCol(mapComponent, bombIndex, i))
                        break;

                auto& bombOwner = registry.get<Player>(bomb.spawner);
                ++bombOwner.bombsNum;
                registry.destroy(bombEntity);
                mapComponent.getTile({bombIndex.x, bombIndex.y}).hasBomb = false;

                ongoingSounds.emplace_back(sounds.getResource(ResourceID::BombSound));
                ongoingSounds.back().play();
            }
        });
}

void ExplosionSystem::removeFinishedSounds()
{
    ongoingSounds.remove_if([](auto& snd) { return snd.getStatus() == sf::Sound::Stopped; });
}

bool ExplosionSystem::spawnFlameInRow(const Map& map, const TileIndex bombIndex, const int i) const
{
    if (auto columnIndex = bombIndex.y + i; columnIndex >= 0 && columnIndex < WIDTH_TILES_NUM &&
                                            map.getTile({bombIndex.x, columnIndex}).tileType != TileType::SolidBlock)
    {
        const auto isExplodable = map.getTile({bombIndex.x, columnIndex}).tileType == TileType::ExplodableBlock;
        spawnFlame(calculatePositionInTileCenter(TileIndex{bombIndex.x, columnIndex}, FLAME_SPRITE_SIZE));
        return !isExplodable;
    }
    return false;
}

bool ExplosionSystem::spawnFlameInCol(const Map& map, const TileIndex bombIndex, const int i) const
{
    if (auto rowIndex = bombIndex.x - i; rowIndex >= 0 && rowIndex < HEIGHT_TILES_NUM &&
                                         map.getTile({rowIndex, bombIndex.y}).tileType != TileType::SolidBlock)
    {
        const auto isExplodable = map.getTile({rowIndex, bombIndex.y}).tileType == TileType::ExplodableBlock;
        spawnFlame(calculatePositionInTileCenter(TileIndex{rowIndex, bombIndex.y}, FLAME_SPRITE_SIZE));
        return !isExplodable;
    }
    return false;
}

void ExplosionSystem::spawnFlame(const sf::Vector2f position) const
{
    dispatcher.trigger<SpawnFlameEvent>(SpawnFlameEvent{position});
}