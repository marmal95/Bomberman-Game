#include "game/stages/play/collision_detector.hpp"
#include "game/stages/play/components/transformable.hpp"
#include <cmath>

std::optional<Collision> detectCollision(const Transformable& firstTransformable,
                                         const Transformable& secondTransformable)
{

    const auto w = 0.5f * (firstTransformable.size.x + secondTransformable.size.x);
    const auto h = 0.5f * (firstTransformable.size.y + secondTransformable.size.y);
    const auto dx = (firstTransformable.position.x + 0.5f * firstTransformable.size.x) -
                    (secondTransformable.position.x + 0.5f * secondTransformable.size.x);
    const auto dy = (firstTransformable.position.y + 0.5f * firstTransformable.size.y) -
                    (secondTransformable.position.y + 0.5f * secondTransformable.size.y);

    if (std::fabs(dx) < w && std::fabs(dy) < h)
    {
        const auto wy = w * dy;
        const auto hx = h * dx;

        if (wy > hx)
        {
            if (wy > -hx)
            {
                return Collision{{0, h - std::fabs(dy)}, Direction::Up};
            }
            else
                return Collision{{-(w - std::fabs(dx)), 0}, Direction::Right};
        }
        else
        {
            if (wy > -hx)
                return Collision{{w - std::fabs(dx), 0}, Direction::Left};
            else
                return Collision{{0, -(h - std::fabs(dy))}, Direction::Down};
        }
    }

    return std::nullopt;
}
