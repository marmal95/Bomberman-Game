#pragma once

#include <util/types.hpp>

struct Player
{
    int bombsNum{5};
    int bombsRange{1};
    int health{3};
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