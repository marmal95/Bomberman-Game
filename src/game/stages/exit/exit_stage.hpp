#pragma once

#include "enums/game_result.hpp"
#include "enums/resource_id.hpp"
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

class ExitStage : public Stage
{
  public:
    explicit ExitStage(GameManager& gameManager, const GameResult);
    void handleEvent(const sf::Event&) override;
    bool update(const TimeDelta) override;
    void render(sf::RenderTarget&) override;

  private:
    enum class MenuOption
    {
        Start,
        Exit
    };

    struct MenuItem
    {
        sf::Text text;
        MenuOption option;
    };

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
    sf::Sprite bomberman;
    sf::Sprite creep;
    sf::Text bombermanStatus;
    sf::Text creepStatus;

    std::vector<MenuItem> items;
    int selectedIndex;
};