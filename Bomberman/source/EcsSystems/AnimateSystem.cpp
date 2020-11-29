#include "AnimateSystem.hpp"
#include "Animated.hpp"
#include "Drawable.hpp"
#include "SpawnTileEvent.hpp"
#include "GameFinishedEvent.hpp"
#include "Constants.hpp"
#include "Utils.hpp"

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
            const auto tileIndexToSpawn = finishingGameTilesPositions.front();
            events.emit<SpawnTileEvent>(SpawnTileEvent{ calculatePositionForTileIndex(tileIndexToSpawn) , TileType::FinishingGameAnimationBlock });

            finishingGameTilesPositions.pop();
            timeToSpawnFinishingGameTile = FINAL_ANIM_STEP_TIME;
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
    int startingRowIndex = 0, startingColumnIndex = 0;
    int endingRowIndex = HEIGHT_TILES_NUM, endingColumnIndex = WIDTH_TILES_NUM;

    while (startingRowIndex < endingRowIndex && startingColumnIndex < endingColumnIndex)
    {
        for (int columnIndex = startingColumnIndex; columnIndex < endingColumnIndex; ++columnIndex)
            finishingGameTilesPositions.push({ startingRowIndex, columnIndex });
        startingRowIndex++;

        for (int rowIndex = startingRowIndex; rowIndex < endingRowIndex; ++rowIndex)
            finishingGameTilesPositions.push({ rowIndex, endingColumnIndex - 1 });
        endingColumnIndex--;

        if (startingRowIndex < endingRowIndex)
        {
            for (int columnIndex = endingColumnIndex - 1; columnIndex >= startingColumnIndex; --columnIndex)
                finishingGameTilesPositions.push({ endingRowIndex - 1, columnIndex });
            endingRowIndex--;
        }

        if (startingColumnIndex < endingColumnIndex)
        {
            for (int rowIndex = endingRowIndex - 1; rowIndex >= startingRowIndex; --rowIndex)
                finishingGameTilesPositions.push({ rowIndex, startingColumnIndex });
            startingColumnIndex++;
        }
    }
}