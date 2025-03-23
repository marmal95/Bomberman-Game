#include "game/stages/settings/settings_stage.hpp"
#include "game/game_manager.hpp"
#include "game/stages/play/config.hpp"
#include "util/constants.hpp"
#include "util/converters.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <algorithm>
#include <format>
#include <ranges>
#include <utility>

namespace
{
constexpr auto FIRST_ITEM_POSITION_Y = 300.f;
constexpr auto ITEM_HEIGHT = 64;

template <typename T>
auto format(const SettingsItem<T>& item)
{
    if constexpr (std::is_same_v<T, bool>)
    {
        return std::format("{} - {}", item.name, item.value ? "enabled" : "disabled");
    }
    return std::format("{} - {}", item.name, item.value);
}

}

SettingsStage::SettingsStage(GameManager& gameManager)
    : Stage{gameManager},
      config{config::parse()},
      textures{},
      fonts{},
      background{},
      title{},
      settings{},
      selectedIndex{}
{
    resizeWindow();
    loadResources();
    initSprites();
    initTexts();
    initLayout();
}

void SettingsStage::handleEvent(const sf::Event& event)
{
    if (isKeyReleased(event, sf::Keyboard::Down))
    {
        if (std::cmp_less(selectedIndex, settings.size() - 1))
        {
            ++selectedIndex;
        }
    }
    else if (isKeyReleased(event, sf::Keyboard::Up))
    {
        if (selectedIndex > 0)
        {
            --selectedIndex;
        }
    }
    if (isKeyReleased(event, sf::Keyboard::Right))
    {
        settings[selectedIndex].value = true;
        settings[selectedIndex].text.setString(format(settings[selectedIndex]));
    }
    if (isKeyReleased(event, sf::Keyboard::Left))
    {
        settings[selectedIndex].value = false;
        settings[selectedIndex].text.setString(format(settings[selectedIndex]));
    }
    if (isKeyReleased(event, sf::Keyboard::Escape))
    {
        saveConfiguration();
        gameManager.changeToMenu();
    }
}

bool SettingsStage::update(const TimeDelta)
{
    repaintOptions();
    return true;
}

void SettingsStage::render(sf::RenderTarget& target)
{

    target.setView(sf::View(sf::FloatRect(sf::Vector2f{}, toVector2f(SETTINGS_WINDOW_SIZE))));
    target.draw(background);
    target.draw(title);

    const auto beginIndex =
        std::max<int>(0, std::cmp_equal(selectedIndex, settings.size()) ? settings.size() - 2 : selectedIndex - 1);
    const auto endIndex = std::min<int>(selectedIndex == 0 ? 2 : selectedIndex + 1, settings.size() - 1);

    for (auto i = beginIndex; i <= endIndex; i++)
    {
        settings[i].text.setPosition((SETTINGS_WINDOW_SIZE.x - settings[i].text.getGlobalBounds().width) / 2,
                                     FIRST_ITEM_POSITION_Y + (i - beginIndex) * ITEM_HEIGHT);
        target.draw(settings[i].text);
    }
}

void SettingsStage::resizeWindow() const
{
    const auto desktopMode = sf::VideoMode::getDesktopMode();
    auto& window = gameManager.getWindow();
    window.setSize(SETTINGS_WINDOW_SIZE);
    window.setView(sf::View(sf::FloatRect(sf::Vector2f{}, toVector2f(SETTINGS_WINDOW_SIZE))));
    window.setPosition({static_cast<int>(desktopMode.width - window.getSize().x) / 2,
                        static_cast<int>(desktopMode.height - window.getSize().y) / 2});
}

void SettingsStage::loadResources()
{
    textures.load(ResourceID::MenuBackground, "resources/Menu/title_background.jpg");
    textures.load(ResourceID::MenuTitle, "resources/Menu/title_titletext.png");
}

void SettingsStage::initSprites()
{
    background.setTexture(textures.getResource(ResourceID::MenuBackground));
    title.setTexture(textures.getResource(ResourceID::MenuTitle));
    fonts.load(ResourceID::Font, "resources/Font/AmazDooMLeft.ttf");
}

void SettingsStage::initTexts()
{
    {
        SettingsItem<bool> playersCollidable{.text = sf::Text{},
                                             .name = "Players collisions",
                                             .key = config::keys::PLAYERS_COLLIDABLE,
                                             .value = config.playersCollidable};
        playersCollidable.text.setFont(fonts.getResource(ResourceID::Font));
        playersCollidable.text.setCharacterSize(48);
        playersCollidable.text.setString(format(playersCollidable));
        settings.push_back(std::move(playersCollidable));
    }

    {
        SettingsItem<bool> playersCollidable{.text = sf::Text{},
                                             .name = "Chain explosions",
                                             .key = config::keys::CHAIN_EXPLOSION,
                                             .value = config.playersCollidable};
        playersCollidable.text.setFont(fonts.getResource(ResourceID::Font));
        playersCollidable.text.setCharacterSize(48);
        playersCollidable.text.setString(format(playersCollidable));
        settings.push_back(std::move(playersCollidable));
    }
}

void SettingsStage::initLayout()
{
}

void SettingsStage::repaintOptions()
{
    for (auto [index, value] : std::views::zip(std::views::iota(0), settings))
    {
        value.text.setFillColor(index == selectedIndex ? color::SELECTED_MENU_ITEM : color::MENU_ITEM);
    }
}

void SettingsStage::saveConfiguration() const
{
    const auto findSettingsItemValue = [&](auto&& key, auto proj) {
        return std::ranges::find(settings, key, proj)->value;
    };

    const config::Config newConfig{
        .playersCollidable = findSettingsItemValue(config::keys::PLAYERS_COLLIDABLE, &SettingsItem<bool>::key),
        .chainExplosion = findSettingsItemValue(config::keys::CHAIN_EXPLOSION, &SettingsItem<bool>::key)};

    config::save(newConfig);
}
