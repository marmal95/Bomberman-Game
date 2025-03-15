#pragma once

#include <SFML/System/Vector2.hpp>
#include <cstdint>

constexpr int WIDTH_TILES_NUM = 21;
constexpr int HEIGHT_TILES_NUM = 21;
constexpr int DISTANCE_TO_EXPLODABLE_BLOCKS = 2;

constexpr float PLAYER_INITIAL_ANIM_SPEED = 1;
constexpr float IMMORTAL_TIME_AFTER_DEAD = 3;

const sf::Vector2i TILE_SIZE = {64, 64};
const sf::Vector2i PORTAL_SPRITE_SIZES = {64, 64};
const sf::Vector2i BOMBERMAN_SPRITE_SIZE = {64, 128};
const sf::Vector2i BOMBERMAN_SIZE = {30, 30};
const sf::Vector2i CREEP_SPRITE_SIZE = {64, 64};
const sf::Vector2i CREEP_SIZE = {30, 30};
const sf::Vector2i BOMB_SPRITE_SIZE = {48, 48};
const sf::Vector2i FLAME_SPRITE_SIZE = {48, 48};
const sf::Vector2i POWER_UP_SPRITE_SIZE = {32, 32};

const sf::Vector2i PLAYER_INITIAL_SPEED = {128, 128};

const sf::Vector2u MENU_WINDOW_SIZE = {576, 576};
const sf::Vector2u EXIT_WINDOW_SIZE = {576, 576};
const sf::Vector2u GAME_WINDOW_SIZE = {static_cast<uint32_t>(WIDTH_TILES_NUM * TILE_SIZE.x),
                                       static_cast<uint32_t>(WIDTH_TILES_NUM* TILE_SIZE.y)};

constexpr int GROUND_Z_INDEX = 1;
constexpr int BLOCKS_Z_INDEX = 2;
constexpr int BOMB_Z_INDEX = 4;
constexpr int FLAME_Z_INDEX = 4;
constexpr int ADDON_Z_INDEX = 4;
constexpr int PLAYERS_Z_INDEX = 5;
constexpr int TOP_LEVEL_Z_INDEX = 10;
