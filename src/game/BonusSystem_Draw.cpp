#include "game/BonusSystem.hpp"
#include "game/GameConfig.hpp"
#include <algorithm>
#include <cmath>

namespace {
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

void BonusSystem::draw(sf::RenderTarget& target, float gameTime) const {
    for (const auto& b : bonuses) {
        float pulse = 1.0f + BONUS_PULSE_AMP * std::sin(gameTime * BONUS_PULSE_FREQ + b.lifetime * 10.0f);
        sf::CircleShape drawn = b.shape; drawn.setScale({pulse, pulse});
        float alpha = std::min(1.0f, b.lifetime / 1.0f);
        if (b.lifetime < 1.0f) {
            sf::Color c = drawn.getFillColor(); c.a = static_cast<uint8_t>(alpha * 255); drawn.setFillColor(c);
            sf::Color oc = drawn.getOutlineColor(); oc.a = static_cast<uint8_t>(alpha * 255); drawn.setOutlineColor(oc);
        }
        target.draw(drawn);
    }
}
