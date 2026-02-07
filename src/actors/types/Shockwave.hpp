#pragma once

#include <SFML/Graphics.hpp>

struct Shockwave {
    sf::Vector2f position;
    float radius;
    float maxRadius;
    float lifetime;
    float maxLifetime;
    sf::Color color;
    bool alive = true;
};

