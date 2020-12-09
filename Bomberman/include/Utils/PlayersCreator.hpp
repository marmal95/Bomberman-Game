#pragma once

#include <entityx/entityx.h>
#include "ResourceHolder.hpp"
#include "ResourceID.hpp"
#include <SFML/Graphics/Texture.hpp>

class PlayersCreator
{
public:
    PlayersCreator(const ResourceHolder<sf::Texture, ResourceID>&, entityx::EntityManager&, entityx::EventManager&);
    void createBomberman();
    void createCreep();

private:
    const ResourceHolder<sf::Texture, ResourceID>& textures;
    entityx::EntityManager& entityManager;
    entityx::EventManager& eventManager;
};