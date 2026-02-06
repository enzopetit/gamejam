#pragma once

#include <SFML/Graphics.hpp>

struct Bullet {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool alive = true;
};

struct Enemy {
    sf::CircleShape shape;
    float hp;
    float maxHp;
    float speed;
    float timeDrop;
    bool alive = true;
    float flashTimer = 0.0f;
    sf::Color baseColor;
};

struct Particle {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    bool alive = true;
};

struct TimeDrop {
    sf::CircleShape shape;
    float value;
    float lifetime = 3.0f;
    bool alive = true;
};
