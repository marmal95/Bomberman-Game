#include "GameplayStage.hpp"
#include "ResourceManager.hpp"

GameplayStage::GameplayStage()
{
    resizeWindow();
    loadResources();
    ecsGameplay = std::make_unique<EcsGameplay>(std::ref(*this));
}

bool GameplayStage::update(const entityx::TimeDelta dt)
{
    return ecsGameplay->update(dt);
}

void GameplayStage::draw(sf::RenderWindow& window)
{
    ecsGameplay->draw(window);
}

void GameplayStage::handleEvent(sf::Event&)
{
}

const sf::Texture& GameplayStage::getTexture(const ResourceID resource) const
{
    return textures.getResource(resource);
}

void GameplayStage::resizeWindow() const
{
    ResourcesManager::getInstance().window.setSize(sf::Vector2u(800, 800));
    ResourcesManager::getInstance().window.setView(sf::View(sf::FloatRect(0, 0, 800, 800)));
}

void GameplayStage::loadResources()
{
    textures.load(ResourceID::BackgroundTile, "resources/Blocks/BackgroundTile.png");
    textures.load(ResourceID::ExplodableBlock, "resources/Blocks/ExplodableBlock.png");
    textures.load(ResourceID::SolidBlock, "resources/Blocks/SolidBlock.png");
    textures.load(ResourceID::BombermanBack, "resources/Bomberman/Back/Bomberman_Back.png");
    textures.load(ResourceID::BombermanFront, "resources/Bomberman/Front/Bomberman_Front.png");
    textures.load(ResourceID::BombermanLeft, "resources/Bomberman/Side/Bomberman_Left.png");
    textures.load(ResourceID::BombermanRight, "resources/Bomberman/Side/Bomberman_Right.png");
}
