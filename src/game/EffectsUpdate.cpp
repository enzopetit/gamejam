#include "game/EffectsUpdate.hpp"

#include <SFML/Graphics.hpp>

void updateParticles(std::vector<Particle>& particles, float rawDt) {
    for (auto& p : particles) {
        p.lifetime -= rawDt;
        if (p.lifetime <= 0.0f) { p.alive = false; continue; }
        sf::Vector2f pos = p.shape.getPosition();
        pos += p.velocity * rawDt;
        p.shape.setPosition(pos);
        p.velocity *= 0.95f;
        float alpha = p.lifetime / p.maxLifetime;
        sf::Color c = p.shape.getFillColor();
        c.a = static_cast<uint8_t>(alpha * 255);
        p.shape.setFillColor(c);
    }
}

void updateShockwaves(std::vector<Shockwave>& shockwaves, float rawDt) {
    for (auto& sw : shockwaves) {
        sw.lifetime -= rawDt;
        if (sw.lifetime <= 0.0f) { sw.alive = false; continue; }
        float t = 1.0f - sw.lifetime / sw.maxLifetime;
        sw.radius = sw.maxRadius * t;
    }
}

void updateFloatingTexts(std::vector<FloatingText>& texts, float rawDt) {
    for (auto& ft : texts) {
        ft.lifetime -= rawDt;
        if (ft.lifetime <= 0.0f) { ft.alive = false; continue; }
        ft.position += ft.velocity * rawDt;
    }
}

