#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include "MenuStage.hpp"
#include "GameplayStage.hpp"
#include "ExitStage.hpp"

class ResourcesManager
{
public:
    sf::RenderWindow window;

    static ResourcesManager& getInstance()
    {
        static ResourcesManager instance{};
        return instance;
    }

private:
    ResourcesManager() : window(sf::VideoMode(800, 800), "Bomberman", sf::Style::Titlebar | sf::Style::Close)
    {
        window.setVerticalSyncEnabled(true);
    }

    ResourcesManager(const ResourcesManager&) = delete;
    ResourcesManager(ResourcesManager&&) = delete;
    ResourcesManager& operator=(const ResourcesManager&) = delete;
    ResourcesManager& operator=(ResourcesManager&&) = delete;
    ~ResourcesManager() = default;
};