#include "game/stages/play/play_stage.hpp"
#include "game/game_manager.hpp"
#include "game/stages/play/components/movable.hpp"
#include "game/stages/play/components/player.hpp"
#include "game/stages/play/config.hpp"
#include "game/stages/play/entity_creator.hpp"
#include "game/stages/play/events/game_finished_event.hpp"
#include "game/stages/play/events/move_change_event.hpp"
#include "game/stages/play/events/spawn_bomb_event.hpp"
#include "game/stages/play/map_creator.hpp"
#include "util/constants.hpp"
#include "util/converters.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

PlayStage::PlayStage(GameManager& gameManager)
    : Stage{gameManager},
      config{config::parse()},
      textures{},
      sounds{},
      registry{},
      dispatcher{},
      entityCreator{registry, textures},
      animateSystem{registry, dispatcher},
      collisionSystem{registry, dispatcher, config},
      explosionSystem{registry, dispatcher, sounds},
      moveSystem{registry, dispatcher},
      renderSystem{registry, dispatcher, textures},
      spawnSystem{registry, dispatcher, entityCreator},
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
    if (isKeyReleased(event, sf::Keyboard::P))
    {
        if (gameResult == GameResult::Running)
        {
            gameResult = GameResult::Paused;
        }
        else if (gameResult == GameResult::Paused)
        {
            gameResult = GameResult::Running;
        }
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
    if (gameResult != GameResult::Paused)
    {
        collisionSystem.update(dt);
        explosionSystem.update(dt);
        moveSystem.update(dt);
        spawnSystem.update(dt);
        animateSystem.update(dt);
        checkIsGameOver();
    }
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
    window.setView(sf::View(sf::FloatRect(sf::Vector2f{}, toVector2f(GAME_WINDOW_SIZE))));
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
    auto bomberman = entityCreator.createBomberman(config);
    dispatcher.trigger<MoveChangeEvent>({bomberman, Direction::None});

    const auto creep = entityCreator.createCreep(config);
    dispatcher.trigger<MoveChangeEvent>({creep, Direction::None});
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
    if (gameResult != GameResult::Finishing)
    {
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
}