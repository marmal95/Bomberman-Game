#include "DrawSystem.hpp"
#include "Drawable.hpp"
#include "Transformable.hpp"
#include "ResourceManager.hpp"
#include "MoveChangeEvent.hpp"
#include "Player.hpp"
#include "TopLevel.hpp"

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
        drawEntity(entity, drawable, transformable, window);
    });

    drawPlayers(es);

    es.each<TopLevel, Transformable, Drawable>([&](entityx::Entity entity, TopLevel&, Transformable& transformable, Drawable& drawable)
    {
        drawEntity(entity, drawable, transformable, window);
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
        if (event.entity.has_component<Bomberman>())
            handleBombermanMoveChangeEvent(event);
        if (event.entity.has_component<Creep>())
            handleCreepMoveChangeEvent(event);
    }
    moveChangeEvents.clear();
}

void DrawSystem::handleBombermanMoveChangeEvent(MoveChangeEvent& event)
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

void DrawSystem::handleCreepMoveChangeEvent(MoveChangeEvent& event)
{
    auto drawable = event.entity.component<Drawable>();
    switch (event.direction)
    {
    case Direction::Left: drawable->sprite.setTexture(textures.getResource(ResourceID::CreepLeft)); break;
    case Direction::Right: drawable->sprite.setTexture(textures.getResource(ResourceID::CreepRight)); break;
    case Direction::Up: drawable->sprite.setTexture(textures.getResource(ResourceID::CreepBack)); break;
    case Direction::Down: drawable->sprite.setTexture(textures.getResource(ResourceID::CreepFront)); break;
    }
}

void DrawSystem::drawPlayers(entityx::EntityManager& es) const
{
    auto& window = ResourcesManager::getInstance().window;

    auto bomberman = (*es.entities_with_components<Bomberman>().begin());
    auto& bombermanPlayer = *bomberman.component<Player>();
    auto& bombermanTransformable = *bomberman.component<Transformable>();
    auto& bombermanDrawable = *bomberman.component<Drawable>();

    auto creep = (*es.entities_with_components<Creep>().begin());
    auto& creepPlayer = *creep.component<Player>();
    auto& creepTransformable = *creep.component<Transformable>();
    auto& creepDrawable = *creep.component<Drawable>();

    changeOpacity(bombermanDrawable, bombermanPlayer);
    changeOpacity(creepDrawable, creepPlayer);

    if (bombermanTransformable.position.y < creepTransformable.position.y)
    {
        drawEntity(bomberman, bombermanDrawable, bombermanTransformable, window);
        drawEntity(creep, creepDrawable, creepTransformable, window);
    }
    else
    {
        drawEntity(creep, creepDrawable, creepTransformable, window);
        drawEntity(bomberman, bombermanDrawable, bombermanTransformable, window);
    }
}

void DrawSystem::drawEntity(entityx::Entity entity, Drawable& drawable, Transformable& transformable, sf::RenderWindow& window) const
{
#ifdef _DEBUG
    if (entity.has_component<Collidable>())
    {
        sf::RectangleShape shape{};
        shape.setPosition(transformable.position);
        shape.setOrigin(drawable.sprite.getOrigin());
        shape.setFillColor({ 255, 0, 0, 128 });
        shape.setSize(transformable.size);
        window.draw(shape);
    }
#endif
    drawable.sprite.setPosition(transformable.position.x - (drawable.sprite.getGlobalBounds().width - transformable.size.x) / 2,
        transformable.position.y - (drawable.sprite.getGlobalBounds().height - transformable.size.y));
    window.draw(drawable.sprite);
}

void DrawSystem::changeOpacity(Drawable& drawable, const Player& player) const
{
    if (player.immortalTime > 0)
        drawable.sprite.setColor(sf::Color(255, 255, 255, 128));
    else
        drawable.sprite.setColor(sf::Color(255, 255, 255, 255));
}
