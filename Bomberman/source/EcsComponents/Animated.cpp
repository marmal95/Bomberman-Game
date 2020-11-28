#include "Animated.hpp"

Animated::Animated(const uint32_t frameWidth, const uint32_t frameHeight, const uint32_t framesNumber, const float speed)
    : frames{}, frame{}, speed{ speed }, paused{ false }
{
    frames.reserve(framesNumber);
    
    for (uint32_t i = 0; i < framesNumber; i++)
        frames.emplace_back(sf::IntRect(i * frameWidth, 0, frameWidth, frameHeight));
}
