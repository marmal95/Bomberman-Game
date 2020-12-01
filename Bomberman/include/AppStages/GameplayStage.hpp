#pragma once

#include "GameStage.hpp"
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"
#include "EcsGameplay.hpp"
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <memory>


class GameplayStage : public GameStage
{
public:
    GameplayStage();
    bool update(const entityx::TimeDelta) override;
    void draw(sf::RenderWindow& window) override;
    void handleEvent(sf::Event&) override;

    const ResourceHolder<sf::Texture, ResourceID>& getTextures() const;
    const ResourceHolder<sf::SoundBuffer, ResourceID>& getSounds() const;

private:
    void resizeWindow() const;
    void loadResources();

    ResourceHolder<sf::Texture, ResourceID> textures;
    ResourceHolder<sf::SoundBuffer, ResourceID> sounds;
    std::unique_ptr<EcsGameplay> ecsGameplay;
};
