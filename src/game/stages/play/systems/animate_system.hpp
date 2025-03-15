#pragma once

#include "game/stages/play/events/finish_game_event.hpp"
#include "util/types.hpp"
#include <SFML/System/Vector2.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <optional>
#include <queue>
#include <vector>

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
    std::queue<sf::Vector2i> finishingGameTilesPositions;
    std::optional<FinishGameEvent> finishGameEvent;

    const int finishAnimationDuration;
    const int finishAnimationSpawnTileInterval;
};