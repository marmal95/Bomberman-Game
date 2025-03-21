#include "game/stages/play/systems/render_system.hpp"
#include "game/stages/play/components/collidable.hpp"
#include "game/stages/play/components/debuggable.hpp"
#include "game/stages/play/components/drawable.hpp"
#include "game/stages/play/components/player.hpp"
#include "game/stages/play/components/transformable.hpp"
#include "game/stages/play/components/z_index.hpp"
#include "game/stages/play/events/move_change_event.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <ranges>
#include <vector>

#define GAME_DEBUG true

RenderSystem::RenderSystem(entt::registry& registry,
                           entt::dispatcher& dispatcher,
                           const ResourceHolder<sf::Texture, ResourceID>& textures)
    : registry{registry}, textures{textures}
{
    dispatcher.sink<MoveChangeEvent>().connect<&RenderSystem::handleEvent>(this);
}

void RenderSystem::update(sf::RenderTarget& target)
{
    auto withZIndex = registry.view<Drawable, Transformable, ZIndex>();

    std::vector<entt::entity> drawQueue;
    drawQueue.assign(withZIndex.begin(), withZIndex.end());

    std::ranges::sort(drawQueue, [this](entt::entity a, entt::entity b) {
        const auto aZIndex = registry.get<ZIndex>(a);
        const auto bZIndex = registry.get<ZIndex>(b);

        if (aZIndex.value != bZIndex.value)
            return aZIndex.value < bZIndex.value;

        const auto aTransformable = registry.get<Transformable>(a);
        const auto bTransformable = registry.get<Transformable>(b);
        return aTransformable.position.y < bTransformable.position.y;
    });

    for (auto entity : drawQueue)
    {
        auto& drawable = registry.get<Drawable>(entity);
        auto& transform = registry.get<Transformable>(entity);

        if (const auto player = registry.try_get<Player>(entity); player != nullptr)
        {
            changeOpacity(drawable, *player);
        }
        drawEntity(target, entity, drawable, transform);
    }

#ifdef GAME_DEBUG
    registry.view<Debuggable>().each([&](entt::entity e, const Debuggable& d) {
        sf::RectangleShape shape{};
        shape.setPosition(d.position);
        shape.setOrigin({});
        shape.setFillColor(d.color);
        shape.setSize(d.size);
        target.draw(shape);
        registry.destroy(e);
    });
#endif
}

void RenderSystem::handleEvent(const MoveChangeEvent& event)
{
    if (registry.try_get<Bomberman>(event.entity))
        handleBombermanMoveChangeEvent(event);
    if (registry.try_get<Creep>(event.entity))
        handleCreepMoveChangeEvent(event);
}

void RenderSystem::handleBombermanMoveChangeEvent(const MoveChangeEvent& event)
{
    handlePlayerMove(event,
                     ResourceID::BombermanLeft,
                     ResourceID::BombermanRight,
                     ResourceID::BombermanBack,
                     ResourceID::BombermanFront);
}

void RenderSystem::handleCreepMoveChangeEvent(const MoveChangeEvent& event)
{
    handlePlayerMove(
        event, ResourceID::CreepLeft, ResourceID::CreepRight, ResourceID::CreepBack, ResourceID::CreepFront);
}

void RenderSystem::handlePlayerMove(const MoveChangeEvent& event,
                                    const ResourceID leftResourceId,
                                    const ResourceID rightResourceId,
                                    const ResourceID backResourceId,
                                    const ResourceID frontResourceId)
{
    auto& drawable = registry.get<Drawable>(event.entity);
    switch (event.direction)
    {
    case Direction::Left:
        drawable.sprite.setTexture(textures.getResource(leftResourceId));
        break;
    case Direction::Right:
        drawable.sprite.setTexture(textures.getResource(rightResourceId));
        break;
    case Direction::Up:
        drawable.sprite.setTexture(textures.getResource(backResourceId));
        break;
    case Direction::Down:
        drawable.sprite.setTexture(textures.getResource(frontResourceId));
        break;
    default:
        break;
    }
}

void RenderSystem::drawEntity(sf::RenderTarget& target,
                              const entt::entity,
                              Drawable& drawable,
                              const Transformable& transformable) const
{
    drawable.sprite.setPosition(
        transformable.position.x - (drawable.sprite.getGlobalBounds().width - transformable.size.x) / 2,
        transformable.position.y - (drawable.sprite.getGlobalBounds().height - transformable.size.y));
    target.draw(drawable.sprite);
}

void RenderSystem::changeOpacity(Drawable& drawable, const Player& player) const
{
    if (player.immortalTime > 0)
        drawable.sprite.setColor(sf::Color(255, 255, 255, 128));
    else
        drawable.sprite.setColor(sf::Color(255, 255, 255, 255));
}