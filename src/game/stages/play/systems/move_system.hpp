#pragma once

#include "util/types.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

class MoveSystem
{
  public:
    MoveSystem(entt::registry&, entt::dispatcher& dispatcher);
    void update(const TimeDelta);

  private:
    void moveBomberman(const entt::entity, const TimeDelta) const;
    void moveCreep(const entt::entity, const TimeDelta) const;
    void movePlayer(const entt::entity entity,
                    const TimeDelta dt,
                    const sf::Keyboard::Key downKey,
                    const sf::Keyboard::Key upKey,
                    const sf::Keyboard::Key rightKey,
                    const sf::Keyboard::Key leftKey) const;

    entt::registry& registry;
    entt::dispatcher& dispatcher;
};