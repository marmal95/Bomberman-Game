#include "MoveSystem.hpp"
#include "Movable.hpp"
#include "Transformable.hpp"
#include "Collidable.hpp"
#include "MoveChangeEvent.hpp"
#include <SFML/Window/Keyboard.hpp>


void MoveSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
    es.each<Movable, Transformable, Collidable>([&](entityx::Entity entity, Movable& movable, Transformable& transformable, Collidable& collidable)
    {
        Direction prevDirection = movable.moveDirection;
        Direction actDirection = Direction::None;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && !collidable.collidedDown)
        {
            transformable.position += sf::Vector2f(0, movable.velocity.y * static_cast<float>(dt));
            actDirection = Direction::Down;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && !collidable.collidedRight)
        {
            transformable.position += sf::Vector2f(movable.velocity.x * static_cast<float>(dt), 0);
            actDirection = Direction::Right;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && !collidable.collidedUp)
        {
            transformable.position += sf::Vector2f(0, -movable.velocity.y * static_cast<float>(dt));
            actDirection = Direction::Up;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && !collidable.collidedLeft)
        {
            transformable.position += sf::Vector2f(-movable.velocity.x * static_cast<float>(dt), 0);
            actDirection = Direction::Left;
        }

        if (prevDirection != actDirection)
        {
            movable.moveDirection = actDirection;
            events.emit<MoveChangeEvent>({ entity, actDirection });
        }
    });
}
