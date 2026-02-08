#include "game/AppFlow.hpp"
#include "utils/FontLoader.hpp"

#include <cstdlib>
#include <ctime>

void appInit(App& a) {
    a.window.setFramerateLimit(60);
    a.player.setFillColor(sf::Color(220, 50, 50));
    a.player.setOrigin({PLAYER_RADIUS, PLAYER_RADIUS});
    a.player.setPosition({WIN_W / 2.0f, WIN_H / 2.0f});
    a.fontLoaded = loadFont(a.font);
    a.menuBgLoaded = a.menuTexture.loadFromFile("assets/Menu.png");
    if (a.menuBgLoaded) {
        a.menuSprite.emplace(a.menuTexture);
        sf::Vector2u s = a.menuTexture.getSize();
        if (s.x > 0 && s.y > 0) {
            a.menuSprite->setScale({static_cast<float>(WIN_W)/s.x, static_cast<float>(WIN_H)/s.y});
        }
    }
    a.menuSoundLoaded = a.menuBuffer.loadFromFile("music/menu.ogg");
    if (!a.menuSoundLoaded) a.menuSoundLoaded = a.menuBuffer.loadFromFile("music/Menu.ogg");
    if (a.menuSoundLoaded) { a.menuSound.emplace(a.menuBuffer); a.menuSound->setVolume(a.musicVolume);}    
    a.menuMusic.emplace();
    if (!a.menuMusic->openFromFile("music/menu.ogg") && !a.menuMusic->openFromFile("music/Menu.ogg"))
        a.menuMusic.reset();
    else { a.menuMusicLoaded = true; a.menuMusic->setLooping(true); a.menuMusic->setVolume(a.musicVolume);}    
    if (a.menuMusicLoaded) a.menuSoundLoaded = false;
    a.music.loadFromJson("music/music.json");
    a.music.setVolume(a.musicVolume);
    a.music.stop();

    a.productionLogoLoaded = a.productionLogoTexture.loadFromFile("assets/ProductionLogo.png");
    if (!a.productionLogoLoaded) a.productionLogoLoaded = a.productionLogoTexture.loadFromFile("assets/Anzoproduction.png");
    if (a.productionLogoLoaded) {
        a.productionLogoSprite.emplace(a.productionLogoTexture);
        sf::Vector2u s = a.productionLogoTexture.getSize();
        if (s.x > 0 && s.y > 0) {
            float sx = static_cast<float>(WIN_W) / static_cast<float>(s.x);
            float sy = static_cast<float>(WIN_H) / static_cast<float>(s.y);
            float scale = std::min(sx, sy);
            a.productionLogoSprite->setScale({scale, scale});
            float drawW = static_cast<float>(s.x) * scale;
            float drawH = static_cast<float>(s.y) * scale;
            a.productionLogoSprite->setPosition({(WIN_W - drawW) * 0.5f, (WIN_H - drawH) * 0.5f});
        }
    }

    std::srand(static_cast<unsigned>(std::time(nullptr)));
    {
        sf::RenderTexture rt;
        if (rt.resize({WIN_W, WIN_H})) {
            rt.clear(sf::Color(34, 120, 34));
            for (int i = 0; i < 800; ++i) {
                float x = static_cast<float>(std::rand() % WIN_W);
                float y = static_cast<float>(std::rand() % WIN_H);
                float r = 8.0f + static_cast<float>(std::rand() % 25);
                int shade = std::rand() % 4;
                sf::Color c;
                if (shade == 0) c = sf::Color(28, 100, 28);
                else if (shade == 1) c = sf::Color(40, 140, 35);
                else if (shade == 2) c = sf::Color(50, 155, 40);
                else c = sf::Color(30, 110, 30);
                sf::CircleShape patch(r);
                patch.setOrigin({r, r});
                patch.setPosition({x, y});
                patch.setFillColor(c);
                rt.draw(patch);
            }
            for (int i = 0; i < 300; ++i) {
                float x = static_cast<float>(std::rand() % WIN_W);
                float y = static_cast<float>(std::rand() % WIN_H);
                float w = 2.0f + static_cast<float>(std::rand() % 3);
                float h = 6.0f + static_cast<float>(std::rand() % 10);
                uint8_t g = static_cast<uint8_t>(90 + std::rand() % 80);
                sf::RectangleShape blade({w, h});
                blade.setOrigin({w * 0.5f, h});
                blade.setPosition({x, y});
                blade.setRotation(sf::degrees(static_cast<float>(-20 + std::rand() % 40)));
                blade.setFillColor(sf::Color(20, g, 15));
                rt.draw(blade);
            }
            for (int i = 0; i < 40; ++i) {
                float x = static_cast<float>(std::rand() % WIN_W);
                float y = static_cast<float>(std::rand() % WIN_H);
                float r = 2.0f + static_cast<float>(std::rand() % 3);
                sf::CircleShape dirt(r);
                dirt.setOrigin({r, r});
                dirt.setPosition({x, y});
                dirt.setFillColor(sf::Color(80, 60, 30, 120));
                rt.draw(dirt);
            }
            rt.display();
            a.grassTexture = rt.getTexture();
            a.grassSprite.emplace(a.grassTexture);
            a.grassLoaded = true;
        }
    }

    a.splashTimer = 0.0f;
    a.state = a.productionLogoLoaded ? AppState::Splash : AppState::Menu;
    if (a.menuMusicLoaded) a.menuMusic->play();
}

void appResetGame(App& a) {
    a.timeLeft = TOTAL_TIME; a.gameTime=0.0f; a.spawnTimer=0.0f; a.spawnInterval=SPAWN_INTERVAL_START; a.score=0;
    a.comboCount=0; a.comboTimer=0.0f; a.shakeTimer=0.0f; a.shootCooldown=0.0f; a.speedBoostTimer=0.0f; a.rapidFireTimer=0.0f; a.pierceTimer=0.0f;
    a.hitstopTimer=0.0f; a.slowMoTimer=0.0f; a.flashAlpha=0.0f; a.shootSquash=0.0f; a.bossContactCooldown=0.0f;
    a.laserCharge=0.0f; a.laserTimer=0.0f; a.laserDirection={1.0f, 0.0f}; a.laserKeyHeld=false;
    a.bullets.clear(); a.enemies.clear(); a.enemyProjectiles.clear(); a.particles.clear(); a.shockwaves.clear(); a.floatingTexts.clear(); a.bonusSystem.clear();
    a.level.reset(); a.player.setPosition({WIN_W/2.0f, WIN_H/2.0f}); a.player.setScale({1.0f,1.0f}); a.music.reset(a.timeLeft); a.music.setVolume(a.musicVolume);
}
