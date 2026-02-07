#pragma once

#include <SFML/Graphics.hpp>

struct Particle {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    bool alive = true;
};

