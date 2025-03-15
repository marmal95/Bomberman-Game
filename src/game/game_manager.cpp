#include "game_manager.hpp"
#include "game/stages/exit/exit_stage.hpp"
#include "game/stages/menu/menu_stage.hpp"
#include "game/stages/play/play_stage.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

GameManager::GameManager(sf::RenderWindow& window) : window{window}
{
}

void GameManager::handleEvent(const sf::Event& event)
{
    currentStage->handleEvent(event);
}

void GameManager::update(const TimeDelta dt)
{
    currentStage->update(dt);
}

void GameManager::render(sf::RenderTarget& renderTarget) const
{
    currentStage->render(renderTarget);
}

void GameManager::changeToMenu()
{
    changeStage(std::make_unique<MenuStage>(*this));
}

void GameManager::changeToGame()
{
    changeStage(std::make_unique<PlayStage>(*this));
}

void GameManager::changeToExit(const GameResult result)
{
    changeStage(std::make_unique<ExitStage>(*this, result));
}

sf::RenderWindow& GameManager::getWindow()
{
    return window;
}

void GameManager::changeStage(std::unique_ptr<Stage> stage)
{
    currentStage = std::move(stage);
}
