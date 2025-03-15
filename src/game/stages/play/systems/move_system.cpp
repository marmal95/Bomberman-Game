#include "game/stages/play/systems/move_system.hpp"
#include "game/stages/play/components/collidable.hpp"
#include "game/stages/play/components/drawable.hpp"
#include "game/stages/play/components/movable.hpp"
#include "game/stages/play/components/player.hpp"
#include "game/stages/play/components/transformable.hpp"
#include "game/stages/play/events/move_change_event.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

MoveSystem::MoveSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : registry{registry}, dispatcher{dispatcher}
{
}

void MoveSystem::update(const TimeDelta dt)
{
    registry.view<Bomberman, Movable>().each(
        [this, dt](const entt::entity entity, const Bomberman&, const Movable&) {
            moveBomberman(entity, dt);
        });

    registry.view<Creep, Movable>().each([this, dt](const entt::entity entity,
                                                    const Creep&,
                                                    const Movable&) { moveCreep(entity, dt); });
}

void MoveSystem::moveBomberman(const entt::entity entity, const TimeDelta dt) const
{
    movePlayer(
        entity, dt, sf::Keyboard::Down, sf::Keyboard::Up, sf::Keyboard::Right, sf::Keyboard::Left);
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
    auto& transformable = registry.get<Transformable>(entity);
    auto& collidable = registry.get<Collidable>(entity);
    auto& movable = registry.get<Movable>(entity);

    Direction prevDirection = movable.moveDirection;
    Direction actDirection = Direction::None;

    if (sf::Keyboard::isKeyPressed(downKey) && collidable.direction != Direction::Down)
    {
        transformable.position += sf::Vector2f(0, movable.velocity.y * static_cast<float>(dt));
        actDirection = Direction::Down;
    }
    else if (sf::Keyboard::isKeyPressed(rightKey) && collidable.direction != Direction::Right)
    {
        transformable.position += sf::Vector2f(movable.velocity.x * static_cast<float>(dt), 0);
        actDirection = Direction::Right;
    }
    else if (sf::Keyboard::isKeyPressed(upKey) && collidable.direction != Direction::Up)
    {
        transformable.position += sf::Vector2f(0, -movable.velocity.y * static_cast<float>(dt));
        actDirection = Direction::Up;
    }
    else if (sf::Keyboard::isKeyPressed(leftKey) && collidable.direction != Direction::Left)
    {
        transformable.position += sf::Vector2f(-movable.velocity.x * static_cast<float>(dt), 0);
        actDirection = Direction::Left;
    }

    if (prevDirection != actDirection)
    {
        movable.moveDirection = actDirection;
        dispatcher.trigger<MoveChangeEvent>(MoveChangeEvent{entity, actDirection});
    }
}