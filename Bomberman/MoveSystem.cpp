#include "MoveSystem.hpp"
#include "Movable.hpp"
#include "Transformable.hpp"
#include <SFML/Window/Keyboard.hpp>

void MoveSystem::update(entityx::EntityManager& es, entityx::EventManager&, entityx::TimeDelta dt)
{
    es.each<Movable, Transformable>([&](entityx::Entity entity, Movable& movable, Transformable& transformable)
    {
        Direction prevDirection = movable.moveDirection;
        Direction actDirection = Direction::None;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			transformable.position += sf::Vector2f(0, movable.velocity.y * static_cast<float>(dt));
			actDirection = Direction::Down;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			transformable.position += sf::Vector2f(movable.velocity.x * static_cast<float>(dt), 0);
			actDirection = Direction::Right;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			transformable.position += sf::Vector2f(0, -movable.velocity.y * static_cast<float>(dt));
			actDirection = Direction::Up;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			transformable.position += sf::Vector2f(-movable.velocity.x * static_cast<float>(dt), 0);
			actDirection = Direction::Left;
		}
    });
}
