#pragma once

#include <SFML/Graphics.hpp>

struct Bullet {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool alive = true;
    bool piercing = false;
    float damage = 1.0f;
};

