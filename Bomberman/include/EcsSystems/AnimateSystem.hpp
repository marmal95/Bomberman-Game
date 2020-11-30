#pragma once

#include "MoveChangeEvent.hpp"
#include "FinishGameEvent.hpp"
#include <entityx/entityx.h>
#include <vector>
#include <optional>
#include <queue>

struct FinishGameEvent;

class AnimateSystem : public entityx::System<AnimateSystem>, public entityx::Receiver<AnimateSystem>
{
public:
    void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;
    void configure(entityx::EventManager&) override;
    void receive(const MoveChangeEvent&);
    void receive(const FinishGameEvent&);

private:
    void handleMoveChangeEvents();
    void handleFinishGameEvent(entityx::EventManager&, entityx::TimeDelta);
    void notifyGameFinished(entityx::EventManager&);
    void prepareFinishingAnimation();

    std::vector<MoveChangeEvent> moveChangeEvents;
    std::queue<sf::Vector2i> finishingGameTilesPositions;
    std::optional<FinishGameEvent> finishGameEvent;
};

