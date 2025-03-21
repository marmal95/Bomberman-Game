#pragma once

#include "enums/direction.hpp"
#include "util/types.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

struct Transformable;
struct Movable;

struct MoveData
{
    Direction direction{Direction::None};
    sf::Vector2f positionChange{};
};

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

    MoveData restrictMoveToNearestCollision(const entt::entity player,
                                            const Transformable& playerTransformable,
                                            const Transformable& movementBoundingBox,
                                            const MoveData& currentMoveData) const;

    void handleMove(const entt::entity entity,
                    const MoveData& currentMoveData,
                    Transformable& playerTransformable,
                    Movable& playerMovable) const;

    entt::registry& registry;
    entt::dispatcher& dispatcher;
};