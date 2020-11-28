#include "AnimateSystem.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"

void AnimateSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
{
    handleMoveChangeEvents();

    es.each<Animated, Drawable>([&](entityx::Entity entity, Animated& animated, Drawable& drawable) {
        if (!animated.paused)
            animated.frame += animated.speed;
        if (animated.frame + animated.speed >= animated.frames.size())
            animated.frame = 0;
        drawable.sprite.setTextureRect(animated.frames[static_cast<int>(animated.frame)]);
    });
}

void AnimateSystem::configure(entityx::EventManager& events)
{
    events.subscribe<MoveChangeEvent>(*this);
}

void AnimateSystem::receive(const MoveChangeEvent& event)
{
    moveChangeEvents.push_back(event);
}

void AnimateSystem::handleMoveChangeEvents()
{
    for (auto& event : moveChangeEvents)
    {
        auto animated = event.entity.component<Animated>();
        animated->paused = (event.direction == Direction::None) ? true : false;
    }
    moveChangeEvents.clear();
}
