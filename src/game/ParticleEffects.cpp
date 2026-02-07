#include "game/ParticleEffects.hpp"

#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <cstdlib>

void spawnParticles(std::vector<Particle>& particles, sf::Vector2f pos, int count, sf::Color color) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.shape = sf::CircleShape(static_cast<float>(2 + std::rand() % 4));
        p.shape.setFillColor(color);
        p.shape.setPosition(pos);
        float angle = static_cast<float>(std::rand() % 360) * 3.14159f / 180.0f;
        float speed = static_cast<float>(50 + std::rand() % 200);
        p.velocity = {std::cos(angle) * speed, std::sin(angle) * speed};
        p.maxLifetime = 0.3f + static_cast<float>(std::rand() % 30) / 100.0f;
        p.lifetime = p.maxLifetime;
        particles.push_back(p);
    }
}

void spawnBulletTrail(std::vector<Particle>& particles, sf::Vector2f pos, sf::Color color) {
    Particle p;
    p.shape = sf::CircleShape(BULLET_TRAIL_RADIUS);
    p.shape.setFillColor(color);
    p.shape.setOrigin({BULLET_TRAIL_RADIUS, BULLET_TRAIL_RADIUS});
    p.shape.setPosition(pos);
    float angle = static_cast<float>(std::rand() % 360) * 3.14159f / 180.0f;
    p.velocity = {std::cos(angle) * 15.0f, std::sin(angle) * 15.0f};
    p.maxLifetime = BULLET_TRAIL_LIFETIME;
    p.lifetime = p.maxLifetime;
    particles.push_back(p);
}

