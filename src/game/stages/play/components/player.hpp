#pragma once

#include <util/types.hpp>

struct Player
{
    int bombsNum{1};
    int bombsRange{1};
    int health{1};
    TimeDelta immortalTime{};
};

struct Bomberman
{
    bool tag{};
};

struct Creep
{
    bool tag{};
};