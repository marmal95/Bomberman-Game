#include "game/stages/menu/menu_stage.hpp"
#include "game/game_manager.hpp"
#include "util/constants.hpp"
#include "util/converters.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

MenuStage::MenuStage(GameManager& gameManager)
    : Stage{gameManager},
      textures{},
      fonts{},
      background{},
      title{},
      playAgain{},
      exitGame{},
      selectedOption{Option::PlayAgain}
{
    resizeWindow();
    loadResources();
    initSprites();
    initTexts();
    initLayout();
}

void MenuStage::handleEvent(const sf::Event& event)
{
    if (isKeyReleased(event, sf::Keyboard::Space))
    {
        changeStage();
    }
    if (isKeyReleased(event, sf::Keyboard::Down))
    {
        selectedOption = Option::ExitGame;
    }
    if (isKeyReleased(event, sf::Keyboard::Up))
    {
        selectedOption = Option::PlayAgain;
    }
}

bool MenuStage::update(const TimeDelta)
{
    repaintOptions();
    return true;
}

void MenuStage::render(sf::RenderTarget& target)
{
    target.draw(background);
    target.draw(title);
    target.draw(playAgain);
    target.draw(exitGame);
}

void MenuStage::resizeWindow() const
{
    const auto desktopMode = sf::VideoMode::getDesktopMode();
    auto& window = gameManager.getWindow();
    window.setSize(MENU_WINDOW_SIZE);
    window.setView(sf::View(sf::FloatRect(sf::Vector2f{}, toVector2f(MENU_WINDOW_SIZE))));
    window.setPosition({static_cast<int>(desktopMode.width - window.getSize().x) / 2,
                        static_cast<int>(desktopMode.height - window.getSize().y) / 2});
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
    playAgain.setFillColor(sf::Color{239, 209, 58});
    playAgain.setCharacterSize(48);
    playAgain.setString("Play again");

    exitGame.setFont(fonts.getResource(ResourceID::Font));
    exitGame.setFillColor(sf::Color{239, 209, 58});
    exitGame.setCharacterSize(48);
    exitGame.setString("Exit game");
}

void MenuStage::initLayout()
{
    playAgain.setPosition((static_cast<float>(MENU_WINDOW_SIZE.y) - playAgain.getGlobalBounds().width) / 2, 340);
    exitGame.setPosition((static_cast<float>(MENU_WINDOW_SIZE.x) - exitGame.getGlobalBounds().width) / 2, 340 + 64);
}

void MenuStage::repaintOptions()
{
    if (selectedOption == Option::PlayAgain)
    {
        playAgain.setFillColor(sf::Color{180, 0, 0});
        exitGame.setFillColor(sf::Color{239, 209, 58});
    }
    else if (selectedOption == Option::ExitGame)
    {
        exitGame.setFillColor(sf::Color{180, 0, 0});
        playAgain.setFillColor(sf::Color{239, 209, 58});
    }
}

void MenuStage::changeStage() const
{
    switch (selectedOption)
    {
    case MenuStage::Option::PlayAgain:
        gameManager.changeToGame();
        break;
    case MenuStage::Option::ExitGame:
        gameManager.getWindow().close();
        break;
    }
}