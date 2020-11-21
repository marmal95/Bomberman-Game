#include "App.hpp"
#include "ResourceManager.hpp"
#include "GameStage.hpp"
#include <SFML/Window/Event.hpp>


App::App()
{
    GameStage::changeStage(std::make_unique<MenuStage>());
}

void App::run()
{
    auto& window = ResourcesManager::getInstance().window;

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event event{};

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (auto& stage = GameStage::getStage(); stage != nullptr)
                stage->handleEvent(event);
        }

        const double dt = clock.restart().asSeconds();
        window.clear();
        if (GameStage::run(dt, window))
            window.display();
        else
        {
            window.close();
            break;
        }
    }
}