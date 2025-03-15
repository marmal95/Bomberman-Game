#include "game/stages/stage.hpp"
#include <SFML/Window/Event.hpp>

Stage::Stage(GameManager& gameManager) : gameManager{gameManager}
{
}

bool Stage::isKeyReleased(const sf::Event& event, const sf::Keyboard::Key key) const
{
    return event.key.code == key && event.type == sf::Event::KeyReleased;
}