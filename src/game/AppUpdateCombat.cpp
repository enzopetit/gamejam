#include "game/AppFlow.hpp"
#include "game/ParticleEffects.hpp"
#include "utils/Math.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace {
constexpr float PI = 3.14159265f;
constexpr float LASER_RANGE = 2200.0f;
constexpr float PROJECTILE_DESPAWN_MARGIN = 80.0f;
constexpr float BOSS_CONTACT_COOLDOWN = 0.45f;
constexpr float BOSS_PHASE_DURATION = 6.0f;
constexpr int BOSS_SPIRAL_PROJECTILE_COUNT = 12;
constexpr float BOSS_SPIRAL_PROJECTILE_SPEED = 260.0f;
constexpr int BOSS_CURTAIN_PROJECTILE_COUNT = 11;
constexpr float BOSS_CURTAIN_SPREAD_DEG = 120.0f;

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

void spawnBossSpiral(App& a, const Enemy& boss, float offsetAngle, bool destructible) {
    sf::Vector2f center = boss.shape.getPosition();
    float spin = boss.phaseTimer * (destructible ? 3.2f : -2.5f);
    for (int i = 0; i < BOSS_SPIRAL_PROJECTILE_COUNT; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(BOSS_SPIRAL_PROJECTILE_COUNT);
        float angle = spin + offsetAngle + t * 2.0f * PI;
        spawnEnemyProjectile(
            a,
            center,
            vectorFromAngle(angle),
            BOSS_SPIRAL_PROJECTILE_SPEED * (destructible ? 0.9f : 1.0f),
            destructible ? BOSS_RING_PROJECTILE_RADIUS : BOSS_FAN_PROJECTILE_RADIUS * 0.85f,
            destructible ? sf::Color(120, 245, 255) : sf::Color(255, 120, 210),
            destructible ? BOSS_RING_PROJECTILE_DAMAGE : BOSS_FAN_PROJECTILE_DAMAGE * 0.85f,
            destructible
        );
    }
}

void spawnBossCurtain(App& a, const Enemy& boss, sf::Vector2f playerPos) {
    sf::Vector2f center = boss.shape.getPosition();
    sf::Vector2f aim = vecNormalize(playerPos - center);
    if (aim.x == 0.0f && aim.y == 0.0f) aim = {1.0f, 0.0f};

    float baseAngle = std::atan2(aim.y, aim.x);
    float spread = toRadians(BOSS_CURTAIN_SPREAD_DEG);
    float start = baseAngle - spread * 0.5f;
    float step = spread / static_cast<float>(BOSS_CURTAIN_PROJECTILE_COUNT - 1);
    for (int i = 0; i < BOSS_CURTAIN_PROJECTILE_COUNT; ++i) {
        float angle = start + static_cast<float>(i) * step;
        bool destructible = (i % 2) == 0;
        spawnEnemyProjectile(
            a,
            center,
            vectorFromAngle(angle),
            destructible ? BOSS_RING_PROJECTILE_SPEED : BOSS_FAN_PROJECTILE_SPEED,
            destructible ? BOSS_RING_PROJECTILE_RADIUS * 0.9f : BOSS_FAN_PROJECTILE_RADIUS * 0.9f,
            destructible ? sf::Color(90, 230, 255) : sf::Color(248, 88, 188),
            destructible ? BOSS_RING_PROJECTILE_DAMAGE : BOSS_FAN_PROJECTILE_DAMAGE,
            destructible
        );
    }
}

int bossAttackPhase(const Enemy& boss) {
    return static_cast<int>(boss.phaseTimer / BOSS_PHASE_DURATION) % 3;
}

void spawnBossPattern(App& a, const Enemy& boss, sf::Vector2f playerPos, int phase) {
    if (phase == 0) {
        spawnBossRing(a, boss);
        spawnBossFan(a, boss, playerPos);
        return;
    }
    if (phase == 1) {
        spawnBossSpiral(a, boss, 0.0f, true);
        spawnBossSpiral(a, boss, PI / static_cast<float>(BOSS_SPIRAL_PROJECTILE_COUNT), false);
        return;
    }
    spawnBossCurtain(a, boss, playerPos);
    spawnBossFan(a, boss, playerPos);
}

float nextBossShotDelay(int phase) {
    if (phase == 1) return BOSS_SHOOT_INTERVAL * 0.75f;
    if (phase == 2) return BOSS_SHOOT_INTERVAL * 1.15f;
    return BOSS_SHOOT_INTERVAL;
}

void updateFireflyTeleports(App& a, float dt) {
    for (auto& e : a.enemies) {
        if (!e.alive || e.type != EnemyType::Firefly) continue;
        e.shootTimer -= dt;
        if (e.shootTimer > 0.0f) continue;
        sf::Vector2f oldPos = e.shape.getPosition();
        spawnParticles(a.particles, oldPos, 10, sf::Color(255, 255, 100));
        float nx = static_cast<float>(std::rand() % WIN_W);
        float ny = static_cast<float>(std::rand() % WIN_H);
        e.shape.setPosition({nx, ny});
        spawnParticles(a.particles, {nx, ny}, 10, sf::Color(255, 255, 100));
        e.shootTimer = FIREFLY_TELEPORT_MIN + static_cast<float>(std::rand() % 100) / 100.0f * (FIREFLY_TELEPORT_MAX - FIREFLY_TELEPORT_MIN);
    }
}

void updateScorpionAttacks(App& a, sf::Vector2f playerPos, float dt) {
    for (auto& e : a.enemies) {
        if (!e.alive || e.type != EnemyType::Scorpion) continue;
        float dist = std::sqrt(distanceSq(e.shape.getPosition(), playerPos));
        if (dist > SCORPION_ENGAGE_DISTANCE) continue;
        e.shootTimer -= dt;
        if (e.shootTimer > 0.0f) continue;
        sf::Vector2f dir = vecNormalize(playerPos - e.shape.getPosition());
        spawnEnemyProjectile(
            a,
            e.shape.getPosition(),
            dir,
            SCORPION_PROJECTILE_SPEED,
            SCORPION_PROJECTILE_RADIUS,
            sf::Color(180, 120, 40),
            SCORPION_PROJECTILE_DAMAGE,
            true
        );
        e.shootTimer = SCORPION_SHOOT_INTERVAL;
    }
}

void updateBossAttacks(App& a, sf::Vector2f playerPos, float dt) {
    for (auto& e : a.enemies) {
        if (!e.alive || e.type != EnemyType::Boss) continue;
        e.shootTimer -= dt;
        if (e.shootTimer > 0.0f) continue;
        int phase = bossAttackPhase(e);
        spawnBossPattern(a, e, playerPos, phase);
        e.shootTimer = nextBossShotDelay(phase);
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
    if (e.type == EnemyType::Ladybug) {
        sf::Vector2f ePos = e.shape.getPosition();
        sf::Vector2f pPos = a.player.getPosition();
        spawnParticles(a.particles, ePos, 30, sf::Color(255, 60, 30));
        spawnParticles(a.particles, ePos, 20, sf::Color(255, 160, 40));
        float dist = std::sqrt(distanceSq(ePos, pPos));
        if (dist < LADYBUG_EXPLOSION_RADIUS) {
            a.timeLeft = std::max(0.0f, a.timeLeft - LADYBUG_EXPLOSION_PENALTY);
            a.shakeTimer = std::max(a.shakeTimer, SHAKE_DURATION);
        }
    }
    if (e.type == EnemyType::Boss) {
        a.speedBoostTimer = std::max(a.speedBoostTimer, SPEED_BONUS_DURATION);
        a.rapidFireTimer = std::max(a.rapidFireTimer, RAPID_FIRE_DURATION);
        a.pierceTimer = std::max(a.pierceTimer, PIERCE_DURATION);
    }
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

    if (e.type == EnemyType::Mosquito) {
        sf::Vector2f perp = {-dir.y, dir.x};
        float zigzag = std::sin(e.phaseTimer * MOSQUITO_ZIGZAG_FREQ) * MOSQUITO_ZIGZAG_AMP;
        ePos += (dir + perp * (zigzag / e.speed)) * e.speed * dt;
    } else if (e.type == EnemyType::Scorpion) {
        float dist = std::sqrt(distanceSq(ePos, playerPos));
        if (dist > SCORPION_ENGAGE_DISTANCE)
            ePos += dir * e.speed * dt;
    } else {
        ePos += dir * e.speed * dt;
    }

    e.shape.setPosition(ePos);
    if (e.flashTimer > 0.0f) {
        e.flashTimer -= dt;
        if (e.flashTimer <= 0.0f) e.shape.setFillColor(e.baseColor);
    }
}

void collideEnemyPlayer(App& a, Enemy& e, sf::Vector2f playerPos) {
    float collDist = PLAYER_RADIUS + e.shape.getRadius();
    if (distanceSq(e.shape.getPosition(), playerPos) >= collDist * collDist) return;
    if (e.type == EnemyType::Boss) {
        if (a.bossContactCooldown > 0.0f) return;
        a.timeLeft -= 3.0f;
        a.bossContactCooldown = BOSS_CONTACT_COOLDOWN;
        a.shakeTimer = std::max(a.shakeTimer, SHAKE_DURATION);
        spawnParticles(a.particles, e.shape.getPosition(), 12, sf::Color(200, 120, 220));
        return;
    }
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
    float comboMultiplier = 1.0f + static_cast<float>(a.comboCount) * 0.5f;
    damagePerSecond *= comboMultiplier;
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
    if (a.bossContactCooldown > 0.0f) a.bossContactCooldown = std::max(0.0f, a.bossContactCooldown - dt);
    sf::Vector2f playerPos = a.player.getPosition();
    for (auto& e : a.enemies) {
        updateEnemyMotion(e, playerPos, rawDt, dt);
        collideEnemyPlayer(a, e, playerPos);
    }
    updateBossAttacks(a, playerPos, dt);
    updateFireflyTeleports(a, dt);
    updateScorpionAttacks(a, playerPos, dt);
    updateEnemyProjectilesMotion(a, dt);
    collideEnemyProjectilesWithPlayer(a, playerPos);
    a.comboTimer -= dt;
    if (a.comboTimer <= 0.0f) a.comboCount = 0;
    updateBulletCollisions(a);
    collideBulletsWithEnemyProjectiles(a);
    applyLaserDamage(a, dt, playerPos);
    applyLaserToEnemyProjectiles(a, playerPos);
}
