#pragma once

#include "GameStage.hpp"
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"
#include "MenuOption.hpp"

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
	void initTexts();
	void initLayout();

	void repaintOptions();
	void changeStage() const;

	ResourceHolder<sf::Texture, ResourceID> textures;
	ResourceHolder<sf::Font, ResourceID> fonts;
	sf::Sprite background;
	sf::Sprite title;
	sf::Text playAgain;
	sf::Text exitGame;
	Option selectedOption;
};

