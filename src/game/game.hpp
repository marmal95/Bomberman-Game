#pragma once

#include "game_manager.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

class Game
{
  public:
    Game();
    void run();

  private:
    void update();
    void pollEvents();
    void render();

    sf::Clock clock;
    sf::RenderWindow window;
    GameManager gameManager;
};