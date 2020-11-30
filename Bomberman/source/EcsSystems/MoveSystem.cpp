#include "MoveSystem.hpp"
#include "Movable.hpp"
#include "Transformable.hpp"
#include "Collidable.hpp"
#include "MoveChangeEvent.hpp"
#include "Player.hpp"
#include <SFML/Window/Keyboard.hpp>


void MoveSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
    es.each<Bomberman, Transformable, Movable>([&](entityx::Entity entity, Bomberman&, Transformable&, Movable&)
    {
        moveBomberman(entity, events, dt);
    });

    es.each<Creep, Transformable, Movable>([&](entityx::Entity entity, Creep&, Transformable&, Movable&)
    {
        moveCreep(entity, events, dt);
    });
}

void MoveSystem::moveBomberman(entityx::Entity entity, entityx::EventManager& events, entityx::TimeDelta dt) const
{
    auto& transformable = *entity.component<Transformable>();
    auto& collidable = *entity.component<Collidable>();
    auto& movable = *entity.component<Movable>();

    Direction prevDirection = movable.moveDirection;
    Direction actDirection = Direction::None;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && collidable.direction != Direction::Down)
    {
        transformable.position += sf::Vector2f(0, movable.velocity.y * static_cast<float>(dt));
        actDirection = Direction::Down;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && collidable.direction != Direction::Right)
    {
        transformable.position += sf::Vector2f(movable.velocity.x * static_cast<float>(dt), 0);
        actDirection = Direction::Right;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && collidable.direction != Direction::Up)
    {
        transformable.position += sf::Vector2f(0, -movable.velocity.y * static_cast<float>(dt));
        actDirection = Direction::Up;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && collidable.direction != Direction::Left)
    {
        transformable.position += sf::Vector2f(-movable.velocity.x * static_cast<float>(dt), 0);
        actDirection = Direction::Left;
    }

    if (prevDirection != actDirection)
    {
        movable.moveDirection = actDirection;
        events.emit<MoveChangeEvent>({ entity, actDirection });
    }
}

void MoveSystem::moveCreep(entityx::Entity entity, entityx::EventManager& events, entityx::TimeDelta dt) const
{
    auto& transformable = *entity.component<Transformable>();
    auto& collidable = *entity.component<Collidable>();
    auto& movable = *entity.component<Movable>();

    Direction prevDirection = movable.moveDirection;
    Direction actDirection = Direction::None;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && collidable.direction != Direction::Down)
    {
        transformable.position += sf::Vector2f(0, movable.velocity.y * static_cast<float>(dt));
        actDirection = Direction::Down;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && collidable.direction != Direction::Right)
    {
        transformable.position += sf::Vector2f(movable.velocity.x * static_cast<float>(dt), 0);
        actDirection = Direction::Right;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && collidable.direction != Direction::Up)
    {
        transformable.position += sf::Vector2f(0, -movable.velocity.y * static_cast<float>(dt));
        actDirection = Direction::Up;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && collidable.direction != Direction::Left)
    {
        transformable.position += sf::Vector2f(-movable.velocity.x * static_cast<float>(dt), 0);
        actDirection = Direction::Left;
    }

    if (prevDirection != actDirection)
    {
        movable.moveDirection = actDirection;
        events.emit<MoveChangeEvent>({ entity, actDirection });
    }
}
