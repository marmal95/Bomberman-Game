#pragma once

#include "enums/game_result.hpp"
#include "enums/resource_id.hpp"
#include "game/stages/play/entity_creator.hpp"
#include "game/stages/play/systems/animate_system.hpp"
#include "game/stages/play/systems/collision_system.hpp"
#include "game/stages/play/systems/explosion_system.hpp"
#include "game/stages/play/systems/move_system.hpp"
#include "game/stages/play/systems/render_system.hpp"
#include "game/stages/play/systems/spawn_system.hpp"
#include "game/stages/stage.hpp"
#include "util/resource_holder.hpp"
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <entt/entt.hpp>

namespace sf
{
class RenderTarget;
class Event;
} // namespace sf

struct GameFinishedEvent;

class GameManager;

class PlayStage : public Stage
{
  public:
    explicit PlayStage(GameManager& gameManager);
    void handleEvent(const sf::Event&) override;
    bool update(const TimeDelta) override;
    void render(sf::RenderTarget&) override;

  private:
    void resizeWindow() const;
    void loadResources();
    void createPlayers();

    void handleGameFinishedEvent(const GameFinishedEvent&);
    void checkIsGameOver();

    ResourceHolder<sf::Texture, ResourceID> textures;
    ResourceHolder<sf::SoundBuffer, ResourceID> sounds;

    entt::registry registry;
    entt::dispatcher dispatcher;
    EntityCreator entityCreator;

    AnimateSystem animateSystem;
    CollisionSystem collisionSystem;
    ExplosionSystem explosionSystem;
    MoveSystem moveSystem;
    RenderSystem renderSystem;
    SpawnSystem spawnSystem;

    GameResult gameResult;
};