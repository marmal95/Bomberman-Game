#include "AnimateSystem.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "SpawnTileEvent.hpp"
#include "GameFinishedEvent.hpp"

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

    handleFinishGameEvent(events, dt);
}

void AnimateSystem::configure(entityx::EventManager& events)
{
    events.subscribe<MoveChangeEvent>(*this);
    events.subscribe<FinishGameEvent>(*this);
}

void AnimateSystem::receive(const MoveChangeEvent& event)
{
    moveChangeEvents.push_back(event);
}

void AnimateSystem::receive(const FinishGameEvent& event)
{
    finishGameEvent = event;
    prepareFinishingAnimation();
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

void AnimateSystem::handleFinishGameEvent(entityx::EventManager& events, entityx::TimeDelta dt)
{
    if (finishGameEvent)
    {
        timeToSpawnFinishingGameTile -= dt;
        if (timeToSpawnFinishingGameTile < 0)
        {
            const auto tilePositionToSpawn = finishingGameTilesPositions.front();
            events.emit<SpawnTileEvent>(SpawnTileEvent{ { tilePositionToSpawn.y * 64.f, tilePositionToSpawn.x * 64.f } , TileType::FinishingGameAnimationBlock });

            finishingGameTilesPositions.pop();
            timeToSpawnFinishingGameTile = 1 / 20;
        }

        if (finishingGameTilesPositions.empty())
            notifyGameFinished(events);
    }
}

void AnimateSystem::notifyGameFinished(entityx::EventManager& events)
{
    finishGameEvent.reset();
    events.emit<GameFinishedEvent>();
}

void AnimateSystem::prepareFinishingAnimation()
{
    int i, k = 0, l = 0;
    int m = 21, n = 21;

    while (k < m && l < n)
    {
        for (i = l; i < n; ++i)
            finishingGameTilesPositions.push({ k, i });
        k++;

        for (i = k; i < m; ++i)
            finishingGameTilesPositions.push({ i, n - 1 });
        n--;

        if (k < m)
        {
            for (i = n - 1; i >= l; --i)
                finishingGameTilesPositions.push({ m - 1, i });
            m--;
        }

        if (l < n)
        {
            for (i = m - 1; i >= k; --i)
                finishingGameTilesPositions.push({ i, l });
            l++;
        }
    }
}