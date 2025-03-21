
#include "game/stages/play/entity_creator.hpp"
#include "enums/resource_id.hpp"
#include "game/stages/play/components/animated.hpp"
#include "game/stages/play/components/bomb.hpp"
#include "game/stages/play/components/collidable.hpp"
#include "game/stages/play/components/drawable.hpp"
#include "game/stages/play/components/flame.hpp"
#include "game/stages/play/components/movable.hpp"
#include "game/stages/play/components/player.hpp"
#include "game/stages/play/components/portal.hpp"
#include "game/stages/play/components/power_up.hpp"
#include "game/stages/play/components/tile.hpp"
#include "game/stages/play/components/transformable.hpp"
#include "game/stages/play/components/z_index.hpp"
#include "game/stages/play/events/spawn_bomb_event.hpp"
#include "game/stages/play/events/spawn_flame_event.hpp"
#include "game/stages/play/events/spawn_portal_event.hpp"
#include "game/stages/play/events/spawn_power_up_event.hpp"
#include "game/stages/play/events/spawn_tile_event.hpp"
#include "util/constants.hpp"
#include "util/converters.hpp"      // TODO: Needed?
#include "util/resource_holder.hpp" // TODO: move to game?
#include "util/tile_calculator.hpp"
#include <entt/entity/fwd.hpp>
#include <entt/entity/registry.hpp>

entt::entity EntityCreator::createBomberman()
{
    auto entity = registry.create();
    registry.emplace<Drawable>(entity, textures.getResource(ResourceID::BombermanFront));
    registry.emplace<Transformable>(
        entity, Transformable{toVector2f(BOMBERMAN_SIZE), calculatePositionInTileCenter({1, 1}, BOMBERMAN_SIZE)});
    registry.emplace<Movable>(entity, Movable{PLAYER_INITIAL_SPEED, Direction::None});
    registry.emplace<Animated>(
        entity, Animated{{BOMBERMAN_SPRITE_SIZE.x, BOMBERMAN_SPRITE_SIZE.y}, 8, PLAYER_INITIAL_ANIM_SPEED});
    registry.emplace<Collidable>(entity);
    registry.emplace<Player>(entity);
    registry.emplace<Bomberman>(entity);
    registry.emplace<ZIndex>(entity, PLAYERS_Z_INDEX);
    return entity;
}

entt::entity EntityCreator::createCreep()
{
    auto entity = registry.create();
    registry.emplace<Drawable>(entity, textures.getResource(ResourceID::CreepFront));
    registry.emplace<Transformable>(
        entity, Transformable{toVector2f(CREEP_SIZE), calculatePositionInTileCenter({19, 19}, CREEP_SIZE)});
    registry.emplace<Movable>(entity, Movable{PLAYER_INITIAL_SPEED, Direction::None});
    registry.emplace<Animated>(entity,
                               Animated{{CREEP_SPRITE_SIZE.x, CREEP_SPRITE_SIZE.y}, 6, PLAYER_INITIAL_ANIM_SPEED});
    registry.emplace<Collidable>(entity);
    registry.emplace<Player>(entity);
    registry.emplace<Creep>(entity);
    registry.emplace<ZIndex>(entity, PLAYERS_Z_INDEX);
    return entity;
}

entt::entity EntityCreator::createEmptyTile(const SpawnTileEvent& event)
{
    auto entity = registry.create();

    registry.emplace<Transformable>(entity, Transformable{toVector2f(TILE_SIZE), event.position});
    registry.emplace<Tile>(entity, Tile{.tileType = event.tileType, .hasBomb = false});

    registry.emplace<ZIndex>(entity, GROUND_Z_INDEX);
    registry.emplace<Drawable>(entity, textures.getResource(ResourceID::BackgroundTile));

    return entity;
}

entt::entity EntityCreator::createSolidBlock(const SpawnTileEvent& event)
{
    auto entity = registry.create();
    registry.emplace<Transformable>(entity, Transformable{toVector2f(TILE_SIZE), event.position});
    registry.emplace<Tile>(entity, Tile{.tileType = event.tileType, .hasBomb = false});

    registry.emplace<ZIndex>(entity, BLOCKS_Z_INDEX);
    registry.emplace<Collidable>(entity, Collidable{CollidableType::Blocking, {}});
    registry.emplace<Drawable>(entity, textures.getResource(ResourceID::SolidBlock));

    return entity;
}

entt::entity EntityCreator::createExplodableBlock(const SpawnTileEvent& event)
{
    auto entity = registry.create();
    registry.emplace<Transformable>(entity, Transformable{toVector2f(TILE_SIZE), event.position});
    registry.emplace<Tile>(entity, Tile{.tileType = event.tileType, .hasBomb = false});

    registry.emplace<ZIndex>(entity, BLOCKS_Z_INDEX);
    registry.emplace<Collidable>(entity, Collidable{CollidableType::Blocking, {}});
    registry.emplace<Drawable>(entity, textures.getResource(ResourceID::ExplodableBlock));

    return entity;
}

entt::entity EntityCreator::createFinishAnimationBlock(const SpawnTileEvent& event)
{
    auto entity = registry.create();
    registry.emplace<Transformable>(entity, Transformable{toVector2f(TILE_SIZE), event.position});
    registry.emplace<Tile>(entity, Tile{.tileType = event.tileType, .hasBomb = false});

    registry.emplace<ZIndex>(entity, TOP_LEVEL_Z_INDEX);
    registry.emplace<Drawable>(entity, textures.getResource(ResourceID::SolidRedBlock));

    return entity;
}

entt::entity EntityCreator::createFlame(const SpawnFlameEvent& event)
{
    auto flame = registry.create();
    registry.emplace<Flame>(flame, Flame{2});
    registry.emplace<Transformable>(flame, Transformable{toVector2f(FLAME_SPRITE_SIZE), event.position});
    registry.emplace<Drawable>(flame, textures.getResource(ResourceID::Flame));
    registry.emplace<Animated>(flame, FLAME_SPRITE_SIZE, 5, 0.1);
    registry.emplace<ZIndex>(flame, FLAME_Z_INDEX);
    return flame;
}

void EntityCreator::createPowerUp(const SpawnPowerUpEvent& event)
{
    const auto tileIndexToSpawn = calculateTileIndexForPosition(event.position);
    const auto positionInTile = calculatePositionInTileCenter(tileIndexToSpawn, POWER_UP_SPRITE_SIZE);

    std::uniform_int_distribution<> dist(0, 100);
    const auto powerUpProbability = 25;
    const auto randomProbability = dist(generator);

    if (randomProbability <= powerUpProbability)
    {
        dist = std::uniform_int_distribution<>(0, static_cast<int>(PowerUpType::Count) - 1);
        PowerUpType powerUpType{dist(generator)};

        auto powerUp = registry.create();
        registry.emplace<PowerUp>(powerUp, PowerUp{powerUpType});
        registry.emplace<Drawable>(powerUp, getPowerUpTexture(powerUpType));
        registry.emplace<ZIndex>(powerUp, ADDON_Z_INDEX);
        registry.emplace<Transformable>(powerUp, Transformable{toVector2f(POWER_UP_SPRITE_SIZE), positionInTile});
    }
}

entt::entity EntityCreator::createPortal(const SpawnPortalEvent& event) const
{
    auto portal = registry.create();
    registry.emplace<Transformable>(portal, Transformable{toVector2f(PORTAL_SPRITE_SIZES), event.position});
    registry.emplace<Drawable>(portal, textures.getResource(ResourceID::Portal));
    registry.emplace<Portal>(portal);
    registry.emplace<Collidable>(portal, Collidable{CollidableType::NonBlocking, {}});
    registry.emplace<ZIndex>(portal, PORTAL_Z_INDEX);
    return portal;
}

entt::entity EntityCreator::createBomb(const SpawnBombEvent& event) const
{
    const auto& playerPosition = registry.get<Transformable>(event.player);
    const auto playerTileIndex = calculateTileIndexForCenterPosition(playerPosition.position, playerPosition.size);
    const auto& player = registry.get<Player>(event.player);

    auto bomb = registry.create();
    registry.emplace<Drawable>(bomb, textures.getResource(ResourceID::Bomb));
    registry.emplace<Transformable>(
        bomb,
        Transformable{toVector2f(BOMB_SPRITE_SIZE), calculatePositionInTileCenter(playerTileIndex, BOMB_SPRITE_SIZE)});
    registry.emplace<Animated>(bomb, BOMB_SPRITE_SIZE, 3, 0.1);
    registry.emplace<Bomb>(bomb, Bomb{event.player, 3, player.bombsRange});
    registry.emplace<ZIndex>(bomb, BOMB_Z_INDEX);

    Collidable collidable{CollidableType::Blocking, {}};
    registry.view<Player>().each(
        [&collidable](const entt::entity entity, Player&) { collidable.skipCollisionEntities.push_back(entity); });
    registry.emplace<Collidable>(bomb, std::move(collidable));

    return bomb;
}

const sf::Texture& EntityCreator::getPowerUpTexture(const PowerUpType type) const
{
    switch (type)
    {
    case PowerUpType::Bomb:
        return textures.getResource(ResourceID::BombPowerUp);
    case PowerUpType::Speed:
        return textures.getResource(ResourceID::SpeedPowerUp);
    case PowerUpType::Flame:
        return textures.getResource(ResourceID::FlamePowerUp);
    default:
        abort(); // should never happen
    }
}