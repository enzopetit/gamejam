#include "game/EnemyFactory.hpp"

#include "game/GameConfig.hpp"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>

Enemy createEnemy(EnemyType type, float difficultyTime, int waveIndex) {
    Enemy e;
    e.type = type;
    float side = static_cast<float>(std::rand() % 4);
    float x, y;
    if (side < 1) { x = -30; y = static_cast<float>(std::rand() % WIN_H); }
    else if (side < 2) { x = WIN_W + 30.0f; y = static_cast<float>(std::rand() % WIN_H); }
    else if (side < 3) { x = static_cast<float>(std::rand() % WIN_W); y = -30; }
    else { x = static_cast<float>(std::rand() % WIN_W); y = WIN_H + 30.0f; }

    float speedMult = 1.0f + difficultyTime / 120.0f;

    switch (type) {
        case EnemyType::Caterpillar:
            e.shape = sf::CircleShape(12.0f);
            e.baseColor = sf::Color(100, 200, 100);
            e.hp = 1; e.maxHp = 1;
            e.speed = ENEMY_BASE_SPEED * 1.5f * speedMult;
            e.timeDrop = 1.0f;
            break;
        case EnemyType::Wasp:
            e.shape = sf::CircleShape(18.0f);
            e.baseColor = sf::Color(220, 200, 50);
            e.hp = 3; e.maxHp = 3;
            e.speed = ENEMY_BASE_SPEED * speedMult;
            e.timeDrop = 2.0f;
            break;
        case EnemyType::Beetle:
            e.shape = sf::CircleShape(25.0f);
            e.baseColor = sf::Color(200, 80, 80);
            e.hp = 5; e.maxHp = 5;
            e.speed = ENEMY_BASE_SPEED * 0.6f * speedMult;
            e.timeDrop = 3.0f;
            break;
        case EnemyType::Mosquito:
            e.shape = sf::CircleShape(MOSQUITO_RADIUS);
            e.baseColor = sf::Color(160, 160, 170);
            e.hp = MOSQUITO_HP; e.maxHp = MOSQUITO_HP;
            e.speed = MOSQUITO_SPEED * speedMult;
            e.timeDrop = MOSQUITO_TIME_DROP;
            break;
        case EnemyType::Firefly:
            e.shape = sf::CircleShape(FIREFLY_RADIUS);
            e.baseColor = sf::Color(255, 240, 80);
            e.hp = FIREFLY_HP; e.maxHp = FIREFLY_HP;
            e.speed = FIREFLY_SPEED * speedMult;
            e.timeDrop = FIREFLY_TIME_DROP;
            e.shootTimer = FIREFLY_TELEPORT_MIN + static_cast<float>(std::rand() % 100) / 100.0f * (FIREFLY_TELEPORT_MAX - FIREFLY_TELEPORT_MIN);
            break;
        case EnemyType::Scorpion:
            e.shape = sf::CircleShape(SCORPION_RADIUS);
            e.baseColor = sf::Color(140, 90, 40);
            e.hp = SCORPION_HP; e.maxHp = SCORPION_HP;
            e.speed = SCORPION_SPEED * speedMult;
            e.timeDrop = SCORPION_TIME_DROP;
            e.shootTimer = SCORPION_SHOOT_INTERVAL * 0.5f;
            break;
        case EnemyType::Ladybug:
            e.shape = sf::CircleShape(LADYBUG_RADIUS);
            e.baseColor = sf::Color(220, 40, 40);
            e.hp = LADYBUG_HP; e.maxHp = LADYBUG_HP;
            e.speed = LADYBUG_SPEED * speedMult;
            e.timeDrop = LADYBUG_TIME_DROP;
            break;
        case EnemyType::Boss:
            e.shape = sf::CircleShape(BOSS_RADIUS);
            e.baseColor = sf::Color(170, 60, 190);
            e.hp = BOSS_HP; e.maxHp = BOSS_HP;
            e.speed = BOSS_SPEED * speedMult;
            e.timeDrop = BOSS_TIME_DROP;
            e.shootTimer = BOSS_SHOOT_INTERVAL * 0.6f;
            break;
        default:
            e.shape = sf::CircleShape(12.0f);
            e.baseColor = sf::Color(100, 200, 100);
            e.hp = 1; e.maxHp = 1;
            e.speed = ENEMY_BASE_SPEED * 1.5f * speedMult;
            e.timeDrop = 1.0f;
            break;
    }
    float hpMult = std::pow(1.0f + ENEMY_HP_PER_WAVE, static_cast<float>(waveIndex));
    e.hp *= hpMult;
    e.maxHp *= hpMult;
    e.shape.setFillColor(e.baseColor);
    e.shape.setOrigin({e.shape.getRadius(), e.shape.getRadius()});
    e.shape.setPosition({x, y});
    e.spawnAge = 0.0f;
    return e;
}
