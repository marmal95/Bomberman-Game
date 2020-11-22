#include "DrawSystem.hpp"
#include "Drawable.hpp"
#include "Transformable.hpp"
#include "ResourceManager.hpp"
#include "MoveChangeEvent.hpp"
#include "Player.hpp"

#ifdef _DEBUG
#include "Collidable.hpp"
#endif

DrawSystem::DrawSystem(const ResourceHolder<sf::Texture, ResourceID>& textures)
	: textures{ textures }
{}

void DrawSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
	handleMoveChangeEvents();

	auto& window = ResourcesManager::getInstance().window;
	es.each<Transformable, Drawable>([&](entityx::Entity entity, Transformable& transformable, Drawable& drawable)
	{
		drawEntity(drawable, transformable, window);

#ifdef _DEBUG
		if (entity.has_component<Collidable>())
		{
			sf::RectangleShape shape{};
			shape.setPosition(transformable.position);
			shape.setFillColor({ 255, 0, 0, 128 });
			shape.setSize(transformable.size);
			window.draw(shape);
		}
#endif
	});

	es.each<Player, Transformable, Drawable>([&](entityx::Entity entity, Player& player, Transformable& transformable, Drawable& drawable)
	{
		drawEntity(drawable, transformable, window);
#ifdef _DEBUG
		if (entity.has_component<Collidable>())
		{
			sf::RectangleShape shape{};
			shape.setPosition(transformable.position);
			shape.setFillColor({ 255, 0, 0, 128 });
			shape.setSize(transformable.size);
			window.draw(shape);
		}
#endif
	});
}

void DrawSystem::configure(entityx::EventManager& events)
{
	events.subscribe<MoveChangeEvent>(*this);
}

void DrawSystem::receive(const MoveChangeEvent& event)
{
	moveChangeEvents.push_back(event);
}

void DrawSystem::handleMoveChangeEvents()
{
	for (auto& event : moveChangeEvents)
	{
		auto drawable = event.entity.component<Drawable>();
		switch (event.direction)
		{
		case Direction::Left: drawable->sprite.setTexture(textures.getResource(ResourceID::BombermanLeft)); break;
		case Direction::Right: drawable->sprite.setTexture(textures.getResource(ResourceID::BombermanRight)); break;
		case Direction::Up: drawable->sprite.setTexture(textures.getResource(ResourceID::BombermanBack)); break;
		case Direction::Down: drawable->sprite.setTexture(textures.getResource(ResourceID::BombermanFront)); break;
		}
	}
	moveChangeEvents.clear();
}

void DrawSystem::drawEntity(Drawable& drawable, Transformable& transformable, sf::RenderWindow& window)
{
	drawable.sprite.setPosition(transformable.position);
	drawable.sprite.setPosition(transformable.position.x - (drawable.sprite.getGlobalBounds().width - transformable.size.x) / 2,
		transformable.position.y - (drawable.sprite.getGlobalBounds().height - transformable.size.y));
	window.draw(drawable.sprite);
}
