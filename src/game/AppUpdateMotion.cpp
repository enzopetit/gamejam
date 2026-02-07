#include "game/AppFlow.hpp"
#include "utils/Math.hpp"
#include "game/ParticleEffects.hpp"
#include <algorithm>

void updateMotionAndShooting(App& a, float rawDt, float dt) {
    if (!a.level.isWaveActive()) return;
    if (a.speedBoostTimer > 0.0f) a.speedBoostTimer -= dt;
    if (a.rapidFireTimer > 0.0f) a.rapidFireTimer -= dt;
    float speedMult = a.speedBoostTimer > 0.0f ? SPEED_BONUS_MULT : 1.0f;
    float shootInterval = a.rapidFireTimer > 0.0f ? RAPID_FIRE_COOLDOWN : BASE_SHOOT_COOLDOWN;
    sf::Vector2f moveDir{0.0f,0.0f};
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) moveDir.x -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) moveDir.x += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) moveDir.y -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) moveDir.y += 1.0f;
    moveDir = vecNormalize(moveDir);
    sf::Vector2i mp = sf::Mouse::getPosition(a.window); sf::Vector2f mouse = a.window.mapPixelToCoords(mp);
    sf::Vector2f pos = a.player.getPosition(); pos += moveDir * (PLAYER_SPEED * speedMult) * dt;
    pos.x = std::clamp(pos.x, PLAYER_RADIUS, static_cast<float>(WIN_W) - PLAYER_RADIUS);
    pos.y = std::clamp(pos.y, PLAYER_RADIUS, static_cast<float>(WIN_H) - PLAYER_RADIUS);
    a.player.setPosition(pos);
    sf::Vector2f aim = vecNormalize(mouse - pos);
    if (a.shootSquash > 0.0f) { a.shootSquash -= rawDt; if (a.shootSquash < 0.0f) a.shootSquash = 0.0f; float t = a.shootSquash / SQUASH_DURATION; a.player.setScale({1.0f + (SQUASH_X-1.0f)*t, 1.0f + (SQUASH_Y-1.0f)*t}); }
    else a.player.setScale({1.0f,1.0f});
    a.shootCooldown -= dt;
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && a.shootCooldown <= 0.0f) {
        Bullet b; b.shape = sf::CircleShape(BULLET_RADIUS); b.shape.setFillColor(sf::Color(240,90,60)); b.shape.setOrigin({BULLET_RADIUS,BULLET_RADIUS});
        b.velocity = aim * BULLET_SPEED; sf::Vector2f bpos = pos + aim * (PLAYER_RADIUS + BULLET_RADIUS + 2.0f);
        b.shape.setPosition(bpos); a.bullets.push_back(b);
        a.shootCooldown = shootInterval; a.shakeTimer = SHAKE_DURATION * 0.3f; a.shootSquash = SQUASH_DURATION; a.timeLeft = std::max(0.0f, a.timeLeft - BULLET_TIME_COST);
        spawnBulletTrail(a.particles, bpos, sf::Color(180,40,20,150));
    }
    for (auto& b : a.bullets) { if (!b.alive) continue; sf::Vector2f p = b.shape.getPosition(); p += b.velocity * dt; b.shape.setPosition(p); if (p.x < -50 || p.x > WIN_W + 50 || p.y < -50 || p.y > WIN_H + 50) b.alive=false; }
}
