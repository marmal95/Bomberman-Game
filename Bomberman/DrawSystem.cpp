#include "DrawSystem.hpp"
#include "Drawable.hpp"
#include "Transformable.hpp"
#include "ResourceManager.hpp"

void DrawSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
	auto& window = ResourcesManager::getInstance().window;

	es.each<Transformable, Drawable>([&](entityx::Entity entity, Transformable& transformable, Drawable& drawable)
	{
		drawable.sprite.setPosition(transformable.position);
		window.draw(drawable.sprite);
	});
}
