#include "game/stages/play/systems/move_system.hpp"
#include "game/stages/play/collision_detector.hpp"
#include "game/stages/play/components/collidable.hpp"
#include "game/stages/play/components/movable.hpp"
#include "game/stages/play/components/player.hpp"
#include "game/stages/play/components/transformable.hpp"
#include "game/stages/play/events/move_change_event.hpp"
#include "move_system.hpp"
#include <cmath>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

#ifdef GAME_DEBUG
#include "game/stages/play/components/debuggable.hpp"
#endif

namespace
{

MoveData getMoveData(const TimeDelta dt,
                     const Movable& movable,
                     const sf::Keyboard::Key downKey,
                     const sf::Keyboard::Key upKey,
                     const sf::Keyboard::Key rightKey,
                     const sf::Keyboard::Key leftKey)
{
    if (sf::Keyboard::isKeyPressed(downKey))
    {
        return {Direction::Down, sf::Vector2f(0, movable.velocity.y * static_cast<float>(dt))};
    }
    else if (sf::Keyboard::isKeyPressed(rightKey))
    {
        return {Direction::Right, sf::Vector2f(movable.velocity.x * static_cast<float>(dt), 0)};
    }
    else if (sf::Keyboard::isKeyPressed(upKey))
    {
        return {Direction::Up, sf::Vector2f(0, -movable.velocity.y * static_cast<float>(dt))};
    }
    else if (sf::Keyboard::isKeyPressed(leftKey))
    {
        return {Direction::Left, sf::Vector2f(-movable.velocity.x * static_cast<float>(dt), 0)};
    }
    return {Direction::None, sf::Vector2f{}};
}

Transformable createMovementBoundingBox(const Transformable& start, const Transformable& end)
{
    float left = std::min(start.position.x, end.position.x);
    float top = std::min(start.position.y, end.position.y);
    float right = std::max(start.position.x + start.size.x, end.position.x + start.size.x);
    float bottom = std::max(start.position.y + start.size.y, end.position.y + start.size.y);

    float width = right - left;
    float height = bottom - top;

    Transformable boundingBox{};
    boundingBox.size.x = width;
    boundingBox.size.y = height;
    boundingBox.position.x = left;
    boundingBox.position.y = top;
    return boundingBox;
}

void updatePossibleMove(const Transformable& playerTransformable,
                        const Transformable& otherTransformable,
                        MoveData& currentMoveData)
{
    auto& posChange = currentMoveData.positionChange;
    const auto& playerPos = playerTransformable.position;
    const auto& playerSize = playerTransformable.size;
    const auto& otherPos = otherTransformable.position;
    const auto& otherSize = otherTransformable.size;

    float deltaX = std::abs(otherPos.x + otherSize.x - playerPos.x);
    float deltaY = std::abs(otherPos.y + otherSize.y - playerPos.y);

    switch (currentMoveData.direction)
    {
    case Direction::Left:
        posChange = {-std::min(std::abs(posChange.x), deltaX), 0.f};
        break;
    case Direction::Right:
        posChange = {std::min(std::abs(posChange.x), std::abs(otherPos.x - (playerPos.x + playerSize.x))), 0.f};
        break;
    case Direction::Up:
        posChange = {0.f, -std::min(std::abs(posChange.y), deltaY)};
        break;
    case Direction::Down:
        posChange = {0.f, std::min(std::abs(posChange.y), std::abs(otherPos.y - (playerPos.y + playerSize.y)))};
        break;
    default:
        break;
    }
}

}

MoveSystem::MoveSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : registry{registry}, dispatcher{dispatcher}
{
}

void MoveSystem::update(const TimeDelta dt)
{
    registry.view<Bomberman, Movable>().each(
        [this, dt](const entt::entity entity, const Bomberman&, const Movable&) { moveBomberman(entity, dt); });

    registry.view<Creep, Movable>().each(
        [this, dt](const entt::entity entity, const Creep&, const Movable&) { moveCreep(entity, dt); });
}

void MoveSystem::moveBomberman(const entt::entity entity, const TimeDelta dt) const
{
    movePlayer(entity, dt, sf::Keyboard::Down, sf::Keyboard::Up, sf::Keyboard::Right, sf::Keyboard::Left);
}

void MoveSystem::moveCreep(const entt::entity entity, const TimeDelta dt) const
{
    movePlayer(entity, dt, sf::Keyboard::S, sf::Keyboard::W, sf::Keyboard::D, sf::Keyboard::A);
}

void MoveSystem::movePlayer(const entt::entity entity,
                            const TimeDelta dt,
                            const sf::Keyboard::Key downKey,
                            const sf::Keyboard::Key upKey,
                            const sf::Keyboard::Key rightKey,
                            const sf::Keyboard::Key leftKey) const
{
    auto& playerTransformable = registry.get<Transformable>(entity);
    auto& playerMovable = registry.get<Movable>(entity);

    auto currentMoveData = getMoveData(dt, playerMovable, downKey, upKey, rightKey, leftKey);
    const auto movementBoundingBox = createMovementBoundingBox(
        playerTransformable,
        Transformable{playerTransformable.size, playerTransformable.position + currentMoveData.positionChange});

#ifdef DEBUG_MODE
    const auto movementBoundingBoxEntity = registry.create();
    registry.emplace<Debuggable>(
        movementBoundingBoxEntity,
        Debuggable{movementBoundingBox.size, movementBoundingBox.position, {0, 255, 255, 128}});
#endif

    currentMoveData = restrictMoveToNearestCollision(entity, playerTransformable, movementBoundingBox, currentMoveData);
    handleMove(entity, currentMoveData, playerTransformable, playerMovable);
}

MoveData MoveSystem::restrictMoveToNearestCollision(const entt::entity player,
                                                    const Transformable& playerTransformable,
                                                    const Transformable& movementBoundingBox,
                                                    const MoveData& currentMoveData) const
{
    auto restrictedMove = currentMoveData;
    registry.view<Transformable, Collidable>().each(
        [&](const entt::entity other, const Transformable& otherTransformable, Collidable& collidable) {
            if (player != other && collidable.type == CollidableType::Blocking &&
                detectCollision(movementBoundingBox, otherTransformable))
            {
                updatePossibleMove(playerTransformable, otherTransformable, restrictedMove);
            }
        });
    return restrictedMove;
}

void MoveSystem::handleMove(const entt::entity entity,
                            const MoveData& currentMoveData,
                            Transformable& playerTransformable,
                            Movable& playerMovable) const
{
    playerTransformable.position += currentMoveData.positionChange;
    if (playerMovable.moveDirection != currentMoveData.direction)
    {
        playerMovable.moveDirection = currentMoveData.direction;
        dispatcher.trigger<MoveChangeEvent>(MoveChangeEvent{entity, currentMoveData.direction});
    }
}