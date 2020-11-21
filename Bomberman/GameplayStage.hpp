#pragma once

#include "GameStage.hpp"

class GameplayStage : public GameStage
{
public:
	bool update(const entityx::TimeDelta) override;
	void draw(sf::RenderWindow& window) override;
	void handleEvent(sf::Event&) override;

private:
};
