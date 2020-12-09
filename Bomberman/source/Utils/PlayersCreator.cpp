#include "PlayersCreator.hpp"
#include "Drawable.hpp"
#include "Transformable.hpp"
#include "Movable.hpp"
#include "Animated.hpp"
#include "Collidable.hpp"
#include "Player.hpp"
#include "MoveChangeEvent.hpp"
#include "Constants.hpp"
#include "ResourceID.hpp"
#include "Utils.hpp"

PlayersCreator::PlayersCreator(
    const ResourceHolder<sf::Texture, ResourceID>& textures,
    entityx::EntityManager& entityManager,
    entityx::EventManager& eventManager)
    : textures{ textures }, entityManager{ entityManager }, eventManager{ eventManager }
{}

void PlayersCreator::createBomberman()
{
    auto entity = entityManager.create();
    entity.assign<Drawable>(textures.getResource(ResourceID::BombermanFront));
    entity.assign<Transformable>(Transformable{ toVector2f(BOMBERMAN_SIZE), calculatePositionInTileCenter({ 1, 1 }, BOMBERMAN_SIZE) });
    entity.assign<Movable>(Movable{ toVector2f(PLAYER_INITIAL_SPEED), Direction::None });
    entity.assign<Animated>(Animated{ { BOMBERMAN_SPRITE_SIZE.x, BOMBERMAN_SPRITE_SIZE.y }, 8, PLAYER_INITIAL_ANIM_SPEED });
    entity.assign<Collidable>();
    entity.assign<Player>();
    entity.assign<Bomberman>();
    eventManager.emit<MoveChangeEvent>({ entity, Direction::None });
}

void PlayersCreator::createCreep()
{
    auto entity = entityManager.create();
    entity.assign<Drawable>(textures.getResource(ResourceID::CreepFront));
    entity.assign<Transformable>(Transformable{ toVector2f(CREEP_SIZE), calculatePositionInTileCenter({ 19, 19 }, CREEP_SIZE) });
    entity.assign<Movable>(Movable{ toVector2f(PLAYER_INITIAL_SPEED), Direction::None });
    entity.assign<Animated>(Animated{ { CREEP_SPRITE_SIZE.x, CREEP_SPRITE_SIZE.y }, 6, PLAYER_INITIAL_ANIM_SPEED });
    entity.assign<Collidable>();
    entity.assign<Player>();
    entity.assign<Creep>();
    eventManager.emit<MoveChangeEvent>({ entity, Direction::None });
}
