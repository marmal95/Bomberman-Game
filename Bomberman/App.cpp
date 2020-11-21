#include "App.hpp"
#include "ResourceManager.hpp"
#include <SFML/Window/Event.hpp>


void App::run()
{
    auto& window = ResourcesManager::get_instance().window;

    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event{};

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.display();
    }
}