#pragma once

#include "ResourceHolder.hpp"
#include "ResourceID.hpp"
#include <SFML/Graphics/Image.hpp>

class App
{
public:
    App();
    void run();

private:
    ResourceHolder<sf::Image, ResourceID> textures;
};