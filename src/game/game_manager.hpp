#pragma once

#include "enums/game_result.hpp"
#include "game/stages/stage.hpp"
#include <memory>

namespace sf
{
class RenderTarget;
class RenderWindow;
class Event;
} // namespace sf

class GameManager
{
  public:
    explicit GameManager(sf::RenderWindow& window);
    void handleEvent(const sf::Event&);
    void update(const TimeDelta);
    void render(sf::RenderTarget&) const;

    void changeToMenu();
    void changeToGame();
    void changeToExit(const GameResult result);

    sf::RenderWindow& getWindow();

  private:
    void changeStage(std::unique_ptr<Stage>);

    sf::RenderWindow& window;
    std::unique_ptr<Stage> currentStage;
};