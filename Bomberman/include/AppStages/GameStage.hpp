#pragma once

#include "IGameStage.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

class GameStage : public IGameStage
{
public:
	GameStage() = default;
	GameStage(const GameStage&) = delete;
	GameStage(GameStage&&) = delete;
	GameStage& operator=(const GameStage&) = delete;
	GameStage& operator=(GameStage&&) = delete;
	virtual ~GameStage() = default;

	static bool run(const entityx::TimeDelta, sf::RenderWindow&);
	static std::unique_ptr<GameStage>& getStage();
	static void changeStage(std::unique_ptr<GameStage>);

private:
	static std::unique_ptr<GameStage> activeStage;
};

