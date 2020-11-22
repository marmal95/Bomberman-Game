#pragma once

#include <entityx/entityx.h>

class MoveSystem : public entityx::System<MoveSystem>
{
public:
    void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;
};

