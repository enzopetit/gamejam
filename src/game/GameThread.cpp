#include "game/GameThread.hpp"

#include "actors/Actor.hpp"
#include "game/GameConfig.hpp"
#include "utils/FontLoader.hpp"
#include "utils/Math.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

namespace {
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

Enemy createEnemy(int type, float gameTime) {
    Enemy e;
    float side = static_cast<float>(std::rand() % 4);
    float x, y;
    if (side < 1) { x = -30; y = static_cast<float>(std::rand() % WIN_H); }
    else if (side < 2) { x = WIN_W + 30.0f; y = static_cast<float>(std::rand() % WIN_H); }
    else if (side < 3) { x = static_cast<float>(std::rand() % WIN_W); y = -30; }
    else { x = static_cast<float>(std::rand() % WIN_W); y = WIN_H + 30.0f; }

    float speedMult = 1.0f + gameTime / 120.0f;

    if (type == 0) {
        e.shape = sf::CircleShape(12.0f);
        e.baseColor = sf::Color(100, 200, 100);
        e.hp = 1; e.maxHp = 1;
        e.speed = ENEMY_BASE_SPEED * 1.5f * speedMult;
        e.timeDrop = 1.0f;
    } else if (type == 1) {
        e.shape = sf::CircleShape(18.0f);
        e.baseColor = sf::Color(220, 200, 50);
        e.hp = 3; e.maxHp = 3;
        e.speed = ENEMY_BASE_SPEED * speedMult;
        e.timeDrop = 2.0f;
    } else {
        e.shape = sf::CircleShape(25.0f);
        e.baseColor = sf::Color(200, 80, 80);
        e.hp = 5; e.maxHp = 5;
        e.speed = ENEMY_BASE_SPEED * 0.6f * speedMult;
        e.timeDrop = 3.0f;
    }
    e.shape.setFillColor(e.baseColor);
    e.shape.setOrigin({e.shape.getRadius(), e.shape.getRadius()});
    e.shape.setPosition({x, y});
    return e;
}
}

int GameThread::run() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    sf::RenderWindow window(sf::VideoMode({WIN_W, WIN_H}), "Tomato Blaster");
    window.setFramerateLimit(60);

    sf::CircleShape player(PLAYER_RADIUS);
    player.setFillColor(sf::Color(220, 50, 50));
    player.setOrigin({PLAYER_RADIUS, PLAYER_RADIUS});
    player.setPosition({WIN_W / 2.0f, WIN_H / 2.0f});

    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    std::vector<Particle> particles;
    std::vector<TimeDrop> timeDrops;

    float timeLeft = TOTAL_TIME;
    float gameTime = 0.0f;
    float spawnTimer = 0.0f;
    float spawnInterval = SPAWN_INTERVAL_START;
    int score = 0;
    int comboCount = 0;
    float comboTimer = 0.0f;
    float shakeTimer = 0.0f;
    bool gameOver = false;
    float shootCooldown = 0.0f;

    sf::Font font;
    bool fontLoaded = loadFont(font);

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        if (dt > 0.05f) dt = 0.05f;

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape)
                    window.close();
                if (gameOver && key->code == sf::Keyboard::Key::R) {
                    timeLeft = TOTAL_TIME;
                    gameTime = 0.0f;
                    score = 0;
                    comboCount = 0;
                    spawnInterval = SPAWN_INTERVAL_START;
                    bullets.clear();
                    enemies.clear();
                    particles.clear();
                    timeDrops.clear();
                    player.setPosition({WIN_W / 2.0f, WIN_H / 2.0f});
                    gameOver = false;
                }
            }
        }

        if (gameOver) {
            window.clear(sf::Color(30, 10, 10));
            if (fontLoaded) {
                sf::Text gameOverText(font, "GAME OVER", 60);
                gameOverText.setFillColor(sf::Color::Red);
                gameOverText.setPosition({WIN_W / 2.0f - 180.0f, WIN_H / 2.0f - 80.0f});
                window.draw(gameOverText);

                sf::Text scoreText(font, "Score: " + std::to_string(score), 30);
                scoreText.setFillColor(sf::Color::White);
                scoreText.setPosition({WIN_W / 2.0f - 80.0f, WIN_H / 2.0f});
                window.draw(scoreText);

                sf::Text restartText(font, "Appuie sur R pour recommencer", 20);
                restartText.setFillColor(sf::Color(200, 200, 200));
                restartText.setPosition({WIN_W / 2.0f - 150.0f, WIN_H / 2.0f + 50.0f});
                window.draw(restartText);
            }
            window.display();
            continue;
        }

        gameTime += dt;
        timeLeft -= dt;
        if (timeLeft <= 0.0f) {
            timeLeft = 0.0f;
            gameOver = true;
        }

        sf::Vector2f moveDir = {0.0f, 0.0f};
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
            moveDir.y -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
            moveDir.y += 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            moveDir.x -= 1.0f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            moveDir.x += 1.0f;

        if (vecLength(moveDir) > 0.0f)
            moveDir = vecNormalize(moveDir);

        sf::Vector2f playerPos = player.getPosition();
        playerPos += moveDir * PLAYER_SPEED * dt;
        playerPos.x = std::clamp(playerPos.x, PLAYER_RADIUS, static_cast<float>(WIN_W) - PLAYER_RADIUS);
        playerPos.y = std::clamp(playerPos.y, PLAYER_RADIUS, static_cast<float>(WIN_H) - PLAYER_RADIUS);
        player.setPosition(playerPos);

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF = {static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)};
        sf::Vector2f aimDir = vecNormalize(mousePosF - playerPos);
        float aimAngle = std::atan2(aimDir.y, aimDir.x) * 180.0f / 3.14159f;
        player.setRotation(sf::degrees(aimAngle));

        shootCooldown -= dt;
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && shootCooldown <= 0.0f) {
            if (timeLeft > BULLET_TIME_COST) {
                Bullet b;
                b.shape = sf::CircleShape(BULLET_RADIUS);
                b.shape.setFillColor(sf::Color(220, 50, 30));
                b.shape.setOrigin({BULLET_RADIUS, BULLET_RADIUS});
                b.shape.setPosition(playerPos + aimDir * (PLAYER_RADIUS + BULLET_RADIUS + 2.0f));
                b.velocity = aimDir * BULLET_SPEED;
                bullets.push_back(b);
                timeLeft -= BULLET_TIME_COST;
                shootCooldown = 0.15f;
                shakeTimer = SHAKE_DURATION * 0.3f;
            }
        }

        for (auto& b : bullets) {
            sf::Vector2f pos = b.shape.getPosition();
            pos += b.velocity * dt;
            b.shape.setPosition(pos);
            if (pos.x < -50 || pos.x > WIN_W + 50 || pos.y < -50 || pos.y > WIN_H + 50)
                b.alive = false;
        }

        spawnTimer += dt;
        spawnInterval = std::max(SPAWN_INTERVAL_MIN, SPAWN_INTERVAL_START - gameTime / 60.0f);
        if (spawnTimer >= spawnInterval) {
            spawnTimer = 0.0f;
            int type;
            int roll = std::rand() % 100;
            if (gameTime < 30.0f) {
                type = 0;
            } else if (gameTime < 60.0f) {
                type = (roll < 70) ? 0 : 1;
            } else {
                if (roll < 50) type = 0;
                else if (roll < 85) type = 1;
                else type = 2;
            }
            enemies.push_back(createEnemy(type, gameTime));
        }

        for (auto& e : enemies) {
            sf::Vector2f ePos = e.shape.getPosition();
            sf::Vector2f dir = vecNormalize(playerPos - ePos);
            ePos += dir * e.speed * dt;
            e.shape.setPosition(ePos);

            if (e.flashTimer > 0.0f) {
                e.flashTimer -= dt;
                if (e.flashTimer <= 0.0f)
                    e.shape.setFillColor(e.baseColor);
            }

            float collDist = PLAYER_RADIUS + e.shape.getRadius();
            if (distanceSq(ePos, playerPos) < collDist * collDist) {
                timeLeft -= 3.0f;
                e.alive = false;
                shakeTimer = SHAKE_DURATION;
                spawnParticles(particles, ePos, 15, sf::Color(200, 50, 50));
            }
        }

        comboTimer -= dt;
        if (comboTimer <= 0.0f) comboCount = 0;

        for (auto& b : bullets) {
            if (!b.alive) continue;
            sf::Vector2f bPos = b.shape.getPosition();
            for (auto& e : enemies) {
                if (!e.alive) continue;
                float collDist = BULLET_RADIUS + e.shape.getRadius();
                if (distanceSq(bPos, e.shape.getPosition()) < collDist * collDist) {
                    b.alive = false;
                    e.hp -= 1.0f;
                    e.flashTimer = 0.08f;
                    e.shape.setFillColor(sf::Color::White);
                    shakeTimer = SHAKE_DURATION * 0.5f;
                    spawnParticles(particles, bPos, 8, sf::Color(220, 50, 30));

                    if (e.hp <= 0.0f) {
                        e.alive = false;
                        comboCount++;
                        comboTimer = 1.0f;
                        float dropValue = e.timeDrop;
                        if (comboCount >= 3)
                            dropValue *= 1.0f + (comboCount - 2) * 0.25f;
                        score += static_cast<int>(10 * e.maxHp * (1.0f + comboCount * 0.1f));
                        spawnParticles(particles, e.shape.getPosition(), 20, e.baseColor);

                        TimeDrop td;
                        td.shape = sf::CircleShape(10.0f);
                        td.shape.setFillColor(sf::Color(255, 80, 80));
                        td.shape.setOutlineColor(sf::Color(255, 200, 50));
                        td.shape.setOutlineThickness(2.0f);
                        td.shape.setOrigin({10.0f, 10.0f});
                        td.shape.setPosition(e.shape.getPosition());
                        td.value = dropValue;
                        timeDrops.push_back(td);
                    }
                    break;
                }
            }
        }

        for (auto& td : timeDrops) {
            td.lifetime -= dt;
            if (td.lifetime <= 0.0f) { td.alive = false; continue; }

            float alpha = std::min(1.0f, td.lifetime / 0.5f);
            sf::Color c = td.shape.getFillColor();
            c.a = static_cast<uint8_t>(alpha * 255);
            td.shape.setFillColor(c);

            float distSq = distanceSq(td.shape.getPosition(), playerPos);
            if (distSq < (PLAYER_RADIUS + 15.0f) * (PLAYER_RADIUS + 15.0f)) {
                timeLeft = std::min(TOTAL_TIME, timeLeft + td.value);
                td.alive = false;
                spawnParticles(particles, td.shape.getPosition(), 10, sf::Color(255, 200, 50));
            }
        }

        for (auto& p : particles) {
            p.lifetime -= dt;
            if (p.lifetime <= 0.0f) { p.alive = false; continue; }
            sf::Vector2f pos = p.shape.getPosition();
            pos += p.velocity * dt;
            p.shape.setPosition(pos);
            p.velocity *= 0.95f;
            float alpha = p.lifetime / p.maxLifetime;
            sf::Color c = p.shape.getFillColor();
            c.a = static_cast<uint8_t>(alpha * 255);
            p.shape.setFillColor(c);
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.alive; }), bullets.end());
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) { return !e.alive; }), enemies.end());
        particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& p) { return !p.alive; }), particles.end());
        timeDrops.erase(std::remove_if(timeDrops.begin(), timeDrops.end(), [](const TimeDrop& td) { return !td.alive; }), timeDrops.end());

        sf::Vector2f shakeOffset = {0.0f, 0.0f};
        if (shakeTimer > 0.0f) {
            shakeTimer -= dt;
            shakeOffset.x = static_cast<float>((std::rand() % 100) - 50) / 50.0f * SHAKE_INTENSITY;
            shakeOffset.y = static_cast<float>((std::rand() % 100) - 50) / 50.0f * SHAKE_INTENSITY;
        }

        sf::Color bgColor(20, 15, 25);
        if (timeLeft < 30.0f) {
            float pulse = (std::sin(gameTime * 8.0f) + 1.0f) / 2.0f;
            bgColor.r = static_cast<uint8_t>(20 + pulse * 30);
        }
        window.clear(bgColor);

        sf::View view = window.getDefaultView();
        view.setCenter(view.getCenter() + shakeOffset);
        window.setView(view);

        for (auto& td : timeDrops)
            window.draw(td.shape);
        for (auto& p : particles)
            window.draw(p.shape);
        for (auto& e : enemies)
            window.draw(e.shape);
        for (auto& b : bullets)
            window.draw(b.shape);
        window.draw(player);

        window.setView(window.getDefaultView());

        if (fontLoaded) {
            int minutes = static_cast<int>(timeLeft) / 60;
            int seconds = static_cast<int>(timeLeft) % 60;
            int centis = static_cast<int>((timeLeft - std::floor(timeLeft)) * 100);
            std::string timeStr = std::to_string(minutes) + ":" +
                (seconds < 10 ? "0" : "") + std::to_string(seconds) + "." +
                (centis < 10 ? "0" : "") + std::to_string(centis);

            sf::Text timerText(font, timeStr, 36);
            if (timeLeft < 30.0f)
                timerText.setFillColor(sf::Color::Red);
            else
                timerText.setFillColor(sf::Color::White);
            timerText.setPosition({WIN_W / 2.0f - 70.0f, 10.0f});
            window.draw(timerText);

            float barWidth = 300.0f;
            float barHeight = 8.0f;
            float barX = WIN_W / 2.0f - barWidth / 2.0f;
            float barY = 55.0f;
            sf::RectangleShape barBg({barWidth, barHeight});
            barBg.setFillColor(sf::Color(60, 60, 60));
            barBg.setPosition({barX, barY});
            window.draw(barBg);

            float fillW = barWidth * (timeLeft / TOTAL_TIME);
            sf::RectangleShape barFill({fillW, barHeight});
            if (timeLeft < 30.0f)
                barFill.setFillColor(sf::Color::Red);
            else if (timeLeft < 60.0f)
                barFill.setFillColor(sf::Color(255, 165, 0));
            else
                barFill.setFillColor(sf::Color::Green);
            barFill.setPosition({barX, barY});
            window.draw(barFill);

            sf::Text scoreText(font, "Score: " + std::to_string(score), 24);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition({10.0f, 10.0f});
            window.draw(scoreText);

            if (comboCount >= 3) {
                sf::Text comboText(font, "COMBO x" + std::to_string(comboCount), 20);
                comboText.setFillColor(sf::Color(255, 200, 50));
                comboText.setPosition({10.0f, 40.0f});
                window.draw(comboText);
            }
        }

        window.display();
    }

    return 0;
}
