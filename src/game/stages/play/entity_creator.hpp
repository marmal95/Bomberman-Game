#pragma once

#include "enums/power_up_type.hpp"
#include "enums/resource_id.hpp"
#include "game/stages/play/resource_holder.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <entt/entity/registry.hpp>
#include <random>

struct SpawnTileEvent;
struct SpawnFlameEvent;
struct SpawnPowerUpEvent;
struct SpawnPortalEvent;
struct SpawnBombEvent;

namespace config
{
struct Config;
}

class EntityCreator
{
  public:
    EntityCreator(entt::registry& registry, const ResourceHolder<sf::Texture, ResourceID>& textures)
        : registry{registry}, textures{textures}, generator{}
    {
    }

    entt::entity createBomberman(const config::Config& config);
    entt::entity createCreep(const config::Config& config);
    entt::entity createEmptyTile(const SpawnTileEvent& event);
    entt::entity createSolidBlock(const SpawnTileEvent& event);
    entt::entity createExplodableBlock(const SpawnTileEvent& event);
    entt::entity createFinishAnimationBlock(const SpawnTileEvent& event);
    entt::entity createFlame(const SpawnFlameEvent& event);
    void createPowerUp(const SpawnPowerUpEvent& event);
    entt::entity createPortal(const SpawnPortalEvent& event) const;
    entt::entity createBomb(const SpawnBombEvent& event) const;

  private:
    const sf::Texture& getPowerUpTexture(const PowerUpType type) const;

    entt::registry& registry;
    const ResourceHolder<sf::Texture, ResourceID>& textures;
    std::mt19937 generator;
};