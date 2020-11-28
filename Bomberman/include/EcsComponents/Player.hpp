#pragma once

#include <entityx/entityx.h>

struct Player 
{
    int bombsNum{ 5 };
    int bombsRange{ 5 };
    int health{ 3 };
    entityx::TimeDelta immortalTime{};
};

struct Bomberman
{};

struct Creep
{};