#include "game/AppFlow.hpp"
#include "game/ParticleEffects.hpp"
#include "utils/Math.hpp"

#include <algorithm>

namespace {
sf::Vector2f readMoveDir() {
    sf::Vector2f moveDir{0.0f, 0.0f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) moveDir.x -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) moveDir.x += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) moveDir.y -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) moveDir.y += 1.0f;
    return vecNormalize(moveDir);
}

sf::Vector2f mouseWorld(const App& a) {
    sf::Vector2i mp = sf::Mouse::getPosition(a.window);
    return a.window.mapPixelToCoords(mp);
}

void updateTimedPowers(App& a, float dt) {
    if (a.speedBoostTimer > 0.0f) a.speedBoostTimer = std::max(0.0f, a.speedBoostTimer - dt);
    if (a.rapidFireTimer > 0.0f) a.rapidFireTimer = std::max(0.0f, a.rapidFireTimer - dt);
    if (a.pierceTimer > 0.0f) a.pierceTimer = std::max(0.0f, a.pierceTimer - dt);
}

void updateLaserState(App& a, float rawDt, sf::Vector2f aim) {
    if (aim.x != 0.0f || aim.y != 0.0f) a.laserDirection = aim;
    if (a.laserTimer > 0.0f) a.laserTimer = std::max(0.0f, a.laserTimer - rawDt);
    else a.laserCharge = std::min(1.0f, a.laserCharge + rawDt * LASER_CHARGE_RATE);

    bool laserPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
    if (laserPressed && !a.laserKeyHeld && a.laserTimer <= 0.0f && a.laserCharge >= 1.0f && a.timeLeft >= LASER_TIME_COST) {
        a.laserCharge = 0.0f;
        a.laserTimer = LASER_DURATION;
        a.timeLeft = std::max(0.0f, a.timeLeft - LASER_TIME_COST);
    }
    a.laserKeyHeld = laserPressed;
}

void updatePlayerPosition(App& a, sf::Vector2f moveDir, float dt) {
    float speedMult = a.speedBoostTimer > 0.0f ? SPEED_BONUS_MULT : 1.0f;
    sf::Vector2f pos = a.player.getPosition();
    pos += moveDir * (PLAYER_SPEED * speedMult) * dt;
    pos.x = std::clamp(pos.x, PLAYER_RADIUS, static_cast<float>(WIN_W) - PLAYER_RADIUS);
    pos.y = std::clamp(pos.y, PLAYER_RADIUS, static_cast<float>(WIN_H) - PLAYER_RADIUS);
    a.player.setPosition(pos);
}

void updatePlayerSquash(App& a, float rawDt) {
    if (a.shootSquash > 0.0f) {
        a.shootSquash = std::max(0.0f, a.shootSquash - rawDt);
        float t = a.shootSquash / SQUASH_DURATION;
        a.player.setScale({1.0f + (SQUASH_X - 1.0f) * t, 1.0f + (SQUASH_Y - 1.0f) * t});
        return;
    }
    a.player.setScale({1.0f, 1.0f});
}

float shootInterval(const App& a) {
    return a.rapidFireTimer > 0.0f ? RAPID_FIRE_COOLDOWN : BASE_SHOOT_COOLDOWN;
}

void fireBullet(App& a, sf::Vector2f pos, sf::Vector2f aim, float interval) {
    if (a.laserTimer > 0.0f) return;
    if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) || a.shootCooldown > 0.0f) return;

    Bullet b;
    b.shape = sf::CircleShape(BULLET_RADIUS);
    b.shape.setFillColor(sf::Color(240, 90, 60));
    b.shape.setOrigin({BULLET_RADIUS, BULLET_RADIUS});
    b.velocity = aim * BULLET_SPEED;
    b.piercing = a.pierceTimer > 0.0f;
    if (b.piercing) b.shape.setFillColor(sf::Color(80, 220, 120));
    sf::Vector2f bpos = pos + aim * (PLAYER_RADIUS + BULLET_RADIUS + 2.0f);
    b.shape.setPosition(bpos);
    a.bullets.push_back(b);

    a.shootCooldown = interval;
    a.shakeTimer = SHAKE_DURATION * 0.3f;
    a.shootSquash = SQUASH_DURATION;
    a.timeLeft = std::max(0.0f, a.timeLeft - BULLET_TIME_COST);
    spawnBulletTrail(a.particles, bpos, sf::Color(180, 40, 20, 150));
}

void updateBullets(App& a, float dt) {
    for (auto& b : a.bullets) {
        if (!b.alive) continue;
        sf::Vector2f p = b.shape.getPosition();
        p += b.velocity * dt;
        b.shape.setPosition(p);
        if (p.x < -50 || p.x > WIN_W + 50 || p.y < -50 || p.y > WIN_H + 50) b.alive = false;
    }
}
}

void updateMotionAndShooting(App& a, float rawDt, float dt) {
    if (!a.level.isWaveActive()) return;
    updateTimedPowers(a, dt);
    updatePlayerPosition(a, readMoveDir(), dt);
    sf::Vector2f pos = a.player.getPosition();
    sf::Vector2f aim = vecNormalize(mouseWorld(a) - pos);
    updateLaserState(a, rawDt, aim);
    updatePlayerSquash(a, rawDt);
    a.shootCooldown -= dt;
    fireBullet(a, pos, aim, shootInterval(a));
    updateBullets(a, dt);
}
