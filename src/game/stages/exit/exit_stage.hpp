#pragma once

#include "enums/game_result.hpp"
#include "enums/resource_id.hpp"
#include "game/stages/play/resource_holder.hpp"
#include "game/stages/stage.hpp"
#include <SFML/Graphics/CircleShape.hpp>
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
    void changeStage() const;

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

    ResourceHolder<sf::Texture, ResourceID> textures;
    ResourceHolder<sf::Font, ResourceID> fonts;
    sf::Sprite background;
    sf::Sprite bomberman;
    sf::Sprite creep;
    sf::Text bombermanStatus;
    sf::Text creepStatus;
    sf::Text playAgain;
    sf::Text exitGame;
    Option selectedOption;
};