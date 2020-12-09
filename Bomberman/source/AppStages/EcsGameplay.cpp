#include "EcsGameplay.hpp"
#include "MoveSystem.hpp"
#include "DrawSystem.hpp"
#include "AnimateSystem.hpp"
#include "CollisionSystem.hpp"
#include "ExplosionSystem.hpp"
#include "SpawnSystem.hpp"
#include "GameplayStage.hpp"
#include "SpawnBombEvent.hpp"
#include "FinishGameEvent.hpp"
#include "ExitStage.hpp"
#include "Player.hpp"
#include "PlayersCreator.hpp"
#include "MapCreator.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

EcsGameplay::EcsGameplay(const GameplayStage& stage)
    : gameplayStage{ stage }, gameStatus{ GameStatus::Running }
{
    systems.add<SpawnSystem>(gameplayStage.getTextures());
    systems.add<MoveSystem>();
    systems.add<CollisionSystem>();
    systems.add<ExplosionSystem>(gameplayStage.getSounds());
    systems.add<AnimateSystem>();
    systems.add<DrawSystem>(gameplayStage.getTextures());
    systems.configure();
    events.subscribe<GameFinishedEvent>(*this);

    PlayersCreator playersCreator{ gameplayStage.getTextures(), entities, events };
    playersCreator.createBomberman();
    playersCreator.createCreep();

    MapCreator{ entities, events }.createMap();
}

void EcsGameplay::receive(const GameFinishedEvent&)
{
    const auto bomberman = (*entities.entities_with_components<Player, Bomberman>().begin()).component<Player>();
    const auto creep = (*entities.entities_with_components<Player, Creep>().begin()).component<Player>();

    GameStatus finalGameStatus{};
    if (bomberman->health <= 0 && creep->health <= 0)
        finalGameStatus = GameStatus::Draw;
    else if (bomberman->health > 0)
        finalGameStatus = GameStatus::BombermanWon;
    else if (creep->health > 0)
        finalGameStatus = GameStatus::CreepWon;
    GameStage::changeStage(std::make_unique<ExitStage>(finalGameStatus));
}

bool EcsGameplay::update(const entityx::TimeDelta dt)
{
    systems.update<SpawnSystem>(dt);
    systems.update<MoveSystem>(dt);
    systems.update<CollisionSystem>(dt);
    systems.update<ExplosionSystem>(dt);
    systems.update<AnimateSystem>(dt);
    checkIsGameOver();
    return true;
}

void EcsGameplay::draw(sf::RenderWindow&)
{
    systems.update<DrawSystem>({});
}

void EcsGameplay::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::EventType::KeyReleased)
    {
        auto bomberman = entities.entities_with_components<Bomberman>().begin();
        auto creep = entities.entities_with_components<Creep>().begin();

        if (event.key.code == sf::Keyboard::RControl && (*bomberman).has_component<Movable>())
            events.emit<SpawnBombEvent>({ *bomberman });

        if (event.key.code == sf::Keyboard::LControl && (*creep).has_component<Movable>())
            events.emit<SpawnBombEvent>({ *creep });
    }
}

void EcsGameplay::checkIsGameOver()
{
    if (gameStatus != GameStatus::Running)
        return;

    const auto bomberman = (*entities.entities_with_components<Player, Bomberman>().begin()).component<Player>();
    const auto creep = (*entities.entities_with_components<Player, Creep>().begin()).component<Player>();

    if (!bomberman->health || !creep->health)
    {
        events.emit<FinishGameEvent>();
        gameStatus = GameStatus::Finishing;
    }
}
