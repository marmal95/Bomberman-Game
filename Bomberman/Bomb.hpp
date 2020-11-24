#pragma once

struct Bomb 
{
    entityx::Entity spawner;
    entityx::TimeDelta timeToExplode;
    int range;
};