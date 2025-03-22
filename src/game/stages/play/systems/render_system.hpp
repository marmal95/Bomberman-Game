#pragma once

#include "enums/resource_id.hpp"
#include "game/stages/play/resource_holder.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace sf
{
class RenderTarget;
}
struct Drawable;
struct Transformable;
struct Player;
struct MoveChangeEvent;

class RenderSystem
{
  public:
    RenderSystem(entt::registry&,
                 entt::dispatcher& dispatcher,
                 const ResourceHolder<sf::Texture, ResourceID>& textures);
    void update(sf::RenderTarget&);

  private:
    void handleEvent(const MoveChangeEvent&);
    void handleBombermanMoveChangeEvent(const MoveChangeEvent&);
    void handleCreepMoveChangeEvent(const MoveChangeEvent&);
    void handlePlayerMove(const MoveChangeEvent& event,
                          const ResourceID leftResourceId,
                          const ResourceID rightResourceId,
                          const ResourceID backResourceId,
                          const ResourceID frontResourceId);

    void drawEntity(sf::RenderTarget&, const entt::entity, Drawable&, const Transformable&) const;
    void changeOpacity(Drawable&, const Player&) const;

    entt::registry& registry;
    const ResourceHolder<sf::Texture, ResourceID>& textures;
};