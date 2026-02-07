#include "game/BonusSystem.hpp"

#include "game/GameConfig.hpp"
#include "utils/Math.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace {
float bonusRadius(BonusType type) {
    if (type == BonusType::Time) return 10.0f;
    if (type == BonusType::Speed) return 12.0f;
    if (type == BonusType::Pierce) return 12.0f;
    return 12.0f;
}

sf::Color bonusColor(BonusType type) {
    if (type == BonusType::Time) return sf::Color(255, 80, 80);
    if (type == BonusType::Speed) return sf::Color(80, 200, 255);
    if (type == BonusType::Pierce) return sf::Color(80, 220, 120);
    return sf::Color(255, 200, 80);
}

sf::Color bonusOutline(BonusType type) {
    if (type == BonusType::Time) return sf::Color(255, 200, 50);
    if (type == BonusType::Speed) return sf::Color(120, 255, 255);
    if (type == BonusType::Pierce) return sf::Color(140, 255, 170);
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
    } else if (roll < 50) {
        spawn(pos, BonusType::Pierce, PIERCE_DURATION);
    }
}

void BonusSystem::update(float dt, sf::Vector2f playerPos, float playerRadius, float& timeLeft, float& speedBoostTimer, float& rapidFireTimer, float& pierceTimer) {
    pickupPositions.clear();

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
            } else if (b.type == BonusType::Pierce) {
                pierceTimer = std::max(pierceTimer, b.value);
            }
            pickupPositions.push_back(b.shape.getPosition());
            b.alive = false;
        }
    }

    bonuses.erase(std::remove_if(bonuses.begin(), bonuses.end(), [](const Bonus& b) { return !b.alive; }), bonuses.end());
}

void BonusSystem::draw(sf::RenderTarget& target, float gameTime) const {
    for (const auto& b : bonuses) {
        float pulse = 1.0f + BONUS_PULSE_AMP * std::sin(gameTime * BONUS_PULSE_FREQ + b.lifetime * 10.0f);
        sf::CircleShape drawn = b.shape;
        drawn.setScale({pulse, pulse});

        float alpha = std::min(1.0f, b.lifetime / 1.0f);
        if (b.lifetime < 1.0f) {
            sf::Color c = drawn.getFillColor();
            c.a = static_cast<uint8_t>(alpha * 255);
            drawn.setFillColor(c);
            sf::Color oc = drawn.getOutlineColor();
            oc.a = static_cast<uint8_t>(alpha * 255);
            drawn.setOutlineColor(oc);
        }

        target.draw(drawn);
    }
}

void BonusSystem::clear() {
    bonuses.clear();
    pickupPositions.clear();
}
