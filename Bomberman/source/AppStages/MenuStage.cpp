#include "MenuStage.hpp"
#include "ResourceManager.hpp"
#include "Constants.hpp"
#include <SFML/Window/Event.hpp>

MenuStage::MenuStage()
    : textures{}, fonts{}, background{}, title{}, playAgain{}, exitGame{}, selectedOption{ Option::PlayAgain }
{
    resizeWindow();
    loadResources();
    initSprites();
    initTexts();
    initLayout();
}

bool MenuStage::update(const entityx::TimeDelta)
{
    repaintOptions();
    return true;
}

void MenuStage::draw(sf::RenderWindow& window)
{
    window.draw(background);
    window.draw(title);
    window.draw(playAgain);
    window.draw(exitGame);
}

void MenuStage::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Down)
        selectedOption = Option::ExitGame;
    else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Up)
        selectedOption = Option::PlayAgain;
    else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space)
        changeStage();
}

void MenuStage::resizeWindow() const
{
    const auto desktopMode = sf::VideoMode::getDesktopMode();
    auto& window = ResourcesManager::getInstance().window;
    window.setSize(sf::Vector2u(576, 576));
    window.setView(sf::View(sf::FloatRect(0, 0, 576, 576)));
    window.setPosition({ static_cast<int>(desktopMode.width - window.getSize().x) / 2,
                         static_cast<int>(desktopMode.height - window.getSize().y) / 2 });
}

void MenuStage::loadResources()
{
    textures.load(ResourceID::MenuBackground, "resources/Menu/title_background.jpg");
    textures.load(ResourceID::MenuTitle, "resources/Menu/title_titletext.png");
}

void MenuStage::initSprites()
{
    background.setTexture(textures.getResource(ResourceID::MenuBackground));
    title.setTexture(textures.getResource(ResourceID::MenuTitle));
    fonts.load(ResourceID::Font, "resources/Font/AmazDooMLeft.ttf");
}

void MenuStage::initTexts()
{
    playAgain.setFont(fonts.getResource(ResourceID::Font));
    playAgain.setFillColor(sf::Color{ 239, 209, 58 });
    playAgain.setCharacterSize(48);
    playAgain.setString("Play again");

    exitGame.setFont(fonts.getResource(ResourceID::Font));
    exitGame.setFillColor(sf::Color{ 239, 209, 58 });
    exitGame.setCharacterSize(48);
    exitGame.setString("Exit game");
}

void MenuStage::initLayout()
{
    const auto& window = ResourcesManager::getInstance().window;
    playAgain.setPosition((MENU_WINDOW_SIZE.y - playAgain.getGlobalBounds().width) / 2, 340);
    exitGame.setPosition((MENU_WINDOW_SIZE.x - exitGame.getGlobalBounds().width) / 2, 340 + 64);
}

void MenuStage::repaintOptions()
{
    if (selectedOption == Option::PlayAgain)
    {
        playAgain.setFillColor(sf::Color{ 180, 0, 0 });
        exitGame.setFillColor(sf::Color{ 239, 209, 58 });
    }
    else if (selectedOption == Option::ExitGame)
    {
        exitGame.setFillColor(sf::Color{ 180, 0, 0 });
        playAgain.setFillColor(sf::Color{ 239, 209, 58 });
    }
}

void MenuStage::changeStage() const
{
    if (selectedOption == Option::PlayAgain)
        GameStage::changeStage(std::make_unique<GameplayStage>());
    else if (selectedOption == Option::ExitGame)
        ResourcesManager::getInstance().window.close();
}