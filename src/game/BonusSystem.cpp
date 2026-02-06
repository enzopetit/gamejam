#include "game/BonusSystem.hpp"

#include "game/GameConfig.hpp"
#include "utils/Math.hpp"

#include <algorithm>
#include <cstdlib>

namespace {
float bonusRadius(BonusType type) {
    if (type == BonusType::Time) return 10.0f;
    if (type == BonusType::Speed) return 12.0f;
    return 12.0f;
}

sf::Color bonusColor(BonusType type) {
    if (type == BonusType::Time) return sf::Color(255, 80, 80);
    if (type == BonusType::Speed) return sf::Color(80, 200, 255);
    return sf::Color(255, 200, 80);
}

sf::Color bonusOutline(BonusType type) {
    if (type == BonusType::Time) return sf::Color(255, 200, 50);
    if (type == BonusType::Speed) return sf::Color(120, 255, 255);
    return sf::Color(255, 255, 120);
}
}

void BonusSystem::spawn(sf::Vector2f pos, BonusType type, float value) {
    Bonus b;
    float radius = bonusRadius(type);
    b.shape = sf::CircleShape(radius);
    b.shape.setFillColor(bonusColor(type));
    b.shape.setOutlineColor(bonusOutline(type));
    b.shape.setOutlineThickness(2.0f);
    b.shape.setOrigin({radius, radius});
    b.shape.setPosition(pos);
    b.type = type;
    b.value = value;
    b.lifetime = BONUS_LIFETIME;
    b.alive = true;
    bonuses.push_back(b);
}

void BonusSystem::spawnOnKill(sf::Vector2f pos, float timeValue) {
    spawn(pos, BonusType::Time, timeValue);
    int roll = std::rand() % 100;
    if (roll < 25) {
        spawn(pos, BonusType::Speed, SPEED_BONUS_DURATION);
    } else if (roll < 40) {
        spawn(pos, BonusType::RapidFire, RAPID_FIRE_DURATION);
    }
}

void BonusSystem::update(float dt, sf::Vector2f playerPos, float playerRadius, float& timeLeft, float& speedBoostTimer, float& rapidFireTimer) {
    for (auto& b : bonuses) {
        b.lifetime -= dt;
        if (b.lifetime <= 0.0f)
            b.alive = false;
    }

    for (auto& b : bonuses) {
        if (!b.alive) continue;
        float collDist = playerRadius + b.shape.getRadius();
        if (distanceSq(b.shape.getPosition(), playerPos) < collDist * collDist) {
            if (b.type == BonusType::Time) {
                timeLeft = std::min(TOTAL_TIME, timeLeft + b.value);
            } else if (b.type == BonusType::Speed) {
                speedBoostTimer = std::max(speedBoostTimer, b.value);
            } else if (b.type == BonusType::RapidFire) {
                rapidFireTimer = std::max(rapidFireTimer, b.value);
            }
            b.alive = false;
        }
    }

    bonuses.erase(std::remove_if(bonuses.begin(), bonuses.end(), [](const Bonus& b) { return !b.alive; }), bonuses.end());
}

void BonusSystem::draw(sf::RenderTarget& target) const {
    for (const auto& b : bonuses)
        target.draw(b.shape);
}

void BonusSystem::clear() {
    bonuses.clear();
}
