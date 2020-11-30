#include "DrawSystem.hpp"
#include "Drawable.hpp"
#include "Transformable.hpp"
#include "ResourceManager.hpp"
#include "MoveChangeEvent.hpp"
#include "Player.hpp"
#include "TopLevel.hpp"
#include <queue>
#include <vector>

#ifdef _DEBUG
#include "Collidable.hpp"
#endif

DrawSystem::DrawSystem(const ResourceHolder<sf::Texture, ResourceID>& textures)
    : textures{ textures }
{}

void DrawSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
    handleMoveChangeEvents();

    es.each<Transformable, Drawable>([&](entityx::Entity entity, Transformable& transformable, Drawable& drawable)
    {
        drawEntity(entity, drawable, transformable);
    });

    drawPlayers(es);

    es.each<TopLevel, Transformable, Drawable>([&](entityx::Entity entity, TopLevel&, Transformable& transformable, Drawable& drawable)
    {
        drawEntity(entity, drawable, transformable);
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
    const auto entityPositionComparator = [](const auto left, const auto right) {
        const auto& transformable1 = *left.component<const Transformable>();
        const auto& transformable2 = *right.component<const Transformable>();
        return transformable1.position.y > transformable2.position.y;
    };

    using VerticalPositionPlayersQueue = std::priority_queue<entityx::Entity, std::vector<entityx::Entity>, decltype(entityPositionComparator)>;
    VerticalPositionPlayersQueue playersQueue{ entityPositionComparator };

    es.entities_with_components<Player, Drawable, Transformable>()
        .each([&](entityx::Entity entity, Player&, Drawable&, Transformable&)
    {
        playersQueue.push(entity);
    });

    while (!playersQueue.empty())
    {
        auto entity = playersQueue.top();
        auto& drawable = *entity.component<Drawable>();
        changeOpacity(drawable, *entity.component<Player>());
        drawEntity(entity, drawable, *entity.component<Transformable>());
        playersQueue.pop();
    }
}

void DrawSystem::drawEntity(entityx::Entity entity, Drawable& drawable, Transformable& transformable) const
{
    auto& window = ResourcesManager::getInstance().window;

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