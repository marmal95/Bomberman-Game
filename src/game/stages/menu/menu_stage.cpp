#include "game/stages/menu/menu_stage.hpp"
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

MenuStage::MenuStage(GameManager& gameManager)
    : Stage{gameManager}, textures{}, fonts{}, background{}, title{}, items{}, selectedIndex{}
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
        selectedIndex = std::min<int>(selectedIndex + 1, items.size() - 1);
    }
    if (isKeyReleased(event, sf::Keyboard::Up))
    {
        selectedIndex = std::max(selectedIndex - 1, 0);
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

    for (const auto& item : items)
    {
        target.draw(item.text);
    }
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
    {
        sf::Text startGame;
        startGame.setFont(fonts.getResource(ResourceID::Font));
        startGame.setCharacterSize(48);
        startGame.setString("Play again");
        items.emplace_back(std::move(startGame), MenuOption::Start);
    }

    {
        sf::Text settings;
        settings.setFont(fonts.getResource(ResourceID::Font));
        settings.setCharacterSize(48);
        settings.setString("Settings");
        items.emplace_back(std::move(settings), MenuOption::Settings);
    }

    {
        sf::Text exitGame;
        exitGame.setFont(fonts.getResource(ResourceID::Font));
        exitGame.setCharacterSize(48);
        exitGame.setString("Exit game");
        items.emplace_back(std::move(exitGame), MenuOption::Exit);
    }
}

void MenuStage::initLayout()
{
    for (auto [index, value] : std::views::zip(std::views::iota(0), items)) // TODO: C++23 std::views::enumerate
    {
        value.text.setPosition((MENU_WINDOW_SIZE.x - value.text.getGlobalBounds().width) / 2,
                               FIRST_ITEM_POSITION_Y + index * ITEM_HEIGHT);
    }
}

void MenuStage::repaintOptions()
{
    for (auto [index, value] : std::views::zip(std::views::iota(0), items))
    {
        value.text.setFillColor(index == selectedIndex ? color::SELECTED_MENU_ITEM : color::MENU_ITEM);
    }
}

void MenuStage::changeStage() const
{
    switch (items[selectedIndex].option)
    {
    case MenuOption::Start:
        gameManager.changeToGame();
        break;
    case MenuOption::Settings:
        gameManager.changeToSettings();
        break;
    case MenuOption::Exit:
        gameManager.getWindow().close();
        break;
    }
}