#pragma once

#include "actors/Actor.hpp"

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>

class BonusSystem {
public:
    void spawnOnKill(sf::Vector2f pos, float timeValue);
    void update(float dt, sf::Vector2f playerPos, float playerRadius, float& timeLeft, float& speedBoostTimer, float& rapidFireTimer);
    void draw(sf::RenderTarget& target) const;
    void clear();

private:
    void spawn(sf::Vector2f pos, BonusType type, float value);

    std::vector<Bonus> bonuses;
};
