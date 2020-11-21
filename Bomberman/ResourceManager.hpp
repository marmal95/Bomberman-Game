#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include "MenuStage.hpp"
#include "GameplayStage.hpp"
#include "ExitStage.hpp"

enum class Font
{
    MainFont
};

class ResourcesManager
{
public:
    sf::RenderWindow window;
    MenuStage menuStage;
    GameplayStage gameplayStage;
    ExitStage exitStage;

    static ResourcesManager& get_instance()
    {
        static ResourcesManager instance{};
        return instance;
    }

private:
    ResourcesManager() : window(sf::VideoMode(800, 800), "Bomberman", sf::Style::Titlebar | sf::Style::Close)
    {}

    ResourcesManager(const ResourcesManager&) = delete;
    ResourcesManager(ResourcesManager&&) = delete;
    ResourcesManager& operator=(const ResourcesManager&) = delete;
    ResourcesManager& operator=(ResourcesManager&&) = delete;
    ~ResourcesManager() = default;
};