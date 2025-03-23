#include "game/stages/exit/exit_stage.hpp"
#include "game/game_manager.hpp"
#include "util/constants.hpp"
#include "util/converters.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <ranges>

namespace
{
constexpr auto FIRST_ITEM_POSITION_Y = 300.f;
constexpr auto ITEM_HEIGHT = 64;
}

ExitStage::ExitStage(GameManager& gameManager, const GameResult gameResult)
    : Stage{gameManager},
      textures{},
      fonts{},
      background{},
      bomberman{},
      creep{},
      bombermanStatus{},
      creepStatus{},
      items{},
      selectedIndex{}
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
        selectedIndex = std::min<int>(selectedIndex + 1, items.size() - 1);
    }
    else if (isKeyReleased(event, sf::Keyboard::Up))
    {
        selectedIndex = std::max(selectedIndex - 1, 0);
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

    for (const auto& item : items)
    {
        target.draw(item.text);
    }
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
    bombermanStatus.setFillColor(color::MENU_ITEM);
    bombermanStatus.setCharacterSize(32);

    creepStatus.setFont(fonts.getResource(ResourceID::Font));
    creepStatus.setFillColor(color::MENU_ITEM);
    creepStatus.setCharacterSize(32);

    {
        sf::Text settings;
        settings.setFont(fonts.getResource(ResourceID::Font));
        settings.setCharacterSize(48);
        settings.setString("Play again");
        items.emplace_back(std::move(settings), MenuOption::Start);
    }

    {
        sf::Text settings;
        settings.setFont(fonts.getResource(ResourceID::Font));
        settings.setCharacterSize(48);
        settings.setString("Exit");
        items.emplace_back(std::move(settings), MenuOption::Exit);
    }
}

void ExitStage::initLayout()
{
    bomberman.setPosition(EXIT_WINDOW_SIZE.x * 1 / 3.f - BOMBERMAN_SPRITE_SIZE.x / 2.f,
                          EXIT_WINDOW_SIZE.y * 1 / 3.f - BOMBERMAN_SPRITE_SIZE.y);
    creep.setPosition(EXIT_WINDOW_SIZE.x * 2 / 3.f - CREEP_SPRITE_SIZE.x,
                      EXIT_WINDOW_SIZE.y * 1 / 3.f - CREEP_SPRITE_SIZE.y);

    constexpr auto playersToStatusMargin = 24.f;
    const sf::Vector2f bombermanStatusPosition{
        bomberman.getPosition().x,
        bomberman.getPosition().y + static_cast<float>(bomberman.getTexture()->getSize().y) + playersToStatusMargin};
    bombermanStatus.setPosition(bombermanStatusPosition);

    const sf::Vector2f creepStatusPosition{creep.getPosition().x,
                                           creep.getPosition().y + static_cast<float>(creep.getTexture()->getSize().y) +
                                               playersToStatusMargin};
    creepStatus.setPosition(creepStatusPosition);

    for (auto [index, value] : std::views::zip(std::views::iota(0), items)) // TODO: C++23 std::views::enumerate
    {
        value.text.setPosition((MENU_WINDOW_SIZE.x - value.text.getGlobalBounds().width) / 2,
                               FIRST_ITEM_POSITION_Y + index * ITEM_HEIGHT);
    }
}

void ExitStage::repaintOptions()
{
    for (auto [index, value] : std::views::zip(std::views::iota(0), items))
    {
        value.text.setFillColor(index == selectedIndex ? color::SELECTED_MENU_ITEM : color::MENU_ITEM);
    }
}

void ExitStage::changeStage() const
{
    switch (items[selectedIndex].option)
    {
    case MenuOption::Start:
        gameManager.changeToGame();
        break;
    case MenuOption::Exit:
        gameManager.getWindow().close();
        break;
    }
}