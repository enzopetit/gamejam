#pragma once

#include <SFML/Graphics.hpp>

enum class EnemyType { Caterpillar, Wasp, Beetle, Ant, Spider, Boss };

struct Enemy {
    sf::CircleShape shape;
    float hp;
    float maxHp;
    float speed;
    float timeDrop;
    bool alive = true;
    float flashTimer = 0.0f;
    sf::Color baseColor;
    EnemyType type = EnemyType::Caterpillar;
    float spawnAge = 0.0f;
    sf::Vector2f direction = {0.0f, 0.0f};
    float shootTimer = 0.0f;
    float phaseTimer = 0.0f;
};

