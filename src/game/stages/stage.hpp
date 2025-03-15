#pragma once

#include "util/types.hpp"
#include <SFML/Window/Keyboard.hpp>

namespace sf
{
class RenderTarget;
class Event;
} // namespace sf

class GameManager;

class Stage
{
  public:
    explicit Stage(GameManager&);
    virtual ~Stage() = default;
    virtual void handleEvent(const sf::Event&) = 0;
    virtual bool update(const TimeDelta) = 0;
    virtual void render(sf::RenderTarget&) = 0;

  protected:
    bool isKeyReleased(const sf::Event&, const sf::Keyboard::Key) const;

    GameManager& gameManager;
};