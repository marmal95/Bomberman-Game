#pragma once

#include "enums/resource_id.hpp"
#include "game/stages/stage.hpp"
#include "util/resource_holder.hpp"
#include <SFML/Graphics/CircleShape.hpp>
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

class MenuStage : public Stage
{
  public:
    explicit MenuStage(GameManager& gameManager);
    void handleEvent(const sf::Event&) override;
    bool update(const TimeDelta) override;
    void render(sf::RenderTarget&) override;

  private:
    enum class Option
    {
        PlayAgain,
        ExitGame
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
    sf::Sprite title;
    sf::Text playAgain;
    sf::Text exitGame;
    Option selectedOption;
};