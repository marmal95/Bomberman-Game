#pragma once

#include "enums/resource_id.hpp"
#include "game/stages/play/config.hpp"
#include "game/stages/stage.hpp"
#include "util/resource_holder.hpp"
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

namespace sf
{
class RenderTarget;
class Event;
} // namespace sf

class StageManager;
class GameManager;

template <typename T>
struct SettingsItem
{
    sf::Text text;
    std::string name;
    config::Key key;
    T value;
};

class SettingsStage : public Stage
{
  public:
    explicit SettingsStage(GameManager& gameManager);
    void handleEvent(const sf::Event&) override;
    bool update(const TimeDelta) override;
    void render(sf::RenderTarget&) override;

  private:
    void resizeWindow() const;
    void loadResources();
    void initSprites();
    void initTexts();
    void initLayout();
    void repaintOptions();
    void saveConfiguration() const;

    config::Config config;
    ResourceHolder<sf::Texture, ResourceID> textures;
    ResourceHolder<sf::Font, ResourceID> fonts;
    sf::Sprite background;
    sf::Sprite title;

    std::vector<SettingsItem<bool>> settings;
    int selectedIndex;
};