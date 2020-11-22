#include "CollisionSystem.hpp"
#include "Collidable.hpp"
#include "Transformable.hpp"
#include "Player.hpp"

void CollisionSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta)
{
    handleMoveChangeEvents();

    es.each<Player, Collidable>([&](entityx::Entity playerEntity, Player&, Collidable& playerCollidable) {
        es.each<Collidable>([&](entityx::Entity otherEntity, Collidable& otherCollidable) {
            if (playerEntity != otherEntity)
                checkCollision(events, playerEntity, otherEntity);
        });
    });
}

void CollisionSystem::configure(entityx::EventManager& events)
{
    events.subscribe<MoveChangeEvent>(*this);
}

void CollisionSystem::receive(const MoveChangeEvent& event)
{
    moveChangeEvents.push_back(event);
}

void CollisionSystem::handleMoveChangeEvents()
{
    for (auto& event : moveChangeEvents)
    {
        if (event.entity.has_component<Collidable>())
        {
            auto collidable = event.entity.component<Collidable>();
            *collidable = Collidable{};
        }
    }
    moveChangeEvents.clear();
}

void CollisionSystem::checkCollision(entityx::EventManager& events, entityx::Entity playerEntity, entityx::Entity otherEntity) const
{
    auto& playerTransformable = *playerEntity.component<Transformable>();
    auto& otherTransformable = *otherEntity.component<Transformable>();
    auto& playerCollidable = *playerEntity.component<Collidable>();

    const auto w = 0.5f * (playerTransformable.size.x + otherTransformable.size.x);
    const auto h = 0.5f * (playerTransformable.size.y + otherTransformable.size.y);
    const auto dx = (playerTransformable.position.x + 0.5f * playerTransformable.size.x) - (otherTransformable.position.x + 0.5f * otherTransformable.size.x);
    const auto dy = (playerTransformable.position.y + 0.5f * playerTransformable.size.y) - (otherTransformable.position.y + 0.5f * otherTransformable.size.y);

    if (abs(dx) <= w && abs(dy) <= h)
    {
        float wy = w * dy;
        float hx = h * dx;

        if (wy > hx)
        {
            if (wy > -hx)
            {
                playerCollidable.collidedUp = true;
                playerTransformable.position.y += h - abs(dy);
            }
            else
            {
                playerCollidable.collidedRight = true;
                playerTransformable.position.x -= w - abs(dx);
            }
        }
        else
        {
            if (wy > -hx)
            {
                playerCollidable.collidedLeft = true;
                playerTransformable.position.x += w - abs(dx);
            }
            else
            {
                playerCollidable.collidedDown = true;
                playerTransformable.position.y -= h - abs(dy);
            }
        }
    }
}