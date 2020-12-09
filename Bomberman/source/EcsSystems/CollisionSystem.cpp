#include "CollisionSystem.hpp"
#include "Transformable.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Bomb.hpp"
#include "Flame.hpp"
#include "Map.hpp"
#include "SpawnPowerUpEvent.hpp"
#include "PowerUp.hpp"
#include "Movable.hpp"
#include "Drawable.hpp"
#include "Utils.hpp"
#include "Portal.hpp"
#include <cmath>


void CollisionSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
    es.each<Player, Collidable>([&](entityx::Entity playerEntity, Player& player, Collidable& playerCollidable) {
        playerCollidable.direction = Direction::None;

        handlePlayerTilesCollisions(playerEntity, es);
        handlePlayerBombsCollisions(playerEntity, es);
        handlePlayerPowerUpsCollisions(playerEntity, es);
        handlePlayerFlamesCollisions(playerEntity, es, dt);
        handlePlayerPortalsCollisions(playerEntity, es);
        handleTilesFlamesCollisions(es, events);
    });
}

void CollisionSystem::handlePlayerTilesCollisions(entityx::Entity playerEntity, entityx::EntityManager& es) const
{
    es.each<Tile, Collidable>([&](entityx::Entity otherEntity, Tile&, Collidable&) {
        handleBlockingCollision(playerEntity, otherEntity);
    });
}

void CollisionSystem::handlePlayerBombsCollisions(entityx::Entity playerEntity, entityx::EntityManager& es) const
{
    es.each<Bomb, Collidable>([&](entityx::Entity otherEntity, Bomb&, Collidable&) {
        handleBlockingCollision(playerEntity, otherEntity);
    });
}

void CollisionSystem::handlePlayerPowerUpsCollisions(entityx::Entity playerEntity, entityx::EntityManager& es) const
{
    es.each<PowerUp, Collidable>([&](entityx::Entity otherEntity, PowerUp&, Collidable&) {
        handlePowerUpCollision(playerEntity, otherEntity);
    });
}

void CollisionSystem::handlePlayerFlamesCollisions(entityx::Entity playerEntity, entityx::EntityManager& es, const entityx::TimeDelta dt) const
{
    auto& player = *playerEntity.component<Player>();

    if (player.immortalTime > 0)
        player.immortalTime -= dt;
    else
    {
        es.each<Flame, Collidable>([&](entityx::Entity otherEntity, Flame&, Collidable&) {
            handleFlameCollision(playerEntity, otherEntity, dt);
        });
    }

}

void CollisionSystem::handlePlayerPortalsCollisions(entityx::Entity playerEntity, entityx::EntityManager& es) const
{
    es.each<Portal, Collidable>([&](entityx::Entity otherEntity, Portal&, Collidable&) {
        handlePlayerPortalCollision(playerEntity, otherEntity, es);
    });
}

void CollisionSystem::handleTilesFlamesCollisions(entityx::EntityManager& es, entityx::EventManager& events) const
{
    es.each<Flame, Collidable>([&](entityx::Entity flameEntity, Flame& flame, Collidable& playerCollidable) {
        es.each<Tile, Collidable, Transformable>([&](entityx::Entity tileEntity, Tile& tile, Collidable&, Transformable& tileTransformable) {
            handleTileFlameCollision(flameEntity, tileEntity, es, events);
        });
    });
}

void CollisionSystem::handleBlockingCollision(entityx::Entity playerEntity, entityx::Entity otherEntity) const
{
    const auto collisionInfo = checkCollision(playerEntity, otherEntity);
    auto& playerCollidable = *playerEntity.component<Collidable>();
    auto& otherCollidable = *otherEntity.component<Collidable>();

    if (collisionInfo && !shouldSkipCollision(playerEntity, otherCollidable))
    {
        auto& playerTransformable = *playerEntity.component<Transformable>();
        playerTransformable.position += collisionInfo->collisionRange;
        playerCollidable.direction = collisionInfo->direction;
    }
    else if (!collisionInfo && shouldSkipCollision(playerEntity, otherCollidable))
    {
        auto& skipEntities = otherCollidable.skipCollisionEntities;
        skipEntities.erase(std::remove(skipEntities.begin(), skipEntities.end(), playerEntity), skipEntities.end());
    }
}

void CollisionSystem::handleFlameCollision(entityx::Entity playerEntity, entityx::Entity otherEntity, const entityx::TimeDelta dt) const
{
    auto& player = *playerEntity.component<Player>();
    const auto collisionInfo = checkCollision(playerEntity, otherEntity);
    if (collisionInfo && player.immortalTime <= 0)
    {
        player.health--;
        player.immortalTime = IMMORTAL_TIME_AFTER_DEAD;

        if (player.health <= 0)
        {
            if (playerEntity.has_component<Movable>()) 
                playerEntity.remove<Movable>();
            if (playerEntity.has_component<Drawable>())
                playerEntity.remove<Drawable>();
        }
    }
}

void CollisionSystem::handlePowerUpCollision(entityx::Entity playerEntity, entityx::Entity powerUpEntity) const
{
    const auto collisionInfo = checkCollision(playerEntity, powerUpEntity);
    if (collisionInfo)
    {
        auto& player = *playerEntity.component<Player>();
        auto& playerMovable = *playerEntity.component<Movable>();
        const auto& powerUp = *powerUpEntity.component<PowerUp>();

        switch (powerUp.type)
        {
        case PowerUpType::Bomb:
            player.bombsNum++;
            break;
        case PowerUpType::Flame:
            player.bombsRange++;
            break;
        case PowerUpType::Speed:
            playerMovable.velocity *= 1.2f;
            break;
        default:
            break;
        }

        powerUpEntity.destroy();
    }
}

void CollisionSystem::handleTileFlameCollision(entityx::Entity flameEntity, entityx::Entity tileEntity, entityx::EntityManager& es, entityx::EventManager& events) const
{
    const auto& tileTransformable = *tileEntity.component<Transformable>();
    const auto tileIndex = calculateTileIndexForPosition(tileTransformable.position);

    auto& map = *(*es.entities_with_components<Map>().begin()).component<Map>();
    if (map.tiles[tileIndex.x][tileIndex.y].tileType != TileType::ExplodableBlock)
        return;

    if (auto collisionInfo = checkCollision(flameEntity, tileEntity); collisionInfo)
    {
        const auto tileIndexPosition = calculateTileIndexForPosition(tileTransformable.position);
        map.tiles[tileIndexPosition.x][tileIndexPosition.y].tileType = TileType::None;
        events.emit<SpawnPowerUpEvent>(SpawnPowerUpEvent{ tileTransformable.position });
        tileEntity.destroy();
    }
}

void CollisionSystem::handlePlayerPortalCollision(entityx::Entity playerEntity, entityx::Entity portalEntity, entityx::EntityManager& es) const
{   
    auto& playerCollidable = *playerEntity.component<Collidable>();
    auto& portalCollidable = *portalEntity.component<Collidable>();
    auto collisionInfo = checkCollision(playerEntity, portalEntity);

    if (collisionInfo && !shouldSkipCollision(playerEntity, portalCollidable))
    {
        auto& playerTransformable = *playerEntity.component<Transformable>();

        es.each<Portal, Transformable, Collidable>([&](entityx::Entity targetPortal, Portal&, Transformable& portalTransformable, Collidable& portalCollidable) {
            if (portalEntity != targetPortal)
            {
                const auto targetPortalIndex = calculateTileIndexForPosition(portalTransformable.position);
                playerTransformable.position = calculatePositionInTileCenter(targetPortalIndex, toVector2i(playerTransformable.size));
                portalCollidable.skipCollisionEntities.push_back(playerEntity);
                return;
            }
        });
    }
    else if (!collisionInfo && shouldSkipCollision(playerEntity, portalCollidable))
    {
        auto& skipEntities = portalCollidable.skipCollisionEntities;
        skipEntities.erase(std::remove(skipEntities.begin(), skipEntities.end(), playerEntity), skipEntities.end());
    }
}

bool CollisionSystem::shouldSkipCollision(const entityx::Entity& playerEntity, const Collidable& otherCollidable) const
{
    auto& skipEntities = otherCollidable.skipCollisionEntities;
    return std::find(skipEntities.begin(), skipEntities.end(), playerEntity) != skipEntities.end();
}

std::optional<CollisionInfo> CollisionSystem::checkCollision(entityx::Entity playerEntity, entityx::Entity otherEntity) const
{
    const auto& playerTransformable = *playerEntity.component<Transformable>();
    const auto& otherTransformable = *otherEntity.component<Transformable>();

    const auto w = 0.5f * (playerTransformable.size.x + otherTransformable.size.x);
    const auto h = 0.5f * (playerTransformable.size.y + otherTransformable.size.y);
    const auto dx = (playerTransformable.position.x + 0.5f * playerTransformable.size.x) - (otherTransformable.position.x + 0.5f * otherTransformable.size.x);
    const auto dy = (playerTransformable.position.y + 0.5f * playerTransformable.size.y) - (otherTransformable.position.y + 0.5f * otherTransformable.size.y);

    if (std::fabs(dx) < w && std::fabs(dy) < h)
    {
        const auto wy = w * dy;
        const auto hx = h * dx;

        if (wy > hx)
        {
            if (wy > -hx)
            {
                return CollisionInfo{ { 0, h - std::fabs(dy) }, Direction::Up };
            }
            else
                return CollisionInfo{ { -(w - std::fabs(dx)), 0 }, Direction::Right };
        }
        else
        {
            if (wy > -hx)
                return CollisionInfo{ { w - std::fabs(dx), 0 }, Direction::Left };
            else
                return CollisionInfo{ { 0, -(h - std::fabs(dy)) }, Direction::Down };
        }
    }

    return std::nullopt;
}
