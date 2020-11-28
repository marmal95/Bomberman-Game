#pragma once

#include <entityx/entityx.h>
#include <SFML/System/Vector2.hpp>
#include "IGameStage.hpp"
#include "GameStatus.hpp"

class GameplayStage;
struct Map;

class EcsGameplay : public entityx::EntityX, public IGameStage
{
public:
	EcsGameplay(const GameplayStage&);
	EcsGameplay(const EcsGameplay&) = delete;
	EcsGameplay(EcsGameplay&&) = delete;
	EcsGameplay& operator=(const EcsGameplay&) = delete;
	EcsGameplay& operator=(EcsGameplay&&) = delete;
	~EcsGameplay() = default;

	bool update(const entityx::TimeDelta) override;
	void draw(sf::RenderWindow&) override;
	void handleEvent(sf::Event&) override;

private:
	void createMap();
	void createExplodableBlocks(Map&, std::vector<sf::Vector2i>&);
	void createBomberman();
	void createCreep();
	void checkIsGameOver();

	const GameplayStage& gameplayStage;
	GameStatus gameStatus;
};

