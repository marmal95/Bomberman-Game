#pragma once

#include <entityx/entityx.h>
#include "IGameStage.hpp"

class GameplayStage;

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
	void createPlayer();

	const GameplayStage& gameplayStage;
};

