#pragma once

#include <entityx/entityx.h>

struct Player 
{
    int bombsNum{ 1 };
    int bombsRange{ 1 };
    int health{ 3 };
    entityx::TimeDelta immortalTime{};
};

struct Bomberman
{};

struct Creep
{};