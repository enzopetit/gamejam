#include "game/BonusSystem.hpp"
#include "game/GameConfig.hpp"
#include <cmath>

void BonusSystem::draw(sf::RenderTarget& target, float gameTime) const {
    for (const auto& b : bonuses) {
        float pulse = 1.0f + BONUS_PULSE_AMP * std::sin(gameTime * BONUS_PULSE_FREQ + b.lifetime * 10.0f);
        sf::CircleShape drawn = b.shape; drawn.setScale({pulse,pulse});
        float alpha = std::min(1.0f, b.lifetime / 1.0f);
        if (b.lifetime < 1.0f) { sf::Color c=drawn.getFillColor(); c.a=static_cast<uint8_t>(alpha*255); drawn.setFillColor(c); sf::Color oc=drawn.getOutlineColor(); oc.a=static_cast<uint8_t>(alpha*255); drawn.setOutlineColor(oc);} 
        target.draw(drawn);
    }
}

void BonusSystem::clear() { bonuses.clear(); pickupPositions.clear(); }

