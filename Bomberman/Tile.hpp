#pragma once

enum class TileType
{
    None,
    SolidBlock,
    ExplodableBlock
};

struct Tile
{
    TileType tileType;
};