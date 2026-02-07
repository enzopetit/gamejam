#include "game/AppFlow.hpp"
#include "utils/FontLoader.hpp"

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
}

void appResetGame(App& a) {
    a.timeLeft = TOTAL_TIME; a.gameTime=0.0f; a.spawnTimer=0.0f; a.spawnInterval=SPAWN_INTERVAL_START; a.score=0;
    a.comboCount=0; a.comboTimer=0.0f; a.shakeTimer=0.0f; a.shootCooldown=0.0f; a.speedBoostTimer=0.0f; a.rapidFireTimer=0.0f; a.pierceTimer=0.0f;
    a.hitstopTimer=0.0f; a.slowMoTimer=0.0f; a.flashAlpha=0.0f; a.shootSquash=0.0f;
    a.laserCharge=0.0f; a.laserTimer=0.0f; a.laserDirection={1.0f, 0.0f}; a.laserKeyHeld=false;
    a.bullets.clear(); a.enemies.clear(); a.particles.clear(); a.shockwaves.clear(); a.floatingTexts.clear(); a.bonusSystem.clear();
    a.level.reset(); a.player.setPosition({WIN_W/2.0f, WIN_H/2.0f}); a.player.setScale({1.0f,1.0f}); a.music.reset(a.timeLeft); a.music.setVolume(a.musicVolume);
}
