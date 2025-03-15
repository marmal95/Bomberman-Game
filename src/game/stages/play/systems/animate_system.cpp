#include "game/stages/play/systems/animate_system.hpp"
#include "game/stages/play/components/animated.hpp"
#include "game/stages/play/components/drawable.hpp"
#include "game/stages/play/events/game_finished_event.hpp"
#include "game/stages/play/events/move_change_event.hpp"
#include "game/stages/play/events/spawn_tile_event.hpp"
#include "util/constants.hpp"
#include "util/tile_calculator.hpp"
#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

AnimateSystem::AnimateSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : registry{registry},
      dispatcher{dispatcher},
      finishingGameTilesPositions{},
      finishGameEvent{},
      finishAnimationDuration{2'000},
      finishAnimationSpawnTileInterval{finishAnimationDuration /
                                       (WIDTH_TILES_NUM * HEIGHT_TILES_NUM)}
{
    dispatcher.sink<MoveChangeEvent>().connect<&AnimateSystem::handleMoveChangeEvent>(this);
    dispatcher.sink<FinishGameEvent>().connect<&AnimateSystem::handleFinishGameEvent>(this);
}

void AnimateSystem::update(const TimeDelta dt)
{
    registry.view<Animated, Drawable>().each(
        [&](entt::entity, Animated& animated, Drawable& drawable) {
            if (!animated.paused)
                animated.frame += animated.speed;
            if (animated.frame + animated.speed >= animated.frames.size())
                animated.frame = 0;
            drawable.sprite.setTextureRect(animated.frames[static_cast<int>(animated.frame)]);
        });

    if (finishGameEvent)
    {
        auto dtMs = dt * 1'000;
        while (dtMs > 0 && !finishingGameTilesPositions.empty())
        {
            const auto tileIndexToSpawn = finishingGameTilesPositions.front();
            dispatcher.trigger<SpawnTileEvent>(
                SpawnTileEvent{calculatePositionForTileIndex(tileIndexToSpawn),
                               TileType::FinishingGameAnimationBlock});

            finishingGameTilesPositions.pop();

            dtMs -= finishAnimationSpawnTileInterval;
        }

        if (finishingGameTilesPositions.empty())
        {
            finishGameEvent.reset();
            dispatcher.trigger<GameFinishedEvent>();
        }
    }
}

void AnimateSystem::handleMoveChangeEvent(const MoveChangeEvent& event)
{
    auto& animated = registry.get<Animated>(event.entity);
    animated.paused = (event.direction == Direction::None) ? true : false;
}

void AnimateSystem::handleFinishGameEvent(const FinishGameEvent& event)
{
    finishGameEvent = event;
    prepareFinishingAnimation();
}

void AnimateSystem::prepareFinishingAnimation()
{
    int startingRowIndex = 0, startingColumnIndex = 0;
    int endingRowIndex = HEIGHT_TILES_NUM, endingColumnIndex = WIDTH_TILES_NUM;

    while (startingRowIndex < endingRowIndex && startingColumnIndex < endingColumnIndex)
    {
        for (int columnIndex = startingColumnIndex; columnIndex < endingColumnIndex; ++columnIndex)
            finishingGameTilesPositions.push({startingRowIndex, columnIndex});
        startingRowIndex++;

        for (int rowIndex = startingRowIndex; rowIndex < endingRowIndex; ++rowIndex)
            finishingGameTilesPositions.push({rowIndex, endingColumnIndex - 1});
        endingColumnIndex--;

        if (startingRowIndex < endingRowIndex)
        {
            for (int columnIndex = endingColumnIndex - 1; columnIndex >= startingColumnIndex;
                 --columnIndex)
                finishingGameTilesPositions.push({endingRowIndex - 1, columnIndex});
            endingRowIndex--;
        }

        if (startingColumnIndex < endingColumnIndex)
        {
            for (int rowIndex = endingRowIndex - 1; rowIndex >= startingRowIndex; --rowIndex)
                finishingGameTilesPositions.push({rowIndex, startingColumnIndex});
            startingColumnIndex++;
        }
    }
}
