#include "game/stages/play/play_stage.hpp"
#include "components/drawable.hpp"
#include "components/movable.hpp"
#include "game/game_manager.hpp"
#include "game/stages/play/components/animated.hpp"
#include "game/stages/play/components/collidable.hpp"
#include "game/stages/play/components/drawable.hpp"
#include "game/stages/play/components/movable.hpp"
#include "game/stages/play/components/player.hpp"
#include "game/stages/play/components/transformable.hpp"
#include "game/stages/play/components/z_index.hpp"
#include "game/stages/play/events/game_finished_event.hpp"
#include "game/stages/play/events/move_change_event.hpp"
#include "game/stages/play/events/spawn_bomb_event.hpp"
#include "game/stages/play/map_creator.hpp"
#include "util/constants.hpp"
#include "util/converters.hpp"
#include "util/tile_calculator.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

PlayStage::PlayStage(GameManager& gameManager)
    : Stage{gameManager},
      registry{},
      dispatcher{},
      animateSystem{registry, dispatcher},
      collisionSystem{registry, dispatcher},
      explosionSystem{registry, dispatcher, sounds},
      moveSystem{registry, dispatcher},
      renderSystem{registry, dispatcher, textures},
      spawnSystem{registry, dispatcher, textures},
      gameResult{GameResult::Running}
{
    resizeWindow();
    loadResources();
    createPlayers();

    MapCreator{registry, dispatcher}.createMap();

    dispatcher.sink<GameFinishedEvent>().connect<&PlayStage::handleGameFinishedEvent>(this);
}

void PlayStage::handleEvent(const sf::Event& event)
{
    if (isKeyReleased(event, sf::Keyboard::Escape))
    {
        gameManager.changeToExit({});
    }
    if (isKeyReleased(event, sf::Keyboard::LControl))
    {
        const auto entity = registry.view<Player, Creep>().front();
        if (registry.try_get<Movable>(entity))
        {
            dispatcher.trigger<SpawnBombEvent>(SpawnBombEvent{entity});
        }
    }
    if (isKeyReleased(event, sf::Keyboard::RControl))
    {
        const auto entity = registry.view<Player, Bomberman>().front();
        if (registry.try_get<Movable>(entity))
        {
            dispatcher.trigger<SpawnBombEvent>(SpawnBombEvent{entity});
        }
    }
}

bool PlayStage::update(const TimeDelta dt)
{
    collisionSystem.update(dt);
    explosionSystem.update(dt);
    moveSystem.update(dt);
    spawnSystem.update(dt);
    animateSystem.update(dt);
    checkIsGameOver();
    return true;
}

void PlayStage::render(sf::RenderTarget& target)
{
    renderSystem.update(target);
}

void PlayStage::resizeWindow() const
{
    const auto desktopMode = sf::VideoMode::getDesktopMode();
    auto& window = gameManager.getWindow();
    window.setSize(GAME_WINDOW_SIZE);
    window.setView(sf::View(sf::FloatRect(0, 0, GAME_WINDOW_SIZE.x, GAME_WINDOW_SIZE.y)));
    window.setPosition({static_cast<int>(desktopMode.width - window.getSize().x) / 2, 0});
}

void PlayStage::loadResources()
{
    textures.load(ResourceID::BackgroundTile, "resources/Blocks/BackgroundTile.png");
    textures.load(ResourceID::ExplodableBlock, "resources/Blocks/ExplodableBlock.png");
    textures.load(ResourceID::SolidBlock, "resources/Blocks/SolidBlock.png");
    textures.load(ResourceID::SolidRedBlock, "resources/Blocks/SolidRedBlock.png");
    textures.load(ResourceID::Portal, "resources/Blocks/Portal.png");
    textures.load(ResourceID::BombermanBack, "resources/Bomberman/Bomberman_Back.png");
    textures.load(ResourceID::BombermanFront, "resources/Bomberman/Bomberman_Front.png");
    textures.load(ResourceID::BombermanLeft, "resources/Bomberman/Bomberman_Left.png");
    textures.load(ResourceID::BombermanRight, "resources/Bomberman/Bomberman_Right.png");
    textures.load(ResourceID::CreepBack, "resources/Creep/Creep_Back.png");
    textures.load(ResourceID::CreepFront, "resources/Creep/Creep_Front.png");
    textures.load(ResourceID::CreepLeft, "resources/Creep/Creep_Left.png");
    textures.load(ResourceID::CreepRight, "resources/Creep/Creep_Right.png");
    textures.load(ResourceID::Bomb, "resources/Bomb/Bomb.png");
    textures.load(ResourceID::Flame, "resources/Flame/Flame.png");
    textures.load(ResourceID::BombPowerUp, "resources/Powerups/BombPowerup.png");
    textures.load(ResourceID::FlamePowerUp, "resources/Powerups/FlamePowerup.png");
    textures.load(ResourceID::SpeedPowerUp, "resources/Powerups/SpeedPowerup.png");
    sounds.load(ResourceID::BombSound, "resources/Sounds/bomb.wav");
}

void PlayStage::createPlayers()
{
    {
        auto entity = registry.create();
        registry.emplace<Drawable>(entity, textures.getResource(ResourceID::BombermanFront));
        registry.emplace<Transformable>(
            entity,
            Transformable{toVector2f(BOMBERMAN_SIZE),
                          calculatePositionInTileCenter({1, 1}, BOMBERMAN_SIZE)});
        registry.emplace<Movable>(entity,
                                  Movable{toVector2f(PLAYER_INITIAL_SPEED), Direction::None});
        registry.emplace<Animated>(entity,
                                   Animated{{BOMBERMAN_SPRITE_SIZE.x, BOMBERMAN_SPRITE_SIZE.y},
                                            8,
                                            PLAYER_INITIAL_ANIM_SPEED});
        registry.emplace<Collidable>(entity);
        registry.emplace<Player>(entity);
        registry.emplace<Bomberman>(entity);
        registry.emplace<ZIndex>(entity, PLAYERS_Z_INDEX);
        dispatcher.trigger<MoveChangeEvent>({entity, Direction::None});
    }
    {
        auto entity = registry.create();
        registry.emplace<Drawable>(entity, textures.getResource(ResourceID::CreepFront));
        registry.emplace<Transformable>(
            entity,
            Transformable{toVector2f(CREEP_SIZE),
                          calculatePositionInTileCenter({19, 19}, CREEP_SIZE)});
        registry.emplace<Movable>(entity,
                                  Movable{toVector2f(PLAYER_INITIAL_SPEED), Direction::None});
        registry.emplace<Animated>(
            entity,
            Animated{{CREEP_SPRITE_SIZE.x, CREEP_SPRITE_SIZE.y}, 6, PLAYER_INITIAL_ANIM_SPEED});
        registry.emplace<Collidable>(entity);
        registry.emplace<Player>(entity);
        registry.emplace<Creep>(entity);
        registry.emplace<ZIndex>(entity, PLAYERS_Z_INDEX);
        dispatcher.trigger<MoveChangeEvent>({entity, Direction::None});
    }
}

void PlayStage::handleGameFinishedEvent(const GameFinishedEvent&)
{
    const auto bomberman = registry.view<Player, Bomberman>().front();
    const auto creep = registry.view<Player, Creep>().front();

    const auto& bombermanPlayer = registry.get<Player>(bomberman);
    const auto& creepPlayer = registry.get<Player>(creep);

    GameResult finalGameStatus{};
    if (bombermanPlayer.health <= 0 && creepPlayer.health <= 0)
        finalGameStatus = GameResult::Draw;
    else if (bombermanPlayer.health > 0)
        finalGameStatus = GameResult::BombermanWon;
    else if (creepPlayer.health > 0)
        finalGameStatus = GameResult::CreepWon;
    gameManager.changeToExit(finalGameStatus);
}

void PlayStage::checkIsGameOver()
{
    if (gameResult != GameResult::Running)
        return;

    const auto bomberman = registry.view<Player, Bomberman>().front();
    const auto creep = registry.view<Player, Creep>().front();

    const auto& bombermanPlayer = registry.get<Player>(bomberman);
    const auto& creepPlayer = registry.get<Player>(creep);

    if (!bombermanPlayer.health || !creepPlayer.health)
    {
        dispatcher.trigger<FinishGameEvent>();
        gameResult = GameResult::Finishing;
    }
}