#pragma once

#include <entityx/entityx.h>
#include <SFML/System/Vector2.hpp>
#include "IGameStage.hpp"
#include "GameStatus.hpp"

class GameplayStage;
struct Map;
struct GameFinishedEvent;

class EcsGameplay : public entityx::EntityX, public IGameStage, public entityx::Receiver<EcsGameplay>
{
public:
	EcsGameplay(const GameplayStage&);
	EcsGameplay(const EcsGameplay&) = delete;
	EcsGameplay(EcsGameplay&&) = delete;
	EcsGameplay& operator=(const EcsGameplay&) = delete;
	EcsGameplay& operator=(EcsGameplay&&) = delete;
	~EcsGameplay() = default;

	void receive(const GameFinishedEvent&);
	bool update(const entityx::TimeDelta) override;
	void draw(sf::RenderWindow&) override;
	void handleEvent(sf::Event&) override;

private:
	void checkIsGameOver();

	const GameplayStage& gameplayStage;
	GameStatus gameStatus;
};

