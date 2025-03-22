#include "game/stages/play/systems/collision_system.hpp"
#include "game/stages/play/collision_detector.hpp"
#include "game/stages/play/components/bomb.hpp"
#include "game/stages/play/components/collidable.hpp"
#include "game/stages/play/components/drawable.hpp"
#include "game/stages/play/components/flame.hpp"
#include "game/stages/play/components/map.hpp"
#include "game/stages/play/components/movable.hpp"
#include "game/stages/play/components/player.hpp"
#include "game/stages/play/components/portal.hpp"
#include "game/stages/play/components/power_up.hpp"
#include "game/stages/play/components/tile.hpp"
#include "game/stages/play/components/transformable.hpp"
#include "game/stages/play/events/spawn_power_up_event.hpp"
#include "util/constants.hpp"
#include "util/converters.hpp"
#include "util/tile_calculator.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

CollisionSystem::CollisionSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : registry{registry}, dispatcher{dispatcher}
{
}

void CollisionSystem::update(const TimeDelta dt)
{
    registry.view<Player>().each([&](const entt::entity playerEntity, const Player&) {
        handlePlayerPowerUpsCollisions(playerEntity);
        handlePlayerFlamesCollisions(playerEntity, dt);
        handlePlayerPortalsCollisions(playerEntity);
    });

    handleTilesFlamesCollisions();
    handleBombsFlamesCollisions();
}

void CollisionSystem::handlePlayerPowerUpsCollisions(const entt::entity player) const
{
    registry.view<PowerUp>().each(
        [&](const entt::entity powerUp, const PowerUp&) { handlePowerUpCollision(player, powerUp); });
}

void CollisionSystem::handlePlayerFlamesCollisions(const entt::entity player, const TimeDelta dt) const
{
    auto& playerComponent = registry.get<Player>(player);
    if (playerComponent.immortalTime > 0)
    {
        playerComponent.immortalTime -= dt;
    }
    else
    {
        registry.view<Flame>().each(
            [&](const entt::entity flame, const Flame&) { handleFlameCollision(player, flame); });
    }
}

void CollisionSystem::handlePlayerPortalsCollisions(const entt::entity player) const
{
    registry.view<Portal>().each(
        [this, player](entt::entity portal, const Portal&) { handlePlayerPortalCollision(player, portal); });
}

void CollisionSystem::handleTilesFlamesCollisions() const
{
    registry.view<Flame>().each([&](const entt::entity flame, const Flame&) {
        registry.view<Tile>().each([&](const entt::entity tile, Tile&) { handleTileFlameCollision(tile, flame); });
    });
}

void CollisionSystem::handleBombsFlamesCollisions() const
{
    registry.view<Bomb>().each([&](const entt::entity bomb, const Bomb&) {
        registry.view<Flame>().each([&](const entt::entity flame, Flame&) { handleBombFlameCollision(bomb, flame); });
    });
}

void CollisionSystem::handleFlameCollision(const entt::entity player, const entt::entity flame) const
{
    auto& playerComponent = registry.get<Player>(player);
    auto& playerTransformable = registry.get<Transformable>(player);
    auto& flameTransformable = registry.get<Transformable>(flame);
    const auto collision = detectCollision(playerTransformable, flameTransformable);

    if (collision && playerComponent.immortalTime <= 0)
    {
        playerComponent.health--;
        playerComponent.immortalTime = IMMORTAL_TIME_AFTER_DEAD;

        if (playerComponent.health <= 0)
        {
            if (registry.try_get<Movable>(player))
            {
                registry.remove<Movable>(player);
            }
            if (registry.try_get<Drawable>(player))
            {
                registry.remove<Drawable>(player);
            }
        }
    }
}

void CollisionSystem::handlePowerUpCollision(const entt::entity player, const entt::entity powerUp) const
{
    auto& playerTransformable = registry.get<Transformable>(player);
    auto& powerUpTransformable = registry.get<Transformable>(powerUp);
    const auto collision = detectCollision(playerTransformable, powerUpTransformable);

    if (collision)
    {
        auto& playerComponent = registry.get<Player>(player);
        auto& movableComponent = registry.get<Movable>(player);
        const auto& powerUpComponent = registry.get<PowerUp>(powerUp);

        switch (powerUpComponent.type)
        {
        case PowerUpType::Bomb:
            playerComponent.bombsNum++;
            break;
        case PowerUpType::Flame:
            playerComponent.bombsRange++;
            break;
        case PowerUpType::Speed:
            movableComponent.velocity *= 1.2f;
            break;
        default:
            break;
        }

        registry.destroy(powerUp);
    }
}

void CollisionSystem::handleTileFlameCollision(const entt::entity tile, const entt::entity flame) const
{
    const auto& tileTransformable = registry.get<Transformable>(tile);
    const auto& flameTransformable = registry.get<Transformable>(flame);
    const auto tileIndex = calculateTileIndexForPosition(tileTransformable.position);

    const auto map = registry.view<Map>().front();
    auto& mapComponent = registry.get<Map>(map);

    if (mapComponent.getTile({tileIndex.x, tileIndex.y}).tileType != TileType::ExplodableBlock)
    {
        return;
    }

    if (const auto collisionInfo = detectCollision(tileTransformable, flameTransformable); collisionInfo.has_value())
    {
        const auto tileIndexPosition = calculateTileIndexForPosition(tileTransformable.position);
        mapComponent.getTile({tileIndexPosition.x, tileIndexPosition.y}).tileType = TileType::None;
        dispatcher.trigger<SpawnPowerUpEvent>(SpawnPowerUpEvent{tileTransformable.position});
        registry.destroy(tile);
    }
}

void CollisionSystem::handleBombFlameCollision(const entt::entity bomb, const entt::entity flame) const
{
    const auto& tileTransformable = registry.get<Transformable>(bomb);
    const auto& flameTransformable = registry.get<Transformable>(flame);

    if (const auto collisionInfo = detectCollision(tileTransformable, flameTransformable); collisionInfo.has_value())
    {
        auto& bombComponent = registry.get<Bomb>(bomb);
        bombComponent.timeToExplode = 0;
    }
}

void CollisionSystem::handlePlayerPortalCollision(const entt::entity player, const entt::entity portal) const
{
    auto& sourcePortalCollidable = registry.get<Collidable>(portal);
    auto& playerTransformable = registry.get<Transformable>(player);
    const auto& portalTransformable = registry.get<Transformable>(portal);

    const auto collisionInfo = detectCollision(playerTransformable, portalTransformable);

    if (collisionInfo && !shouldSkipCollision(player, sourcePortalCollidable))
    {
        registry.view<Portal, Transformable, Collidable>().each([&](const entt::entity targetPortal,
                                                                    const Portal&,
                                                                    const Transformable& targetPortalTransformable,
                                                                    Collidable& targetPortalCollidable) {
            if (portal != targetPortal)
            {
                const auto targetPortalIndex = calculateTileIndexForPosition(targetPortalTransformable.position);
                playerTransformable.position =
                    calculatePositionInTileCenter(targetPortalIndex, toVector2i(playerTransformable.size));
                targetPortalCollidable.skipCollisionEntities.push_back(player);
                return;
            }
        });
    }
    else if (!collisionInfo && shouldSkipCollision(player, sourcePortalCollidable))
    {
        auto& skipEntities = sourcePortalCollidable.skipCollisionEntities;
        skipEntities.erase(std::remove(skipEntities.begin(), skipEntities.end(), player), skipEntities.end());
    }
}

bool CollisionSystem::shouldSkipCollision(const entt::entity entity, const Collidable& collidable) const
{
    auto& skipEntities = collidable.skipCollisionEntities;
    return std::find(skipEntities.begin(), skipEntities.end(), entity) != skipEntities.end();
}