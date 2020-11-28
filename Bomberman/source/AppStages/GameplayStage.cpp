#include "GameplayStage.hpp"
#include "ResourceManager.hpp"
#include "Constants.hpp"

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

void GameplayStage::handleEvent(sf::Event& event)
{
    ecsGameplay->handleEvent(event);
}

const ResourceHolder<sf::Texture, ResourceID>& GameplayStage::getTextures() const
{
    return textures;
}

void GameplayStage::resizeWindow() const
{
    const auto desktopMode = sf::VideoMode::getDesktopMode();
    auto& window = ResourcesManager::getInstance().window;
    window.setSize(sf::Vector2u(WIDTH_TILES_NUM * 64, HEIGHT_TILES_NUM * 64));
    window.setView(sf::View(sf::FloatRect(0, 0, WIDTH_TILES_NUM * 64, HEIGHT_TILES_NUM * 64)));
    window.setPosition({ static_cast<int>(desktopMode.width - window.getSize().x) / 2, 0 });
}

void GameplayStage::loadResources()
{
    textures.load(ResourceID::BackgroundTile, "resources/Blocks/BackgroundTile.png");
    textures.load(ResourceID::ExplodableBlock, "resources/Blocks/ExplodableBlock.png");
    textures.load(ResourceID::SolidBlock, "resources/Blocks/SolidBlock.png");
    textures.load(ResourceID::SolidRedBlock, "resources/Blocks/SolidRedBlock.png");
    textures.load(ResourceID::BombermanBack, "resources/Bomberman/Bomberman_Back.png");
    textures.load(ResourceID::BombermanFront, "resources/Bomberman/Bomberman_Front.png");
    textures.load(ResourceID::BombermanLeft, "resources/Bomberman/Bomberman_Left.png");
    textures.load(ResourceID::BombermanRight, "resources/Bomberman/Bomberman_Right.png");
    textures.load(ResourceID::CreepBack, "resources/Creep/Creep_Back.png");
    textures.load(ResourceID::CreepFront, "resources/Creep/Creep_Front.png");
    textures.load(ResourceID::CreepLeft, "resources/Creep/Creep_Left.png");
    textures.load(ResourceID::CreepRight, "resources/Creep/Creep_Right.png");
    textures.load(ResourceID::Bomb, "resources/Bomb/Bomb.png");
    textures.load(ResourceID::Flame, "resources/Flame/Flame.png");
    textures.load(ResourceID::BombPowerUp, "resources/Powerups/BombPowerup.png");
    textures.load(ResourceID::FlamePowerUp, "resources/Powerups/FlamePowerup.png");
    textures.load(ResourceID::SpeedPowerUp, "resources/Powerups/SpeedPowerup.png");
}
