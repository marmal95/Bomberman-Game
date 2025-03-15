#pragma once

#include <SFML/Graphics/Sprite.hpp>

struct Drawable
{
    explicit Drawable(const sf::Texture& texture) : sprite{}
    {
        sprite.setTexture(texture);
    }

    sf::Sprite sprite;
};
