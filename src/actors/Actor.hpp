#pragma once

#include <SFML/Graphics.hpp>
#include <string>

struct Bullet {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    bool alive = true;
    bool piercing = false;
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
    float spawnAge = 0.0f;
};

struct Particle {
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    bool alive = true;
};

enum class BonusType {
    Time,
    Speed,
    RapidFire,
    Pierce
};

struct Bonus {
    sf::CircleShape shape;
    float value;
    float lifetime;
    BonusType type;
    bool alive = true;
};

struct Shockwave {
    sf::Vector2f position;
    float radius;
    float maxRadius;
    float lifetime;
    float maxLifetime;
    sf::Color color;
    bool alive = true;
};

struct FloatingText {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float lifetime;
    float maxLifetime;
    std::string text;
    sf::Color color;
    unsigned int size;
    bool alive = true;
};
