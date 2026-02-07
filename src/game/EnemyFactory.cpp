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
