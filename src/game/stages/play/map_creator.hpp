#pragma once

#include "game/stages/play/components/map.hpp"
#include "util/types.hpp"
#include <SFML/System/Vector2.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>
#include <vector>

class MapCreator
{
  public:
    MapCreator(entt::registry&, entt::dispatcher&);
    void createMap();

  private:
    void createSolidBlocks();
    void createPortals();
    void createExplodableBlocks();

    entt::registry& registry;
    entt::dispatcher& dispatcher;
    std::vector<TileIndex> blankTilesIndexes;
    Map map;
};