#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <vector>

struct Animated
{
	Animated(const uint32_t frameWidth, const uint32_t frameHeight, const uint32_t framesNumber, const float speed);

	std::vector<sf::IntRect> frames;
	float frame;
	float speed;
	bool paused;
};