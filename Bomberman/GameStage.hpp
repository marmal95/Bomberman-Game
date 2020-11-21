#pragma once

#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <memory>

class GameStage
{
public:
	GameStage() = default;
	GameStage(const GameStage&) = delete;
	GameStage(GameStage&&) = delete;
	GameStage& operator=(const GameStage&) = delete;
	GameStage& operator=(GameStage&&) = delete;
	virtual ~GameStage() = default;

	virtual bool update(const entityx::TimeDelta) = 0;
	virtual void draw(sf::RenderWindow&) = 0;
	virtual void handleEvent(sf::Event&) = 0;

	static bool run(const entityx::TimeDelta, sf::RenderWindow&);
	static std::unique_ptr<GameStage>& getStage();
	static void changeStage(std::unique_ptr<GameStage>);

private:
	static std::unique_ptr<GameStage> activeStage;
};

