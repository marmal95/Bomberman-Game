#pragma once

#include "MoveChangeEvent.hpp"
#include <entityx/entityx.h>
#include <vector>

class AnimateSystem : public entityx::System<AnimateSystem>, public entityx::Receiver<AnimateSystem>
{
public:
    void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;
    void configure(entityx::EventManager&) override;
    void receive(const MoveChangeEvent&);

private:
    void handleMoveChangeEvents();
    std::vector<MoveChangeEvent> moveChangeEvents;
};

