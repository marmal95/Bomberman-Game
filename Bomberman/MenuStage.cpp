#include "MenuStage.hpp"
#include "ResourceManager.hpp"
#include <iostream>

MenuStage::MenuStage()
{
    resizeWindow();
    loadResources();
    initSprites();
    initLayout();
}

bool MenuStage::update(const entityx::TimeDelta)
{
    return true;
}

void MenuStage::draw(sf::RenderWindow& window)
{
    window.draw(background);
    window.draw(title);
    window.draw(onePlayerOption);
    window.draw(twoPlayersOption);
    window.draw(controlPlayerOne);
    window.draw(controlPlayerTwo);
}

void MenuStage::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::EventType::KeyPressed &&
        event.key.code == sf::Keyboard::Space)
    {
        GameStage::changeStage(std::make_unique<GameplayStage>());
    }
    else if (event.type == sf::Event::EventType::MouseMoved)
    {
        const auto mousePos = sf::Mouse::getPosition(ResourcesManager::getInstance().window);

        isMouseOverElement(onePlayerOption) ? 
            onePlayerOption.setTexture(textures.getResource(ResourceID::MenuOnePlayerHover)) :
            onePlayerOption.setTexture(textures.getResource(ResourceID::MenuOnePlayer));

        isMouseOverElement(twoPlayersOption) ?
            twoPlayersOption.setTexture(textures.getResource(ResourceID::MenuTwoPlayersHover)) :
            twoPlayersOption.setTexture(textures.getResource(ResourceID::MenuTwoPlayers));
    }
}

void MenuStage::resizeWindow() const
{
    ResourcesManager::getInstance().window.setSize(sf::Vector2u(576, 576));
    ResourcesManager::getInstance().window.setView(sf::View(sf::FloatRect(0, 0, 576, 576)));
}

void MenuStage::loadResources()
{
    textures.load(ResourceID::MenuBackground, "resources/Menu/title_background.jpg");
    textures.load(ResourceID::MenuTitle, "resources/Menu/title_titletext.png");
    textures.load(ResourceID::MenuOnePlayer, "resources/Menu/One_Player_Normal.png");
    textures.load(ResourceID::MenuOnePlayerHover, "resources/Menu/One_Player_Hover.png");
    textures.load(ResourceID::MenuTwoPlayers, "resources/Menu/Two_Players_Normal.png");
    textures.load(ResourceID::MenuTwoPlayersHover, "resources/Menu/Two_Players_Hover.png");
    textures.load(ResourceID::MenuControlPlayerOne, "resources/Menu/Control_PlayerOne.png");
    textures.load(ResourceID::MenuControlPlayerTwo, "resources/Menu/Control_PlayerTwo.png");
}

void MenuStage::initSprites()
{
    background.setTexture(textures.getResource(ResourceID::MenuBackground));
    title.setTexture(textures.getResource(ResourceID::MenuTitle));
    onePlayerOption.setTexture(textures.getResource(ResourceID::MenuOnePlayer));
    twoPlayersOption.setTexture(textures.getResource(ResourceID::MenuTwoPlayers));
    controlPlayerOne.setTexture(textures.getResource(ResourceID::MenuControlPlayerOne));
    controlPlayerTwo.setTexture(textures.getResource(ResourceID::MenuControlPlayerTwo));
}

void MenuStage::initLayout()
{
    const auto& window = ResourcesManager::getInstance().window;
    onePlayerOption.setOrigin(twoPlayersOption.getTexture()->getSize().x / 2, twoPlayersOption.getTexture()->getSize().y / 2);
    onePlayerOption.setPosition(window.getSize().x / 2, 320);

    twoPlayersOption.setOrigin(twoPlayersOption.getTexture()->getSize().x / 2, twoPlayersOption.getTexture()->getSize().y / 2);
    twoPlayersOption.setPosition(window.getSize().x / 2, onePlayerOption.getPosition().y + 40);

    controlPlayerOne.setOrigin(controlPlayerOne.getTexture()->getSize().x / 2, controlPlayerOne.getTexture()->getSize().y / 2);
    controlPlayerOne.setPosition(twoPlayersOption.getPosition().x, twoPlayersOption.getPosition().y + 60);

    controlPlayerTwo.setOrigin(controlPlayerTwo.getTexture()->getSize().x / 2, controlPlayerTwo.getTexture()->getSize().y / 2);
    controlPlayerTwo.setPosition(controlPlayerOne.getPosition().x, controlPlayerOne.getPosition().y + 40);
}

bool MenuStage::isMouseOverElement(const sf::Sprite& element) const
{
    const auto mousePos = sf::Mouse::getPosition(ResourcesManager::getInstance().window);
    const auto& elementPos = element.getPosition();
    const auto& elementSize = element.getTexture()->getSize();

    return ((mousePos.x >= (elementPos.x - (elementSize.x / 2))) &&
            (mousePos.x <= (elementPos.x + (elementSize.x / 2))) &&
            (mousePos.y <= (elementPos.y + (elementSize.y / 2))) &&
            (mousePos.y >= (elementPos.y - (elementSize.y / 2))));
}
