#pragma once

#include "GameStage.hpp"
#include "GameResult.hpp"

class ExitStage : public GameStage
{
public:
	ExitStage(const GameResult);
	bool update(const entityx::TimeDelta) override;
	void draw(sf::RenderWindow&) override;
	void handleEvent(sf::Event&) override;
};

