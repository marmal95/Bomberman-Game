#include "game/stages/exit/exit_stage.hpp"
#include "game/game_manager.hpp"
#include "util/constants.hpp"
#include "util/converters.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

ExitStage::ExitStage(GameManager& gameManager, const GameResult gameResult)
    : Stage{gameManager},
      textures{},
      fonts{},
      background{},
      bomberman{},
      creep{},
      bombermanStatus{},
      creepStatus{},
      playAgain{},
      exitGame{},
      selectedOption{Option::PlayAgain}
{
    resizeWindow();
    loadResources();
    initSprites();
    initTexts();
    initLayout();

    bombermanStatus.setString((gameResult == GameResult::BombermanWon || gameResult == GameResult::Draw) ? "Winner"
                                                                                                         : "Loser");
    creepStatus.setString((gameResult == GameResult::CreepWon || gameResult == GameResult::Draw) ? "Winner" : "Loser");
}

void ExitStage::handleEvent(const sf::Event& event)
{
    if (isKeyReleased(event, sf::Keyboard::Space))
    {
        changeStage();
    }
    else if (isKeyReleased(event, sf::Keyboard::Escape))
    {
        gameManager.getWindow().close();
    }
    else if (isKeyReleased(event, sf::Keyboard::Down))
    {
        selectedOption = Option::ExitGame;
    }
    else if (isKeyReleased(event, sf::Keyboard::Up))
    {
        selectedOption = Option::PlayAgain;
    }
}

bool ExitStage::update(const TimeDelta)
{
    repaintOptions();
    return true;
}

void ExitStage::render(sf::RenderTarget& target)
{
    target.draw(background);
    target.draw(bomberman);
    target.draw(creep);
    target.draw(bombermanStatus);
    target.draw(creepStatus);
    target.draw(playAgain);
    target.draw(exitGame);
}

void ExitStage::resizeWindow() const
{
    const auto desktopMode = sf::VideoMode::getDesktopMode();
    auto& window = gameManager.getWindow();
    window.setSize(EXIT_WINDOW_SIZE);
    window.setView(sf::View(sf::FloatRect(sf::Vector2f{}, toVector2f(EXIT_WINDOW_SIZE))));
    window.setPosition({static_cast<int>(desktopMode.width - window.getSize().x) / 2,
                        static_cast<int>(desktopMode.height - window.getSize().y) / 2});
}

void ExitStage::loadResources()
{
    textures.load(ResourceID::MenuBackground, "resources/Menu/title_background.jpg");
    textures.load(ResourceID::MenuTitle, "resources/Menu/title_titletext.png");
    textures.load(ResourceID::BombermanFront, "resources/Bomberman/Bomberman_Front.png");
    textures.load(ResourceID::CreepFront, "resources/Creep/Creep_Front.png");
    fonts.load(ResourceID::Font, "resources/Font/AmazDooMLeft.ttf");
}

void ExitStage::initSprites()
{
    background.setTexture(textures.getResource(ResourceID::MenuBackground));
    bomberman.setTexture(textures.getResource(ResourceID::BombermanFront));
    bomberman.setTextureRect(sf::IntRect(0, 0, BOMBERMAN_SPRITE_SIZE.x, BOMBERMAN_SPRITE_SIZE.y));
    creep.setTexture(textures.getResource(ResourceID::CreepFront));
    creep.setTextureRect(sf::IntRect(0, 0, CREEP_SPRITE_SIZE.x, CREEP_SPRITE_SIZE.y));
}

void ExitStage::initTexts()
{
    bombermanStatus.setFont(fonts.getResource(ResourceID::Font));
    bombermanStatus.setFillColor(sf::Color{239, 209, 58});
    bombermanStatus.setCharacterSize(32);

    creepStatus.setFont(fonts.getResource(ResourceID::Font));
    creepStatus.setFillColor(sf::Color{239, 209, 58});
    creepStatus.setCharacterSize(32);

    playAgain.setFont(fonts.getResource(ResourceID::Font));
    playAgain.setFillColor(sf::Color{239, 209, 58});
    playAgain.setCharacterSize(48);
    playAgain.setString("Play again");

    exitGame.setFont(fonts.getResource(ResourceID::Font));
    exitGame.setFillColor(sf::Color{239, 209, 58});
    exitGame.setCharacterSize(48);
    exitGame.setString("Exit game");
}

void ExitStage::initLayout()
{
    bomberman.setPosition(576 * 1 / 3.f - 32, 576 * 1 / 3.f - 128);
    creep.setPosition(576 * 2 / 3.f - 32, 576 * 1 / 3.f - 64);

    const sf::Vector2f bombermanPosition{bomberman.getPosition().x,
                                         bomberman.getPosition().y +
                                             static_cast<float>(bomberman.getTexture()->getSize().y) + 24.f};
    bombermanStatus.setPosition(bombermanPosition);

    const sf::Vector2f creepPostion{creep.getPosition().x,
                                    creep.getPosition().y + static_cast<float>(creep.getTexture()->getSize().y) + 24.f};
    creepStatus.setPosition(creepPostion);

    playAgain.setPosition((static_cast<float>(EXIT_WINDOW_SIZE.y) - playAgain.getGlobalBounds().width) / 2, 340);
    exitGame.setPosition((static_cast<float>(EXIT_WINDOW_SIZE.x) - exitGame.getGlobalBounds().width) / 2, 340 + 64);
}

void ExitStage::repaintOptions()
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

void ExitStage::changeStage() const
{
    switch (selectedOption)
    {
    case ExitStage::Option::PlayAgain:
        gameManager.changeToGame();
        break;
    case ExitStage::Option::ExitGame:
        gameManager.getWindow().close();
        break;
    }
}