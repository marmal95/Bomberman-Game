#pragma once

#include <entityx/entityx.h>
#include <SFML/Graphics/Texture.hpp>
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"
#include "MoveChangeEvent.hpp"

struct Drawable;
struct Transformable;
struct Player;

namespace sf
{
    class RenderWindow;
}

class DrawSystem : public entityx::System<DrawSystem>, public entityx::Receiver<DrawSystem>
{
public:
    DrawSystem(const ResourceHolder<sf::Texture, ResourceID>&);
    void update(entityx::EntityManager&, entityx::EventManager&, entityx::TimeDelta) override;
    void configure(entityx::EventManager&) override;
    void receive(const MoveChangeEvent&);

private:
    void handleMoveChangeEvents();
    void handleBombermanMoveChangeEvent(MoveChangeEvent&);
    void handleCreepMoveChangeEvent(MoveChangeEvent&);

    void drawPlayers(entityx::EntityManager&) const;
    void drawEntity(entityx::Entity, Drawable&, Transformable&) const;
    void changeOpacity(Drawable&, const Player&) const;

    const ResourceHolder<sf::Texture, ResourceID>& textures;
    std::vector<MoveChangeEvent> moveChangeEvents;
};

