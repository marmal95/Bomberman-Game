#pragma once

#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>

class GameStage
{
public:
	GameStage() = default;
	GameStage(const GameStage&) = delete;
	GameStage(GameStage&&) = delete;
	GameStage& operator=(const GameStage&) = delete;
	GameStage& operator=(GameStage&&) = delete;
	virtual ~GameStage() = default;

	bool run(entityx::TimeDelta, sf::RenderWindow& window);
	GameStage* get_stage();

	virtual bool init() = 0;
	virtual bool update(float) = 0;
	virtual void draw(sf::RenderWindow&) = 0;
	virtual void release() = 0;

private:
	GameStage* active_stage;
	GameStage* next_stage;
};

