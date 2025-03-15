#include "game/stages/play/systems/collision_system.hpp"
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

CollisionSystem::CollisionSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : registry{registry}, dispatcher{dispatcher}
{
}

void CollisionSystem::update(const TimeDelta dt)
{
    registry.view<Player, Collidable>().each(
        [&](const entt::entity playerEntity, const Player&, Collidable& playerCollidable) {
            playerCollidable.direction = Direction::None;

            handlePlayerTilesCollisions(playerEntity);
            handlePlayerBombsCollisions(playerEntity);
            handlePlayerPowerUpsCollisions(playerEntity);
            handlePlayerFlamesCollisions(playerEntity, dt);
            handlePlayerPortalsCollisions(playerEntity);
            handleTilesFlamesCollisions();
        });
}

void CollisionSystem::handlePlayerTilesCollisions(const entt::entity player) const
{
    registry.view<Tile, Collidable>().each(
        [&](const entt::entity tile, const Tile&, const Collidable&) {
            handleBlockingCollision(player, tile);
        });
}

void CollisionSystem::handlePlayerBombsCollisions(const entt::entity player) const
{
    registry.view<Bomb, Collidable>().each(
        [&](const entt::entity bomb, const Bomb&, const Collidable&) {
            handleBlockingCollision(player, bomb);
        });
}

void CollisionSystem::handlePlayerPowerUpsCollisions(const entt::entity player) const
{
    registry.view<PowerUp, Collidable>().each(
        [&](const entt::entity powerUp, const PowerUp&, const Collidable&) {
            handlePowerUpCollision(player, powerUp);
        });
}

void CollisionSystem::handlePlayerFlamesCollisions(const entt::entity player,
                                                   const TimeDelta dt) const
{
    auto& playerComponent = registry.get<Player>(player);
    if (playerComponent.immortalTime > 0)
    {
        playerComponent.immortalTime -= dt;
    }
    else
    {
        registry.view<Flame, Collidable>().each(
            [&](const entt::entity flame, const Flame&, const Collidable&) {
                handleFlameCollision(player, flame);
            });
    }
}

void CollisionSystem::handlePlayerPortalsCollisions(const entt::entity player) const
{
    registry.view<Portal>().each([this, player](entt::entity portal, const Portal&) {
        handlePlayerPortalCollision(player, portal);
    });
}

void CollisionSystem::handleTilesFlamesCollisions() const
{
    registry.view<Flame, Collidable>().each([&](const entt::entity flame,
                                                const Flame&,
                                                const Collidable&) {
        registry.view<Tile, Collidable>().each([&](const entt::entity tile, Tile&, Collidable&) {
            handleTileFlameCollision(tile, flame);
        });
    });
}

void CollisionSystem::handleBlockingCollision(const entt::entity first,
                                              const entt::entity second) const
{
    const auto collisionInfo = checkCollision(first, second);
    auto& firstCollidable = registry.get<Collidable>(first);
    auto& secondCollidable = registry.get<Collidable>(second);

    if (collisionInfo && !shouldSkipCollision(first, secondCollidable))
    {
        auto& firstTransformable = registry.get<Transformable>(first);
        firstTransformable.position += collisionInfo->collisionRange;
        firstCollidable.direction = collisionInfo->direction;
    }
    else if (!collisionInfo && shouldSkipCollision(first, secondCollidable))
    {
        auto& skipEntities = secondCollidable.skipCollisionEntities;
        skipEntities.erase(std::remove(skipEntities.begin(), skipEntities.end(), first),
                           skipEntities.end());
    }
}

void CollisionSystem::handleFlameCollision(const entt::entity player,
                                           const entt::entity flame) const
{
    auto& playerComponent = registry.get<Player>(player);
    const auto collisionInfo = checkCollision(player, flame);
    if (collisionInfo && playerComponent.immortalTime <= 0)
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

void CollisionSystem::handlePowerUpCollision(const entt::entity player,
                                             const entt::entity powerUp) const
{
    const auto collisionInfo = checkCollision(player, powerUp);
    if (collisionInfo)
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

void CollisionSystem::handleTileFlameCollision(const entt::entity tile,
                                               const entt::entity flame) const
{
    const auto& tileTransformable = registry.get<Transformable>(tile);
    const auto tileIndex = calculateTileIndexForPosition(tileTransformable.position);

    const auto map = registry.view<Map>().front();
    auto& mapComponent = registry.get<Map>(map);

    if (mapComponent.tiles[tileIndex.x][tileIndex.y].tileType != TileType::ExplodableBlock)
    {
        return;
    }

    if (const auto collisionInfo = checkCollision(tile, flame); collisionInfo.has_value())
    {
        const auto tileIndexPosition = calculateTileIndexForPosition(tileTransformable.position);
        mapComponent.tiles[tileIndexPosition.x][tileIndexPosition.y].tileType = TileType::None;
        dispatcher.trigger<SpawnPowerUpEvent>(SpawnPowerUpEvent{tileTransformable.position});
        registry.destroy(tile);
    }
}

void CollisionSystem::handlePlayerPortalCollision(const entt::entity player,
                                                  const entt::entity portal) const
{
    auto& sourcePortalCollidable = registry.get<Collidable>(portal);
    const auto collisionInfo = checkCollision(player, portal);

    if (collisionInfo && !shouldSkipCollision(player, sourcePortalCollidable))
    {
        auto& playerTransformable = registry.get<Transformable>(player);

        registry.view<Portal, Transformable, Collidable>().each(
            [&](const entt::entity targetPortal,
                const Portal&,
                const Transformable& targetPortalTransformable,
                Collidable& targetPortalCollidable) {
                if (portal != targetPortal)
                {
                    const auto targetPortalIndex =
                        calculateTileIndexForPosition(targetPortalTransformable.position);
                    playerTransformable.position = calculatePositionInTileCenter(
                        targetPortalIndex, toVector2i(playerTransformable.size));
                    targetPortalCollidable.skipCollisionEntities.push_back(player);
                    return;
                }
            });
    }
    else if (!collisionInfo && shouldSkipCollision(player, sourcePortalCollidable))
    {
        auto& skipEntities = sourcePortalCollidable.skipCollisionEntities;
        skipEntities.erase(std::remove(skipEntities.begin(), skipEntities.end(), player),
                           skipEntities.end());
    }
}

bool CollisionSystem::shouldSkipCollision(const entt::entity entity,
                                          const Collidable& collidable) const
{
    auto& skipEntities = collidable.skipCollisionEntities;
    return std::find(skipEntities.begin(), skipEntities.end(), entity) != skipEntities.end();
}

std::optional<CollisionInfo> CollisionSystem::checkCollision(const entt::entity first,
                                                             const entt::entity second) const
{
    const auto& firstTransformable = registry.get<Transformable>(first);
    const auto& secondTransformable = registry.get<Transformable>(second);

    const auto w = 0.5f * (firstTransformable.size.x + secondTransformable.size.x);
    const auto h = 0.5f * (firstTransformable.size.y + secondTransformable.size.y);
    const auto dx = (firstTransformable.position.x + 0.5f * firstTransformable.size.x) -
                    (secondTransformable.position.x + 0.5f * secondTransformable.size.x);
    const auto dy = (firstTransformable.position.y + 0.5f * firstTransformable.size.y) -
                    (secondTransformable.position.y + 0.5f * secondTransformable.size.y);

    if (std::fabs(dx) < w && std::fabs(dy) < h)
    {
        const auto wy = w * dy;
        const auto hx = h * dx;

        if (wy > hx)
        {
            if (wy > -hx)
            {
                return CollisionInfo{{0, h - std::fabs(dy)}, Direction::Up};
            }
            else
                return CollisionInfo{{-(w - std::fabs(dx)), 0}, Direction::Right};
        }
        else
        {
            if (wy > -hx)
                return CollisionInfo{{w - std::fabs(dx), 0}, Direction::Left};
            else
                return CollisionInfo{{0, -(h - std::fabs(dy))}, Direction::Down};
        }
    }

    return std::nullopt;
}