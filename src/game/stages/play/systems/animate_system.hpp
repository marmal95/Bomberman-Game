#pragma once

#include "game/stages/play/events/finish_game_event.hpp"
#include "util/types.hpp"
#include <SFML/System/Vector2.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <optional>
#include <queue>

struct MoveChangeEvent;

class AnimateSystem
{
  public:
    AnimateSystem(entt::registry&, entt::dispatcher& dispatcher);
    void update(const TimeDelta);

  private:
    void handleMoveChangeEvent(const MoveChangeEvent&);
    void handleFinishGameEvent(const FinishGameEvent&);
    void prepareFinishingAnimation();

    entt::registry& registry;
    entt::dispatcher& dispatcher;
    std::queue<TileIndex> finishingGameTilesPositions;
    std::optional<FinishGameEvent> finishGameEvent;

    const float finishAnimationDuration;
    const float finishAnimationSpawnTileInterval;
};