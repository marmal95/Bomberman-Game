#include "game.hpp"
#include "util/constants.hpp"
#include <SFML/Window/Event.hpp>

Game::Game()
    : clock{},
      window{sf::VideoMode{GAME_WINDOW_SIZE.x, GAME_WINDOW_SIZE.y}, "Game Template"},
      gameManager{window}
{
    window.setFramerateLimit(60);
    gameManager.changeToMenu();
}

void Game::run()
{
    while (window.isOpen())
    {
        pollEvents();
        update();
        render();
    }
}

void Game::update()
{
    const auto dt = clock.restart().asSeconds();
    gameManager.update(dt);
}

void Game::pollEvents()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
        else
        {
            gameManager.handleEvent(event);
        }
    }
}

void Game::render()
{
    window.clear();
    gameManager.render(window);
    window.display();
}
