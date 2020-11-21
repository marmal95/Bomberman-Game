#include "GameStage.hpp"

bool GameStage::run(entityx::TimeDelta dt, sf::RenderWindow& window)
{

}

GameStage* GameStage::get_stage()
{
    return active_stage;
}