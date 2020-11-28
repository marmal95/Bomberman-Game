#pragma once

#include <entityx/entityx.h>

namespace sf
{
	class RenderWindow;
	class Event;
}

class IGameStage
{
public:
	virtual bool update(const entityx::TimeDelta) = 0;
	virtual void draw(sf::RenderWindow&) = 0;
	virtual void handleEvent(sf::Event&) = 0;
};