#include "game/AppFlow.hpp"
#include "game/ParticleEffects.hpp"
#include "utils/Math.hpp"

#include <algorithm>
#include <cmath>

namespace {
constexpr float PI = 3.14159265f;
constexpr float LASER_RANGE = 2200.0f;
constexpr float PROJECTILE_DESPAWN_MARGIN = 80.0f;

float dot(sf::Vector2f a, sf::Vector2f b) {
    return a.x * b.x + a.y * b.y;
}

float laserDistanceSq(sf::Vector2f point, sf::Vector2f origin, sf::Vector2f dir, float range) {
    sf::Vector2f rel = point - origin;
    float proj = std::clamp(dot(rel, dir), 0.0f, range);
    sf::Vector2f closest = origin + dir * proj;
    return distanceSq(point, closest);
}

float toRadians(float degrees) {
    return degrees * PI / 180.0f;
}

sf::Vector2f vectorFromAngle(float angle) {
    return {std::cos(angle), std::sin(angle)};
}

void spawnEnemyProjectile(
    App& a,
    sf::Vector2f position,
    sf::Vector2f dir,
    float speed,
    float radius,
    sf::Color color,
    float damage,
    bool destructible
) {
    EnemyProjectile p;
    p.shape = sf::CircleShape(radius);
    p.shape.setOrigin({radius, radius});
    p.shape.setPosition(position);
    p.shape.setFillColor(color);
    p.shape.setOutlineThickness(2.0f);
    p.shape.setOutlineColor(destructible ? sf::Color(220, 255, 255) : sf::Color(40, 20, 60));
    p.velocity = vecNormalize(dir) * speed;
    p.damage = damage;
    p.destructible = destructible;
    a.enemyProjectiles.push_back(p);
}

void spawnBossRing(App& a, const Enemy& boss) {
    sf::Vector2f center = boss.shape.getPosition();
    float baseAngle = boss.phaseTimer * 1.3f;
    for (int i = 0; i < BOSS_RING_PROJECTILE_COUNT; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(BOSS_RING_PROJECTILE_COUNT);
        float angle = baseAngle + t * 2.0f * PI;
        spawnEnemyProjectile(
            a,
            center,
            vectorFromAngle(angle),
            BOSS_RING_PROJECTILE_SPEED,
            BOSS_RING_PROJECTILE_RADIUS,
            sf::Color(90, 230, 255),
            BOSS_RING_PROJECTILE_DAMAGE,
            true
        );
    }
}

void spawnBossFan(App& a, const Enemy& boss, sf::Vector2f playerPos) {
    sf::Vector2f center = boss.shape.getPosition();
    sf::Vector2f aim = vecNormalize(playerPos - center);
    if (aim.x == 0.0f && aim.y == 0.0f) aim = {1.0f, 0.0f};

    float baseAngle = std::atan2(aim.y, aim.x);
    float spread = toRadians(BOSS_FAN_PROJECTILE_SPREAD_DEG);
    float start = baseAngle - spread * 0.5f;
    float step = BOSS_FAN_PROJECTILE_COUNT > 1 ? spread / static_cast<float>(BOSS_FAN_PROJECTILE_COUNT - 1) : 0.0f;
    for (int i = 0; i < BOSS_FAN_PROJECTILE_COUNT; ++i) {
        float angle = start + static_cast<float>(i) * step;
        spawnEnemyProjectile(
            a,
            center,
            vectorFromAngle(angle),
            BOSS_FAN_PROJECTILE_SPEED,
            BOSS_FAN_PROJECTILE_RADIUS,
            sf::Color(245, 80, 185),
            BOSS_FAN_PROJECTILE_DAMAGE,
            false
        );
    }
}

void updateBossAttacks(App& a, sf::Vector2f playerPos, float dt) {
    for (auto& e : a.enemies) {
        if (!e.alive || e.type != EnemyType::Boss) continue;
        e.shootTimer -= dt;
        if (e.shootTimer > 0.0f) continue;
        spawnBossRing(a, e);
        spawnBossFan(a, e, playerPos);
        e.shootTimer = BOSS_SHOOT_INTERVAL;
    }
}

void updateEnemyProjectilesMotion(App& a, float dt) {
    for (auto& p : a.enemyProjectiles) {
        if (!p.alive) continue;
        sf::Vector2f pos = p.shape.getPosition();
        pos += p.velocity * dt;
        p.shape.setPosition(pos);
        if (pos.x < -PROJECTILE_DESPAWN_MARGIN || pos.x > WIN_W + PROJECTILE_DESPAWN_MARGIN || pos.y < -PROJECTILE_DESPAWN_MARGIN || pos.y > WIN_H + PROJECTILE_DESPAWN_MARGIN) {
            p.alive = false;
        }
    }
}

void onEnemyKilled(App& a, Enemy& e) {
    e.alive = false;
    a.comboCount++;
    a.comboTimer = 1.0f;
    float dropValue = e.timeDrop;
    if (a.comboCount >= 3) dropValue *= 1.0f + (a.comboCount - 2) * 0.25f;
    int killScore = static_cast<int>(10 * e.maxHp * (1.0f + a.comboCount * 0.1f));
    a.score += killScore;
    spawnParticles(a.particles, e.shape.getPosition(), 20, e.baseColor);

    bool isBig = e.maxHp >= 5;
    a.hitstopTimer = isBig ? HITSTOP_BIG : HITSTOP_SMALL;
    if (a.comboCount >= SLOWMO_COMBO_THRESHOLD || isBig) a.slowMoTimer = SLOWMO_DURATION;
    a.flashAlpha = FLASH_MAX_ALPHA;

    Shockwave sw;
    sw.position = e.shape.getPosition();
    sw.radius = e.shape.getRadius();
    sw.maxRadius = SHOCKWAVE_MAX_RADIUS + (isBig ? 40.0f : 0.0f);
    sw.maxLifetime = SHOCKWAVE_LIFETIME;
    sw.lifetime = sw.maxLifetime;
    sw.color = e.baseColor;
    a.shockwaves.push_back(sw);

    if (a.fontLoaded) {
        FloatingText ft;
        ft.position = e.shape.getPosition() + sf::Vector2f(0.0f, -20.0f);
        ft.velocity = {0.0f, -FLOATING_TEXT_SPEED};
        ft.maxLifetime = FLOATING_TEXT_LIFETIME;
        ft.lifetime = ft.maxLifetime;
        ft.text = "+" + std::to_string(killScore);
        ft.color = sf::Color(255, 255, 100);
        ft.size = 18;
        a.floatingTexts.push_back(ft);

        if (a.comboCount >= 3) {
            FloatingText ct;
            ct.position = e.shape.getPosition() + sf::Vector2f(0.0f, -40.0f);
            ct.velocity = {0.0f, -FLOATING_TEXT_SPEED * 0.8f};
            ct.maxLifetime = FLOATING_TEXT_LIFETIME;
            ct.lifetime = ct.maxLifetime;
            ct.text = "x" + std::to_string(a.comboCount);
            ct.color = sf::Color(255, 200, 50);
            ct.size = 22;
            a.floatingTexts.push_back(ct);
        }
    }

    a.bonusSystem.spawnOnKill(e.shape.getPosition(), dropValue);
    a.level.onEnemyKilled();
}

void updateEnemyMotion(Enemy& e, sf::Vector2f playerPos, float rawDt, float dt) {
    e.spawnAge += rawDt;
    e.phaseTimer += dt;
    sf::Vector2f ePos = e.shape.getPosition();
    sf::Vector2f dir = vecNormalize(playerPos - ePos);
    ePos += dir * e.speed * dt;
    e.shape.setPosition(ePos);
    if (e.flashTimer > 0.0f) {
        e.flashTimer -= dt;
        if (e.flashTimer <= 0.0f) e.shape.setFillColor(e.baseColor);
    }
}

void collideEnemyPlayer(App& a, Enemy& e, sf::Vector2f playerPos) {
    float collDist = PLAYER_RADIUS + e.shape.getRadius();
    if (distanceSq(e.shape.getPosition(), playerPos) >= collDist * collDist) return;
    a.timeLeft -= 3.0f;
    e.alive = false;
    a.shakeTimer = SHAKE_DURATION;
    spawnParticles(a.particles, e.shape.getPosition(), 15, sf::Color(200, 50, 50));
}

bool isBulletEnemyCollision(const Bullet& b, const Enemy& e) {
    float collDist = b.shape.getRadius() + e.shape.getRadius();
    return distanceSq(b.shape.getPosition(), e.shape.getPosition()) < collDist * collDist;
}

void onBulletHit(App& a, Bullet& b, Enemy& e) {
    sf::Vector2f bPos = b.shape.getPosition();
    if (!b.piercing) b.alive = false;
    e.hp -= b.damage;
    e.flashTimer = 0.08f;
    e.shape.setFillColor(sf::Color::White);
    a.shakeTimer = SHAKE_DURATION * 0.5f;
    spawnParticles(a.particles, bPos, 8, sf::Color(220, 50, 30));

    if (b.piercing) {
        sf::Vector2f shift = vecNormalize(b.velocity) * (e.shape.getRadius() + b.shape.getRadius() + 1.0f);
        b.shape.setPosition(bPos + shift);
    }

    if (e.hp <= 0.0f) onEnemyKilled(a, e);
}

void updateBulletCollisions(App& a) {
    for (auto& b : a.bullets) {
        if (!b.alive) continue;
        for (auto& e : a.enemies) {
            if (!e.alive || !isBulletEnemyCollision(b, e)) continue;
            onBulletHit(a, b, e);
            break;
        }
    }
}

void onDestructibleProjectileDestroyedByPlayer(App& a, EnemyProjectile& p) {
    p.alive = false;
    sf::Vector2f pos = p.shape.getPosition();
    spawnParticles(a.particles, pos, 6, p.shape.getFillColor());
    a.bonusSystem.spawnRandomPowerBonus(pos);
}

bool isBulletProjectileCollision(const Bullet& b, const EnemyProjectile& p) {
    float collDist = b.shape.getRadius() + p.shape.getRadius();
    return distanceSq(b.shape.getPosition(), p.shape.getPosition()) < collDist * collDist;
}

void collideBulletsWithEnemyProjectiles(App& a) {
    for (auto& b : a.bullets) {
        if (!b.alive) continue;
        for (auto& p : a.enemyProjectiles) {
            if (!p.alive || !p.destructible || !isBulletProjectileCollision(b, p)) continue;
            onDestructibleProjectileDestroyedByPlayer(a, p);
            if (!b.piercing) {
                b.alive = false;
                break;
            }
            sf::Vector2f shift = vecNormalize(b.velocity) * (p.shape.getRadius() + b.shape.getRadius() + 1.0f);
            b.shape.setPosition(b.shape.getPosition() + shift);
        }
    }
}

void collideEnemyProjectilesWithPlayer(App& a, sf::Vector2f playerPos) {
    for (auto& p : a.enemyProjectiles) {
        if (!p.alive) continue;
        float collDist = PLAYER_RADIUS + p.shape.getRadius();
        if (distanceSq(p.shape.getPosition(), playerPos) >= collDist * collDist) continue;
        p.alive = false;
        a.timeLeft = std::max(0.0f, a.timeLeft - p.damage);
        a.shakeTimer = std::max(a.shakeTimer, SHAKE_DURATION * 0.45f);
        spawnParticles(a.particles, p.shape.getPosition(), 10, p.shape.getFillColor());
    }
}

void applyLaserDamage(App& a, float dt, sf::Vector2f playerPos) {
    if (a.laserTimer <= 0.0f) return;
    sf::Vector2f dir = vecNormalize(a.laserDirection);
    if (dir.x == 0.0f && dir.y == 0.0f) return;

    float damagePerSecond = LASER_DAMAGE_PER_SEC + static_cast<float>(a.level.waveIndex()) * LASER_DAMAGE_PER_WAVE;
    float beamRadius = LASER_WIDTH * 0.5f;
    for (auto& e : a.enemies) {
        if (!e.alive) continue;
        float hitRadius = beamRadius + e.shape.getRadius();
        if (laserDistanceSq(e.shape.getPosition(), playerPos, dir, LASER_RANGE) > hitRadius * hitRadius) continue;

        e.hp -= damagePerSecond * dt;
        e.flashTimer = 0.03f;
        e.shape.setFillColor(sf::Color(255, 230, 200));
        if (e.hp <= 0.0f) onEnemyKilled(a, e);
    }
}

void applyLaserToEnemyProjectiles(App& a, sf::Vector2f playerPos) {
    if (a.laserTimer <= 0.0f) return;
    sf::Vector2f dir = vecNormalize(a.laserDirection);
    if (dir.x == 0.0f && dir.y == 0.0f) return;

    float beamRadius = LASER_WIDTH * 0.5f;
    for (auto& p : a.enemyProjectiles) {
        if (!p.alive || !p.destructible) continue;
        float hitRadius = beamRadius + p.shape.getRadius();
        if (laserDistanceSq(p.shape.getPosition(), playerPos, dir, LASER_RANGE) > hitRadius * hitRadius) continue;
        onDestructibleProjectileDestroyedByPlayer(a, p);
    }
}
}

void updateEnemiesAndCollisions(App& a, float rawDt, float dt) {
    if (!a.level.isWaveActive()) return;
    sf::Vector2f playerPos = a.player.getPosition();
    for (auto& e : a.enemies) {
        updateEnemyMotion(e, playerPos, rawDt, dt);
        collideEnemyPlayer(a, e, playerPos);
    }
    updateBossAttacks(a, playerPos, dt);
    updateEnemyProjectilesMotion(a, dt);
    collideEnemyProjectilesWithPlayer(a, playerPos);
    a.comboTimer -= dt;
    if (a.comboTimer <= 0.0f) a.comboCount = 0;
    updateBulletCollisions(a);
    collideBulletsWithEnemyProjectiles(a);
    applyLaserDamage(a, dt, playerPos);
    applyLaserToEnemyProjectiles(a, playerPos);
}
