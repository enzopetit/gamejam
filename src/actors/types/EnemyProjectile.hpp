#pragma once

#include <SFML/Graphics.hpp>

struct EnemyProjectile {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float damage;
    bool destructible = false;
    bool alive = true;
};
