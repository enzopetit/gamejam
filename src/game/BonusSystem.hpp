#pragma once

#include "actors/Actor.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

class BonusSystem {
public:
    void spawnOnKill(sf::Vector2f pos, float timeValue);
    void spawnRandomPowerBonus(sf::Vector2f pos);
    void update(float dt, sf::Vector2f playerPos, float playerRadius, float& timeLeft, float& speedBoostTimer, float& rapidFireTimer, float& pierceTimer, float pickupRadius = 0.0f);
    void update(float dt, sf::Vector2f playerPos, float playerRadius, float& timeLeft, float& speedBoostTimer, float& rapidFireTimer, float pickupRadius = 0.0f);
    void draw(sf::RenderTarget& target, float gameTime) const;
    void clear();

    const std::vector<sf::Vector2f>& getPickups() const { return pickupPositions; }
    void clearPickups() { pickupPositions.clear(); }

private:
    void spawn(sf::Vector2f pos, BonusType type, float value);

    std::vector<Bonus> bonuses;
    std::vector<sf::Vector2f> pickupPositions;
};
