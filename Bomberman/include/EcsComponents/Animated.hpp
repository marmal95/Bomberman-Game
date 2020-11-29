#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <vector>

struct Animated
{
    Animated(const sf::Vector2i& frameSize, const uint32_t framesNumber, const float speed)
        : frames{}, frame{}, speed{ speed }, paused{ false }
    {
        frames.reserve(framesNumber);

        for (uint32_t i = 0; i < framesNumber; i++)
            frames.emplace_back(sf::IntRect(i * frameSize.x, 0, frameSize.x, frameSize.y));
    }


	std::vector<sf::IntRect> frames;
	float frame;
	float speed;
	bool paused;
};