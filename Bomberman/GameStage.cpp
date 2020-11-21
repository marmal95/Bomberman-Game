#include "GameStage.hpp"

std::unique_ptr<GameStage> GameStage::activeStage = nullptr;

bool GameStage::run(const entityx::TimeDelta dt, sf::RenderWindow& window)
{
    if (activeStage)
    {
        if (activeStage->update(dt))
            activeStage->draw(window);
        else
        {
            activeStage = nullptr;
            return false;
        }
        return true;
    }

    return false;
}

std::unique_ptr<GameStage>& GameStage::getStage()
{
    return activeStage;
}

void GameStage::changeStage(std::unique_ptr<GameStage> nextStage)
{
    activeStage = std::move(nextStage);
}