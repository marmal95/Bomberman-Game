#pragma once

#include <entityx/entityx.h>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include "Map.hpp"

class MapCreator
{
public:
    MapCreator(entityx::EntityManager&, entityx::EventManager&);
    void createMap();

private:
    void createSolidBlocks();
    void createPortals();
    void createExplodableBlocks();

    entityx::EntityManager& entityManager;
    entityx::EventManager& eventManager;
    std::vector<sf::Vector2i> blankTilesIndexes;
    Map map;
};