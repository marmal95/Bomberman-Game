#include "ExitStage.hpp"
#include "ResourceManager.hpp"
#include "Constants.hpp"

ExitStage::ExitStage(const GameStatus gameResult)
    : textures{}, fonts{}, background{}, bomberman{}, creep{}, bombermanStatus{}, creepStatus{}, playAgain{}, exitGame{}, selectedOption{ Option::PlayAgain }
{
    resizeWindow();
    loadResources();
    initSprites();
    initTexts();
    initLayout();

    bombermanStatus.setString((gameResult == GameStatus::BombermanWon || gameResult == GameStatus::Draw) ? "Winner" : "Loser");
    creepStatus.setString((gameResult == GameStatus::CreepWon || gameResult == GameStatus::Draw) ? "Winner" : "Loser");
}

bool ExitStage::update(const entityx::TimeDelta)
{
    repaintOptions();
    return true;
}

void ExitStage::draw(sf::RenderWindow& window)
{
    window.draw(background);
    window.draw(bomberman);
    window.draw(creep);
    window.draw(bombermanStatus);
    window.draw(creepStatus);
    window.draw(playAgain);
    window.draw(exitGame);
}

void ExitStage::handleEvent(sf::Event& event)
{
    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Down)
        selectedOption = Option::ExitGame;
    else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Up)
        selectedOption = Option::PlayAgain;
    else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space)
        changeStage();
}

void ExitStage::resizeWindow() const
{
    const auto desktopMode = sf::VideoMode::getDesktopMode();
    auto& window = ResourcesManager::getInstance().window;
    window.setSize(EXIT_WINDOW_SIZE);
    window.setView(sf::View(sf::FloatRect(0, 0, EXIT_WINDOW_SIZE.x, EXIT_WINDOW_SIZE.y)));
    window.setPosition({ static_cast<int>(desktopMode.width - window.getSize().x) / 2,
                         static_cast<int>(desktopMode.height - window.getSize().y) / 2 });
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
    bombermanStatus.setFillColor(sf::Color{ 239, 209, 58 });
    bombermanStatus.setCharacterSize(32);

    creepStatus.setFont(fonts.getResource(ResourceID::Font));
    creepStatus.setFillColor(sf::Color{ 239, 209, 58 });
    creepStatus.setCharacterSize(32);

    playAgain.setFont(fonts.getResource(ResourceID::Font));
    playAgain.setFillColor(sf::Color{ 239, 209, 58 });
    playAgain.setCharacterSize(48);
    playAgain.setString("Play again");

    exitGame.setFont(fonts.getResource(ResourceID::Font));
    exitGame.setFillColor(sf::Color{ 239, 209, 58 });
    exitGame.setCharacterSize(48);
    exitGame.setString("Exit game");
}

void ExitStage::initLayout()
{
    bomberman.setPosition(576 * 1 / 3.f - 32, 576 * 1 / 3.f - 128);
    creep.setPosition(576 * 2 / 3.f - 32, 576 * 1 / 3.f - 64);

    bombermanStatus.setPosition(bomberman.getPosition().x, bomberman.getPosition().y + bomberman.getTexture()->getSize().y + 24);
    creepStatus.setPosition(creep.getPosition().x, creep.getPosition().y + creep.getTexture()->getSize().y + 24);

    playAgain.setPosition((EXIT_WINDOW_SIZE.y - playAgain.getGlobalBounds().width) / 2, 340);
    exitGame.setPosition((EXIT_WINDOW_SIZE.x - exitGame.getGlobalBounds().width) / 2, 340 + 64);
}

void ExitStage::repaintOptions()
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

void ExitStage::changeStage() const
{
    if (selectedOption == Option::PlayAgain)
        GameStage::changeStage(std::make_unique<GameplayStage>());
    else if (selectedOption == Option::ExitGame)
        ResourcesManager::getInstance().window.close();
}