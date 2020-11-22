#include "DrawSystem.hpp"
#include "Drawable.hpp"
#include "Transformable.hpp"
#include "ResourceManager.hpp"
#include "MoveChangeEvent.hpp"

DrawSystem::DrawSystem(const ResourceHolder<sf::Texture, ResourceID>& textures)
	: textures{ textures }
{}

void DrawSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
	handleMoveChangeEvents();

	auto& window = ResourcesManager::getInstance().window;
	es.each<Transformable, Drawable>([&](entityx::Entity entity, Transformable& transformable, Drawable& drawable)
	{
		drawable.sprite.setPosition(transformable.position);
		window.draw(drawable.sprite);
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
