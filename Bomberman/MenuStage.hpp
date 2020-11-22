#pragma once

#include "GameStage.hpp"
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"

class MenuStage : public GameStage
{
public:
	MenuStage();
	bool update(const entityx::TimeDelta) override;
	void draw(sf::RenderWindow&) override;
	void handleEvent(sf::Event&) override;

private:
	void resizeWindow() const;
	void loadResources();
	void initSprites();
	void initLayout();

	bool isMouseOverElement(const sf::Sprite&) const;

	ResourceHolder<sf::Texture, ResourceID> textures;
	sf::Sprite background;
	sf::Sprite title;
	sf::Sprite onePlayerOption;
	sf::Sprite twoPlayersOption;
	sf::Sprite controlPlayerOne;
	sf::Sprite controlPlayerTwo;
};

