#include "game/GameThread.hpp"

#include "actors/Actor.hpp"
#include "audio/MusicManager.hpp"
#include "game/BonusSystem.hpp"
#include "game/GameConfig.hpp"
#include "game/LevelManager.hpp"
#include "utils/FontLoader.hpp"
#include "utils/Math.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <optional>
#include <string>
#include <vector>

namespace {
enum class GameState {
    Menu,
    Options,
    Playing,
    GameOver
};

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

void drawTomato(sf::RenderWindow& window, const sf::CircleShape& body) {
    sf::Vector2f pos = body.getPosition();
    sf::Vector2f sc = body.getScale();
    float r = PLAYER_RADIUS;

    window.draw(body);

    sf::CircleShape shade(r * 0.7f);
    shade.setFillColor(sf::Color(180, 30, 30));
    shade.setOrigin({r * 0.7f, r * 0.7f});
    shade.setPosition({pos.x + r * 0.15f * sc.x, pos.y + r * 0.2f * sc.y});
    shade.setScale(sc);
    window.draw(shade);

    sf::CircleShape highlight(r * 0.25f);
    highlight.setFillColor(sf::Color(255, 130, 130, 160));
    highlight.setOrigin({r * 0.25f, r * 0.25f});
    highlight.setPosition({pos.x - r * 0.35f * sc.x, pos.y - r * 0.35f * sc.y});
    highlight.setScale(sc);
    window.draw(highlight);

    sf::CircleShape highlight2(r * 0.12f);
    highlight2.setFillColor(sf::Color(255, 180, 180, 120));
    highlight2.setOrigin({r * 0.12f, r * 0.12f});
    highlight2.setPosition({pos.x - r * 0.2f * sc.x, pos.y - r * 0.5f * sc.y});
    highlight2.setScale(sc);
    window.draw(highlight2);

    sf::RectangleShape stem({r * 0.15f, r * 0.4f});
    stem.setFillColor(sf::Color(80, 140, 40));
    stem.setOrigin({r * 0.075f, r * 0.4f});
    stem.setPosition({pos.x, pos.y - r * 0.85f * sc.y});
    stem.setScale(sc);
    window.draw(stem);

    sf::ConvexShape leaf1(4);
    leaf1.setPoint(0, {0.0f, 0.0f});
    leaf1.setPoint(1, {r * 0.5f, -r * 0.15f});
    leaf1.setPoint(2, {r * 0.55f, -r * 0.05f});
    leaf1.setPoint(3, {r * 0.1f, r * 0.1f});
    leaf1.setFillColor(sf::Color(60, 160, 40));
    leaf1.setPosition({pos.x, pos.y - r * 0.85f * sc.y});
    leaf1.setScale(sc);
    window.draw(leaf1);

    sf::ConvexShape leaf2(4);
    leaf2.setPoint(0, {0.0f, 0.0f});
    leaf2.setPoint(1, {-r * 0.45f, -r * 0.2f});
    leaf2.setPoint(2, {-r * 0.5f, -r * 0.08f});
    leaf2.setPoint(3, {-r * 0.08f, r * 0.1f});
    leaf2.setFillColor(sf::Color(50, 150, 35));
    leaf2.setPosition({pos.x, pos.y - r * 0.85f * sc.y});
    leaf2.setScale(sc);
    window.draw(leaf2);

    sf::ConvexShape leaf3(4);
    leaf3.setPoint(0, {0.0f, 0.0f});
    leaf3.setPoint(1, {r * 0.3f, -r * 0.3f});
    leaf3.setPoint(2, {r * 0.35f, -r * 0.18f});
    leaf3.setPoint(3, {r * 0.05f, r * 0.05f});
    leaf3.setFillColor(sf::Color(70, 170, 45));
    leaf3.setPosition({pos.x - r * 0.05f, pos.y - r * 0.9f * sc.y});
    leaf3.setScale(sc);
    window.draw(leaf3);
}

void drawEnemy(sf::RenderWindow& window, const Enemy& e, sf::Vector2f playerPos) {
    sf::Vector2f ePos = e.shape.getPosition();
    sf::Vector2f sc = e.shape.getScale();
    float r = e.shape.getRadius();
    sf::Vector2f dir = vecNormalize(playerPos - ePos);
    sf::Vector2f perp = {-dir.y, dir.x};

    window.draw(e.shape);

    if (e.maxHp <= 1) {
        sf::CircleShape seg1(r * 0.65f);
        seg1.setFillColor(sf::Color(60, 150, 60));
        seg1.setOrigin({r * 0.65f, r * 0.65f});
        seg1.setPosition(ePos - dir * r * 0.9f * sc.x);
        seg1.setScale(sc);
        window.draw(seg1);

        sf::CircleShape seg2(r * 0.5f);
        seg2.setFillColor(sf::Color(50, 130, 50));
        seg2.setOrigin({r * 0.5f, r * 0.5f});
        seg2.setPosition(ePos - dir * r * 1.6f * sc.x);
        seg2.setScale(sc);
        window.draw(seg2);

        sf::CircleShape eyeL(r * 0.22f);
        eyeL.setFillColor(sf::Color::White);
        eyeL.setOrigin({r * 0.22f, r * 0.22f});
        eyeL.setPosition(ePos + dir * r * 0.5f * sc.x + perp * r * 0.35f * sc.x);
        eyeL.setScale(sc);
        window.draw(eyeL);

        sf::CircleShape eyeR(r * 0.22f);
        eyeR.setFillColor(sf::Color::White);
        eyeR.setOrigin({r * 0.22f, r * 0.22f});
        eyeR.setPosition(ePos + dir * r * 0.5f * sc.x - perp * r * 0.35f * sc.x);
        eyeR.setScale(sc);
        window.draw(eyeR);

        sf::CircleShape pupilL(r * 0.12f);
        pupilL.setFillColor(sf::Color::Black);
        pupilL.setOrigin({r * 0.12f, r * 0.12f});
        pupilL.setPosition(ePos + dir * r * 0.6f * sc.x + perp * r * 0.35f * sc.x);
        pupilL.setScale(sc);
        window.draw(pupilL);

        sf::CircleShape pupilR(r * 0.12f);
        pupilR.setFillColor(sf::Color::Black);
        pupilR.setOrigin({r * 0.12f, r * 0.12f});
        pupilR.setPosition(ePos + dir * r * 0.6f * sc.x - perp * r * 0.35f * sc.x);
        pupilR.setScale(sc);
        window.draw(pupilR);

        sf::RectangleShape antL({r * 0.08f, r * 0.6f});
        antL.setFillColor(sf::Color(40, 100, 40));
        antL.setOrigin({r * 0.04f, r * 0.6f});
        antL.setPosition(ePos + dir * r * 0.7f * sc.x + perp * r * 0.25f * sc.x);
        antL.setScale(sc);
        window.draw(antL);

        sf::RectangleShape antR({r * 0.08f, r * 0.6f});
        antR.setFillColor(sf::Color(40, 100, 40));
        antR.setOrigin({r * 0.04f, r * 0.6f});
        antR.setPosition(ePos + dir * r * 0.7f * sc.x - perp * r * 0.25f * sc.x);
        antR.setScale(sc);
        window.draw(antR);

    } else if (e.maxHp <= 3) {
        sf::CircleShape abdomen(r * 0.8f);
        abdomen.setFillColor(sf::Color(220, 200, 50));
        abdomen.setOrigin({r * 0.8f, r * 0.8f});
        abdomen.setPosition(ePos - dir * r * 1.0f * sc.x);
        abdomen.setScale(sc);
        window.draw(abdomen);

        sf::RectangleShape stripe1({r * 1.4f, r * 0.15f});
        stripe1.setFillColor(sf::Color::Black);
        stripe1.setOrigin({r * 0.7f, r * 0.075f});
        stripe1.setPosition(ePos - dir * r * 0.8f * sc.x);
        stripe1.setScale(sc);
        window.draw(stripe1);

        sf::RectangleShape stripe2({r * 1.2f, r * 0.15f});
        stripe2.setFillColor(sf::Color::Black);
        stripe2.setOrigin({r * 0.6f, r * 0.075f});
        stripe2.setPosition(ePos - dir * r * 1.2f * sc.x);
        stripe2.setScale(sc);
        window.draw(stripe2);

        sf::ConvexShape wingL(4);
        wingL.setPoint(0, {0.0f, 0.0f});
        wingL.setPoint(1, {perp.x * r * 1.2f + dir.x * r * 0.3f, perp.y * r * 1.2f + dir.y * r * 0.3f});
        wingL.setPoint(2, {perp.x * r * 1.0f - dir.x * r * 0.4f, perp.y * r * 1.0f - dir.y * r * 0.4f});
        wingL.setPoint(3, {-dir.x * r * 0.2f, -dir.y * r * 0.2f});
        wingL.setFillColor(sf::Color(255, 255, 255, 80));
        wingL.setOutlineColor(sf::Color(200, 200, 200, 120));
        wingL.setOutlineThickness(1.0f);
        wingL.setPosition(ePos + dir * r * 0.2f * sc.x);
        wingL.setScale(sc);
        window.draw(wingL);

        sf::ConvexShape wingR(4);
        wingR.setPoint(0, {0.0f, 0.0f});
        wingR.setPoint(1, {-perp.x * r * 1.2f + dir.x * r * 0.3f, -perp.y * r * 1.2f + dir.y * r * 0.3f});
        wingR.setPoint(2, {-perp.x * r * 1.0f - dir.x * r * 0.4f, -perp.y * r * 1.0f - dir.y * r * 0.4f});
        wingR.setPoint(3, {-dir.x * r * 0.2f, -dir.y * r * 0.2f});
        wingR.setFillColor(sf::Color(255, 255, 255, 80));
        wingR.setOutlineColor(sf::Color(200, 200, 200, 120));
        wingR.setOutlineThickness(1.0f);
        wingR.setPosition(ePos + dir * r * 0.2f * sc.x);
        wingR.setScale(sc);
        window.draw(wingR);

        sf::ConvexShape dard(3);
        dard.setPoint(0, {-perp.x * r * 0.15f, -perp.y * r * 0.15f});
        dard.setPoint(1, {perp.x * r * 0.15f, perp.y * r * 0.15f});
        dard.setPoint(2, {-dir.x * r * 0.5f, -dir.y * r * 0.5f});
        dard.setFillColor(sf::Color(60, 60, 60));
        dard.setPosition(ePos - dir * r * 1.6f * sc.x);
        dard.setScale(sc);
        window.draw(dard);

        sf::CircleShape eyeL(r * 0.2f);
        eyeL.setFillColor(sf::Color(200, 30, 30));
        eyeL.setOrigin({r * 0.2f, r * 0.2f});
        eyeL.setPosition(ePos + dir * r * 0.55f * sc.x + perp * r * 0.35f * sc.x);
        eyeL.setScale(sc);
        window.draw(eyeL);

        sf::CircleShape eyeR(r * 0.2f);
        eyeR.setFillColor(sf::Color(200, 30, 30));
        eyeR.setOrigin({r * 0.2f, r * 0.2f});
        eyeR.setPosition(ePos + dir * r * 0.55f * sc.x - perp * r * 0.35f * sc.x);
        eyeR.setScale(sc);
        window.draw(eyeR);

    } else {
        sf::RectangleShape carapace({r * 0.06f, r * 1.6f});
        carapace.setFillColor(sf::Color::Black);
        carapace.setOrigin({r * 0.03f, r * 0.8f});
        carapace.setPosition(ePos);
        carapace.setScale(sc);
        window.draw(carapace);

        sf::ConvexShape mandL(3);
        mandL.setPoint(0, {0.0f, 0.0f});
        mandL.setPoint(1, {dir.x * r * 0.5f + perp.x * r * 0.25f, dir.y * r * 0.5f + perp.y * r * 0.25f});
        mandL.setPoint(2, {dir.x * r * 0.4f - perp.x * r * 0.05f, dir.y * r * 0.4f - perp.y * r * 0.05f});
        mandL.setFillColor(sf::Color(80, 40, 20));
        mandL.setPosition(ePos + dir * r * 0.8f * sc.x + perp * r * 0.15f * sc.x);
        mandL.setScale(sc);
        window.draw(mandL);

        sf::ConvexShape mandR(3);
        mandR.setPoint(0, {0.0f, 0.0f});
        mandR.setPoint(1, {dir.x * r * 0.5f - perp.x * r * 0.25f, dir.y * r * 0.5f - perp.y * r * 0.25f});
        mandR.setPoint(2, {dir.x * r * 0.4f + perp.x * r * 0.05f, dir.y * r * 0.4f + perp.y * r * 0.05f});
        mandR.setFillColor(sf::Color(80, 40, 20));
        mandR.setPosition(ePos + dir * r * 0.8f * sc.x - perp * r * 0.15f * sc.x);
        mandR.setScale(sc);
        window.draw(mandR);

        for (int i = 0; i < 3; ++i) {
            float offset = (i - 1) * r * 0.5f;
            sf::RectangleShape legL({r * 0.6f, r * 0.1f});
            legL.setFillColor(sf::Color(60, 30, 15));
            legL.setOrigin({0.0f, r * 0.05f});
            legL.setPosition(ePos + perp * r * 0.85f * sc.x + dir * offset * sc.x);
            legL.setScale(sc);
            window.draw(legL);

            sf::RectangleShape legR({r * 0.6f, r * 0.1f});
            legR.setFillColor(sf::Color(60, 30, 15));
            legR.setOrigin({r * 0.6f, r * 0.05f});
            legR.setPosition(ePos - perp * r * 0.85f * sc.x + dir * offset * sc.x);
            legR.setScale(sc);
            window.draw(legR);
        }

        sf::CircleShape eyeL(r * 0.2f);
        eyeL.setFillColor(sf::Color(230, 200, 50));
        eyeL.setOrigin({r * 0.2f, r * 0.2f});
        eyeL.setPosition(ePos + dir * r * 0.5f * sc.x + perp * r * 0.35f * sc.x);
        eyeL.setScale(sc);
        window.draw(eyeL);

        sf::CircleShape eyeR(r * 0.2f);
        eyeR.setFillColor(sf::Color(230, 200, 50));
        eyeR.setOrigin({r * 0.2f, r * 0.2f});
        eyeR.setPosition(ePos + dir * r * 0.5f * sc.x - perp * r * 0.35f * sc.x);
        eyeR.setScale(sc);
        window.draw(eyeR);
    }
}

Enemy createEnemy(int type, float difficultyTime) {
    Enemy e;
    float side = static_cast<float>(std::rand() % 4);
    float x, y;
    if (side < 1) { x = -30; y = static_cast<float>(std::rand() % WIN_H); }
    else if (side < 2) { x = WIN_W + 30.0f; y = static_cast<float>(std::rand() % WIN_H); }
    else if (side < 3) { x = static_cast<float>(std::rand() % WIN_W); y = -30; }
    else { x = static_cast<float>(std::rand() % WIN_W); y = WIN_H + 30.0f; }

    float speedMult = 1.0f + difficultyTime / 120.0f;

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
    e.spawnAge = 0.0f;
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
    std::vector<Shockwave> shockwaves;
    std::vector<FloatingText> floatingTexts;
    BonusSystem bonusSystem;

    float timeLeft = TOTAL_TIME;
    float gameTime = 0.0f;
    float spawnTimer = 0.0f;
    float spawnInterval = SPAWN_INTERVAL_START;
    int score = 0;
    int comboCount = 0;
    float comboTimer = 0.0f;
    float shakeTimer = 0.0f;
    float shootCooldown = 0.0f;
    float speedBoostTimer = 0.0f;
    float rapidFireTimer = 0.0f;
    float pierceTimer = 0.0f;
    float hitstopTimer = 0.0f;
    float slowMoTimer = 0.0f;
    float flashAlpha = 0.0f;
    float shootSquash = 0.0f;
    LevelManager level;
    GameState state = GameState::Menu;
    int menuIndex = 0;
    int optionsIndex = 0;
    float musicVolume = 70.0f;

    sf::Font font;
    bool fontLoaded = loadFont(font);

    sf::Texture menuTexture;
    std::optional<sf::Sprite> menuSprite;
    bool menuBgLoaded = menuTexture.loadFromFile("assets/Menu.png");
    if (menuBgLoaded) {
        menuSprite.emplace(menuTexture);
        sf::Vector2u size = menuTexture.getSize();
        if (size.x > 0 && size.y > 0) {
            menuSprite->setScale(sf::Vector2f{
                static_cast<float>(WIN_W) / static_cast<float>(size.x),
                static_cast<float>(WIN_H) / static_cast<float>(size.y)
            });
        }
    }

    sf::SoundBuffer menuBuffer;
    std::optional<sf::Sound> menuSound;
    bool menuSoundLoaded = menuBuffer.loadFromFile("music/menu.ogg");
    if (!menuSoundLoaded)
        menuSoundLoaded = menuBuffer.loadFromFile("music/Menu.ogg");
    if (menuSoundLoaded) {
        menuSound.emplace(menuBuffer);
        menuSound->setVolume(musicVolume);
    }

    std::optional<sf::Music> menuMusic;
    bool menuMusicLoaded = false;
    menuMusic.emplace();
    if (!menuMusic->openFromFile("music/menu.ogg") && !menuMusic->openFromFile("music/Menu.ogg")) {
        menuMusic.reset();
    } else {
        menuMusicLoaded = true;
        menuMusic->setLooping(true);
        menuMusic->setVolume(musicVolume);
    }
    if (menuMusicLoaded)
        menuSoundLoaded = false;

    MusicManager music;
    music.loadFromJson("music/music.json");
    music.setVolume(musicVolume);
    music.stop();

    sf::Clock clock;

    auto resetGame = [&]() {
        timeLeft = TOTAL_TIME;
        gameTime = 0.0f;
        spawnTimer = 0.0f;
        spawnInterval = SPAWN_INTERVAL_START;
        score = 0;
        comboCount = 0;
        comboTimer = 0.0f;
        shakeTimer = 0.0f;
        shootCooldown = 0.0f;
        speedBoostTimer = 0.0f;
        rapidFireTimer = 0.0f;
        pierceTimer = 0.0f;
        hitstopTimer = 0.0f;
        slowMoTimer = 0.0f;
        flashAlpha = 0.0f;
        shootSquash = 0.0f;
        bullets.clear();
        enemies.clear();
        particles.clear();
        shockwaves.clear();
        floatingTexts.clear();
        bonusSystem.clear();
        level.reset();
        player.setPosition({WIN_W / 2.0f, WIN_H / 2.0f});
        player.setScale({1.0f, 1.0f});
        music.reset(timeLeft);
        music.setVolume(musicVolume);
    };

    while (window.isOpen()) {
        auto playMenuSound = [&]() {
            if (menuSoundLoaded)
                menuSound->play();
        };

        float rawDt = clock.restart().asSeconds();
        if (rawDt > 0.05f) rawDt = 0.05f;

        float dt = rawDt;
        if (hitstopTimer > 0.0f) {
            hitstopTimer -= rawDt;
            dt = 0.0f;
        } else if (slowMoTimer > 0.0f) {
            slowMoTimer -= rawDt;
            dt *= SLOWMO_SCALE;
        }

        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (state == GameState::Menu) {
                    if (key->code == sf::Keyboard::Key::Escape)
                        window.close();
                    else if (key->code == sf::Keyboard::Key::Up || key->code == sf::Keyboard::Key::Z || key->code == sf::Keyboard::Key::W) {
                        menuIndex = (menuIndex + 2) % 3;
                        playMenuSound();
                    } else if (key->code == sf::Keyboard::Key::Down || key->code == sf::Keyboard::Key::S) {
                        menuIndex = (menuIndex + 1) % 3;
                        playMenuSound();
                    }
                    else if (key->code == sf::Keyboard::Key::Enter || key->code == sf::Keyboard::Key::Space) {
                        playMenuSound();
                        if (menuIndex == 0) {
                            if (menuMusicLoaded)
                                menuMusic->stop();
                            resetGame();
                            state = GameState::Playing;
                        } else if (menuIndex == 1) {
                            state = GameState::Options;
                        } else if (menuIndex == 2) {
                            window.close();
                        }
                    }
                } else if (state == GameState::Options) {
                    if (key->code == sf::Keyboard::Key::Escape)
                        state = GameState::Menu;
                    else if (key->code == sf::Keyboard::Key::Up || key->code == sf::Keyboard::Key::Z || key->code == sf::Keyboard::Key::W) {
                        optionsIndex = (optionsIndex + 1) % 2;
                        playMenuSound();
                    } else if (key->code == sf::Keyboard::Key::Down || key->code == sf::Keyboard::Key::S) {
                        optionsIndex = (optionsIndex + 1) % 2;
                        playMenuSound();
                    } else if (optionsIndex == 0 && (key->code == sf::Keyboard::Key::Left || key->code == sf::Keyboard::Key::Q || key->code == sf::Keyboard::Key::A)) {
                        musicVolume = std::max(0.0f, musicVolume - 5.0f);
                        music.setVolume(musicVolume);
                        if (menuSoundLoaded)
                            menuSound->setVolume(musicVolume);
                        if (menuMusicLoaded)
                            menuMusic->setVolume(musicVolume);
                        playMenuSound();
                    } else if (optionsIndex == 0 && (key->code == sf::Keyboard::Key::Right || key->code == sf::Keyboard::Key::D)) {
                        musicVolume = std::min(100.0f, musicVolume + 5.0f);
                        music.setVolume(musicVolume);
                        if (menuSoundLoaded)
                            menuSound->setVolume(musicVolume);
                        if (menuMusicLoaded)
                            menuMusic->setVolume(musicVolume);
                        playMenuSound();
                    } else if (key->code == sf::Keyboard::Key::Enter || key->code == sf::Keyboard::Key::Space) {
                        playMenuSound();
                        if (optionsIndex == 1)
                            state = GameState::Menu;
                    }
                } else if (state == GameState::Playing) {
                    if (key->code == sf::Keyboard::Key::Escape) {
                        state = GameState::Menu;
                        music.stop();
                    }
                } else if (state == GameState::GameOver) {
                    if (key->code == sf::Keyboard::Key::R || key->code == sf::Keyboard::Key::Enter) {
                        if (menuMusicLoaded)
                            menuMusic->stop();
                        resetGame();
                        state = GameState::Playing;
                    } else if (key->code == sf::Keyboard::Key::Escape) {
                        state = GameState::Menu;
                        music.stop();
                    }
                }
            }
        }

        if (state == GameState::Menu) {
            if (menuMusicLoaded && menuMusic->getStatus() != sf::SoundSource::Status::Playing)
                menuMusic->play();
            window.clear(sf::Color(20, 15, 25));
            if (menuBgLoaded)
                window.draw(*menuSprite);
            if (fontLoaded) {
                const char* items[] = {"START", "OPTIONS", "QUIT"};
                for (int i = 0; i < 3; ++i) {
                    sf::Text t(font, items[i], 28);
                    t.setFillColor(i == menuIndex ? sf::Color(255, 220, 120) : sf::Color(220, 220, 220));
                    sf::FloatRect b = t.getLocalBounds();
                    t.setOrigin(sf::Vector2f{b.position.x + b.size.x / 2.0f, b.position.y + b.size.y / 2.0f});
                    t.setPosition({WIN_W / 2.0f, 260.0f + i * 46.0f});
                    window.draw(t);
                }
            }
            window.display();
            continue;
        }

        if (state == GameState::Options) {
            if (menuMusicLoaded && menuMusic->getStatus() != sf::SoundSource::Status::Playing)
                menuMusic->play();
            window.clear(sf::Color(18, 14, 22));
            if (menuBgLoaded) {
                menuSprite->setColor(sf::Color(200, 200, 200));
                window.draw(*menuSprite);
                menuSprite->setColor(sf::Color::White);
            }
            if (fontLoaded) {
                sf::Text title(font, "OPTIONS", 42);
                title.setFillColor(sf::Color(230, 230, 230));
                sf::FloatRect tb = title.getLocalBounds();
                title.setOrigin(sf::Vector2f{tb.position.x + tb.size.x / 2.0f, tb.position.y + tb.size.y / 2.0f});
                title.setPosition({WIN_W / 2.0f, 140.0f});
                window.draw(title);

                std::string vol = "MUSIC VOLUME: " + std::to_string(static_cast<int>(musicVolume));
                sf::Text volText(font, vol, 24);
                volText.setFillColor(optionsIndex == 0 ? sf::Color(255, 220, 120) : sf::Color(220, 220, 220));
                sf::FloatRect vb = volText.getLocalBounds();
                volText.setOrigin(sf::Vector2f{vb.position.x + vb.size.x / 2.0f, vb.position.y + vb.size.y / 2.0f});
                volText.setPosition({WIN_W / 2.0f, 260.0f});
                window.draw(volText);

                sf::Text backText(font, "BACK", 24);
                backText.setFillColor(optionsIndex == 1 ? sf::Color(255, 220, 120) : sf::Color(220, 220, 220));
                sf::FloatRect bb = backText.getLocalBounds();
                backText.setOrigin(sf::Vector2f{bb.position.x + bb.size.x / 2.0f, bb.position.y + bb.size.y / 2.0f});
                backText.setPosition({WIN_W / 2.0f, 306.0f});
                window.draw(backText);
            }
            window.display();
            continue;
        }

        if (state == GameState::GameOver) {
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

        level.update(dt);
        bool waveActive = level.isWaveActive();
        music.update(timeLeft, dt);

        gameTime += rawDt;
        if (waveActive) {
            timeLeft -= dt;
            if (timeLeft <= 0.0f) {
                timeLeft = 0.0f;
                state = GameState::GameOver;
                continue;
            }
        }

        if (waveActive) {
            if (speedBoostTimer > 0.0f) speedBoostTimer -= dt;
            if (rapidFireTimer > 0.0f) rapidFireTimer -= dt;
            if (pierceTimer > 0.0f) pierceTimer -= dt;
        }

        float speedMult = speedBoostTimer > 0.0f ? SPEED_BONUS_MULT : 1.0f;
        float shootInterval = rapidFireTimer > 0.0f ? RAPID_FIRE_COOLDOWN : BASE_SHOOT_COOLDOWN;

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
        playerPos += moveDir * (PLAYER_SPEED * speedMult) * dt;
        playerPos.x = std::clamp(playerPos.x, PLAYER_RADIUS, static_cast<float>(WIN_W) - PLAYER_RADIUS);
        playerPos.y = std::clamp(playerPos.y, PLAYER_RADIUS, static_cast<float>(WIN_H) - PLAYER_RADIUS);
        player.setPosition(playerPos);

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePosF = {static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)};
        sf::Vector2f aimDir = vecNormalize(mousePosF - playerPos);

        if (shootSquash > 0.0f) {
            shootSquash -= rawDt;
            if (shootSquash < 0.0f) shootSquash = 0.0f;
            float t = shootSquash / SQUASH_DURATION;
            float sx = 1.0f + (SQUASH_X - 1.0f) * t;
            float sy = 1.0f + (SQUASH_Y - 1.0f) * t;
            player.setScale({sx, sy});
        } else {
            player.setScale({1.0f, 1.0f});
        }

        shootCooldown -= dt;
        if (waveActive && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && shootCooldown <= 0.0f) {
            if (timeLeft > BULLET_TIME_COST) {
                Bullet b;
                float bulletRadius = (pierceTimer > 0.0f) ? (BULLET_RADIUS * BULLET_BIG_MULT) : BULLET_RADIUS;
                b.shape = sf::CircleShape(bulletRadius);
                b.shape.setFillColor(sf::Color(220, 50, 30));
                b.shape.setOrigin({bulletRadius, bulletRadius});
                b.shape.setPosition(playerPos + aimDir * (PLAYER_RADIUS + bulletRadius + 2.0f));
                b.velocity = aimDir * BULLET_SPEED;
                b.piercing = pierceTimer > 0.0f;
                bullets.push_back(b);
                timeLeft -= BULLET_TIME_COST;
                shootCooldown = shootInterval;
                shakeTimer = SHAKE_DURATION * 0.3f;
                shootSquash = SQUASH_DURATION;
            }
        }

        if (waveActive) {
            for (auto& b : bullets) {
                sf::Vector2f pos = b.shape.getPosition();
                pos += b.velocity * dt;
                b.shape.setPosition(pos);
                if (pos.x < -50 || pos.x > WIN_W + 50 || pos.y < -50 || pos.y > WIN_H + 50)
                    b.alive = false;
                else
                    spawnBulletTrail(particles, pos, sf::Color(180, 40, 20, 150));
            }
        }

        if (waveActive) {
            spawnTimer += dt;
            float spawnBias = static_cast<float>(level.waveIndex()) * 0.05f;
            float timePressure = 1.0f - (timeLeft / TOTAL_TIME);
            float pressureBoost = TIME_LEFT_SPAWN_BOOST * timePressure;
            spawnInterval = std::max(SPAWN_INTERVAL_MIN, SPAWN_INTERVAL_START - level.waveTime() / 60.0f - spawnBias - pressureBoost);
            if (spawnTimer >= spawnInterval) {
                spawnTimer = 0.0f;
                int waveBonus = level.waveIndex() / 3;
                int pressureBonus = static_cast<int>(timePressure * 2.0f);
                int spawnCount = std::clamp(1 + waveBonus + pressureBonus, 1, 5);
                for (int i = 0; i < spawnCount; ++i) {
                    int type;
                    int roll = std::rand() % 100;
                    if (level.waveIndex() < 2) {
                        type = 0;
                    } else if (level.waveIndex() < 4) {
                        type = (roll < 70) ? 0 : 1;
                    } else {
                        if (roll < 50) type = 0;
                        else if (roll < 85) type = 1;
                        else type = 2;
                    }
                    float difficultyTime = level.waveTime() + static_cast<float>(level.waveIndex()) * 20.0f;
                    enemies.push_back(createEnemy(type, difficultyTime));
                }
            }
        }

        if (waveActive) {
            for (auto& e : enemies) {
                e.spawnAge += rawDt;

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
        }

        if (waveActive) {
            comboTimer -= dt;
            if (comboTimer <= 0.0f) comboCount = 0;
        }

        if (waveActive) {
            for (auto& b : bullets) {
                if (!b.alive) continue;
                sf::Vector2f bPos = b.shape.getPosition();
                for (auto& e : enemies) {
                    if (!e.alive) continue;
                    float bulletRadius = b.shape.getRadius();
                    float collDist = bulletRadius + e.shape.getRadius();
                    if (distanceSq(bPos, e.shape.getPosition()) < collDist * collDist) {
                        e.hp -= 1.0f;
                        e.flashTimer = 0.08f;
                        e.shape.setFillColor(sf::Color::White);
                        shakeTimer = SHAKE_DURATION * 0.5f;
                        spawnParticles(particles, bPos, 8, sf::Color(220, 50, 30));

                        if (b.piercing) {
                            sf::Vector2f dir = vecNormalize(b.velocity);
                            b.shape.setPosition(bPos + dir * (collDist + 2.0f));
                        } else {
                            b.alive = false;
                        }

                        if (e.hp <= 0.0f) {
                            e.alive = false;
                            comboCount++;
                            comboTimer = 1.0f;
                            float dropValue = e.timeDrop;
                            if (comboCount >= 3)
                                dropValue *= 1.0f + (comboCount - 2) * 0.25f;
                            int killScore = static_cast<int>(10 * e.maxHp * (1.0f + comboCount * 0.1f));
                            score += killScore;
                            spawnParticles(particles, e.shape.getPosition(), 20, e.baseColor);

                            bool isBig = e.maxHp >= 5;
                            hitstopTimer = isBig ? HITSTOP_BIG : HITSTOP_SMALL;

                            if (comboCount >= SLOWMO_COMBO_THRESHOLD || isBig)
                                slowMoTimer = SLOWMO_DURATION;

                            flashAlpha = FLASH_MAX_ALPHA;

                            Shockwave sw;
                            sw.position = e.shape.getPosition();
                            sw.radius = e.shape.getRadius();
                            sw.maxRadius = SHOCKWAVE_MAX_RADIUS + (isBig ? 40.0f : 0.0f);
                            sw.maxLifetime = SHOCKWAVE_LIFETIME;
                            sw.lifetime = sw.maxLifetime;
                            sw.color = e.baseColor;
                            shockwaves.push_back(sw);

                            if (fontLoaded) {
                                FloatingText ft;
                                ft.position = e.shape.getPosition() + sf::Vector2f(0.0f, -20.0f);
                                ft.velocity = {0.0f, -FLOATING_TEXT_SPEED};
                                ft.maxLifetime = FLOATING_TEXT_LIFETIME;
                                ft.lifetime = ft.maxLifetime;
                                ft.text = "+" + std::to_string(killScore);
                                ft.color = sf::Color(255, 255, 100);
                                ft.size = 18;
                                floatingTexts.push_back(ft);

                                if (comboCount >= 3) {
                                    FloatingText ct;
                                    ct.position = e.shape.getPosition() + sf::Vector2f(0.0f, -40.0f);
                                    ct.velocity = {0.0f, -FLOATING_TEXT_SPEED * 0.8f};
                                    ct.maxLifetime = FLOATING_TEXT_LIFETIME;
                                    ct.lifetime = ct.maxLifetime;
                                    ct.text = "x" + std::to_string(comboCount);
                                    ct.color = sf::Color(255, 200, 50);
                                    ct.size = 22;
                                    floatingTexts.push_back(ct);
                                }
                            }

                            bonusSystem.spawnOnKill(e.shape.getPosition(), dropValue);
                            level.onEnemyKilled();
                        }
                        break;
                    }
                }
            }
        }

        if (waveActive) {
            bonusSystem.update(dt, playerPos, PLAYER_RADIUS, timeLeft, speedBoostTimer, rapidFireTimer, pierceTimer);
            for (const auto& pickupPos : bonusSystem.getPickups()) {
                spawnParticles(particles, pickupPos, 12, sf::Color(255, 255, 200));
            }
            bonusSystem.clearPickups();
        }

        for (auto& sw : shockwaves) {
            sw.lifetime -= rawDt;
            if (sw.lifetime <= 0.0f) { sw.alive = false; continue; }
            float t = 1.0f - sw.lifetime / sw.maxLifetime;
            sw.radius = sw.maxRadius * t;
        }

        for (auto& ft : floatingTexts) {
            ft.lifetime -= rawDt;
            if (ft.lifetime <= 0.0f) { ft.alive = false; continue; }
            ft.position += ft.velocity * rawDt;
        }

        if (flashAlpha > 0.0f) {
            flashAlpha -= rawDt * (FLASH_MAX_ALPHA / FLASH_DURATION);
            if (flashAlpha < 0.0f) flashAlpha = 0.0f;
        }

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

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) { return !b.alive; }), bullets.end());
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](const Enemy& e) { return !e.alive; }), enemies.end());
        particles.erase(std::remove_if(particles.begin(), particles.end(), [](const Particle& p) { return !p.alive; }), particles.end());
        shockwaves.erase(std::remove_if(shockwaves.begin(), shockwaves.end(), [](const Shockwave& s) { return !s.alive; }), shockwaves.end());
        floatingTexts.erase(std::remove_if(floatingTexts.begin(), floatingTexts.end(), [](const FloatingText& f) { return !f.alive; }), floatingTexts.end());

        if (level.consumeWaveCompleted()) {
            bullets.clear();
            enemies.clear();
            particles.clear();
            shockwaves.clear();
            floatingTexts.clear();
            bonusSystem.clear();
            comboCount = 0;
            comboTimer = 0.0f;
            spawnTimer = 0.0f;
        }

        sf::Vector2f shakeOffset = {0.0f, 0.0f};
        if (shakeTimer > 0.0f) {
            shakeTimer -= rawDt;
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

        bonusSystem.draw(window, gameTime);

        for (const auto& sw : shockwaves) {
            float t = 1.0f - sw.lifetime / sw.maxLifetime;
            float alpha = (1.0f - t) * 200.0f;
            sf::CircleShape ring(sw.radius);
            ring.setOrigin({sw.radius, sw.radius});
            ring.setPosition(sw.position);
            ring.setFillColor(sf::Color::Transparent);
            sf::Color outlineColor = sw.color;
            outlineColor.a = static_cast<uint8_t>(std::max(0.0f, alpha));
            ring.setOutlineColor(outlineColor);
            float thickness = 3.0f * (1.0f - t);
            ring.setOutlineThickness(std::max(0.5f, thickness));
            window.draw(ring);
        }

        for (auto& p : particles)
            window.draw(p.shape);
        for (auto& e : enemies) {
            float spawnScale = std::min(1.0f, e.spawnAge / ENEMY_SPAWN_DURATION);
            e.shape.setScale({spawnScale, spawnScale});
            drawEnemy(window, e, playerPos);
        }
        for (auto& b : bullets)
            window.draw(b.shape);
        drawTomato(window, player);

        if (fontLoaded) {
            for (const auto& ft : floatingTexts) {
                float alpha = ft.lifetime / ft.maxLifetime;
                sf::Text text(font, ft.text, ft.size);
                sf::Color c = ft.color;
                c.a = static_cast<uint8_t>(alpha * 255);
                text.setFillColor(c);
                sf::FloatRect bounds = text.getLocalBounds();
                text.setOrigin(sf::Vector2f{bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
                text.setPosition(ft.position);
                float scale = 0.5f + 0.5f * alpha;
                text.setScale({scale, scale});
                window.draw(text);
            }
        }

        if (flashAlpha > 0.0f) {
            sf::RectangleShape flash({static_cast<float>(WIN_W), static_cast<float>(WIN_H)});
            flash.setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(flashAlpha)));
            window.setView(window.getDefaultView());
            window.draw(flash);
            view.setCenter(view.getCenter());
            window.setView(view);
        }

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

            auto formatBuffTime = [](float value) {
                if (value < 0.0f) value = 0.0f;
                int totalTenths = static_cast<int>(std::ceil(value * 10.0f));
                int whole = totalTenths / 10;
                int tenths = totalTenths % 10;
                return std::to_string(whole) + "." + std::to_string(tenths);
            };

            auto buffFill = [](BonusType type) {
                if (type == BonusType::Speed) return sf::Color(80, 200, 255);
                if (type == BonusType::RapidFire) return sf::Color(255, 200, 80);
                if (type == BonusType::Pierce) return sf::Color(80, 220, 120);
                return sf::Color(255, 80, 80);
            };

            auto buffOutline = [](BonusType type) {
                if (type == BonusType::Speed) return sf::Color(120, 255, 255);
                if (type == BonusType::RapidFire) return sf::Color(255, 255, 120);
                if (type == BonusType::Pierce) return sf::Color(140, 255, 170);
                return sf::Color(255, 200, 50);
            };

            auto buffName = [](BonusType type) {
                if (type == BonusType::Speed) return std::string("VITESSE");
                if (type == BonusType::RapidFire) return std::string("RAPIDE");
                if (type == BonusType::Pierce) return std::string("PERCANT");
                return std::string("TEMPS");
            };

            float iconSize = 28.0f;
            float iconGap = 8.0f;
            float iconX = barX;
            float iconY = barY + barHeight + 10.0f;

            auto drawBuff = [&](BonusType type, float remaining) {
                if (remaining <= 0.0f) return;
                std::string name = buffName(type);

                sf::Text label(font, name, 12);
                label.setFillColor(sf::Color(230, 230, 230));
                sf::FloatRect lb = label.getLocalBounds();

                float slotWidth = std::max(iconSize, lb.size.x) + 6.0f;
                float centerX = iconX + slotWidth / 2.0f;

                sf::RectangleShape icon({iconSize, iconSize});
                icon.setFillColor(buffFill(type));
                icon.setOutlineColor(buffOutline(type));
                icon.setOutlineThickness(2.0f);
                icon.setPosition({centerX - iconSize / 2.0f, iconY});
                window.draw(icon);

                sf::Text t(font, formatBuffTime(remaining), 14);
                t.setFillColor(sf::Color::Black);
                sf::FloatRect bounds = t.getLocalBounds();
                t.setOrigin(sf::Vector2f{
                    bounds.position.x + bounds.size.x / 2.0f,
                    bounds.position.y + bounds.size.y / 2.0f
                });
                t.setPosition({centerX, iconY + iconSize / 2.0f});
                window.draw(t);

                label.setOrigin(sf::Vector2f{
                    lb.position.x + lb.size.x / 2.0f,
                    lb.position.y + lb.size.y / 2.0f
                });
                label.setPosition({centerX, iconY + iconSize + 10.0f});
                window.draw(label);

                iconX += slotWidth + iconGap;
            };

            drawBuff(BonusType::Speed, speedBoostTimer);
            drawBuff(BonusType::RapidFire, rapidFireTimer);
            drawBuff(BonusType::Pierce, pierceTimer);

            auto formatWaveTime = [](float value) {
                if (value < 0.0f) value = 0.0f;
                int totalTenths = static_cast<int>(std::ceil(value * 10.0f));
                int whole = totalTenths / 10;
                int tenths = totalTenths % 10;
                return std::to_string(whole) + "." + std::to_string(tenths);
            };

            float objectiveY = iconY + iconSize + 26.0f;
            if (level.isIntermission()) {
                std::string inter = "VAGUE TERMINEE";
                sf::Text interText(font, inter, 18);
                interText.setFillColor(sf::Color(230, 230, 230));
                sf::FloatRect ib = interText.getLocalBounds();
                interText.setOrigin(sf::Vector2f{
                    ib.position.x + ib.size.x / 2.0f,
                    ib.position.y + ib.size.y / 2.0f
                });
                interText.setPosition({WIN_W / 2.0f, objectiveY});
                window.draw(interText);

                std::string next = "PROCHAINE VAGUE DANS " + formatWaveTime(level.intermissionLeft()) + "s";
                sf::Text nextText(font, next, 16);
                nextText.setFillColor(sf::Color(200, 200, 200));
                sf::FloatRect nb = nextText.getLocalBounds();
                nextText.setOrigin(sf::Vector2f{
                    nb.position.x + nb.size.x / 2.0f,
                    nb.position.y + nb.size.y / 2.0f
                });
                nextText.setPosition({WIN_W / 2.0f, objectiveY + 22.0f});
                window.draw(nextText);
            } else {
                std::string header = "VAGUE " + std::to_string(level.waveIndex() + 1) + "  OBJECTIF";
                sf::Text headerText(font, header, 16);
                headerText.setFillColor(sf::Color(220, 220, 220));
                sf::FloatRect hb = headerText.getLocalBounds();
                headerText.setOrigin(sf::Vector2f{
                    hb.position.x + hb.size.x / 2.0f,
                    hb.position.y + hb.size.y / 2.0f
                });
                headerText.setPosition({WIN_W / 2.0f, objectiveY});
                window.draw(headerText);

                std::string obj;
                if (level.objectiveType() == LevelManager::ObjectiveType::Kill) {
                    obj = "ELIMINER " + std::to_string(level.killsInWave()) + " / " + std::to_string(level.killTarget());
                } else {
                    obj = "SURVIVRE " + formatWaveTime(level.waveTime()) + " / " + formatWaveTime(level.surviveTarget()) + "s";
                }
                sf::Text objText(font, obj, 18);
                objText.setFillColor(sf::Color(255, 255, 255));
                sf::FloatRect ob = objText.getLocalBounds();
                objText.setOrigin(sf::Vector2f{
                    ob.position.x + ob.size.x / 2.0f,
                    ob.position.y + ob.size.y / 2.0f
                });
                objText.setPosition({WIN_W / 2.0f, objectiveY + 22.0f});
                window.draw(objText);
            }

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
