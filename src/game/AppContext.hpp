#pragma once

#include "actors/Actor.hpp"
#include "audio/MusicManager.hpp"
#include "game/BonusSystem.hpp"
#include "game/GameConfig.hpp"
#include "game/LevelManager.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <optional>
#include <vector>

enum class AppState { Splash, Menu, Options, Playing, GameOver };

struct App {
    sf::RenderWindow window{sf::VideoMode({WIN_W, WIN_H}), "Tomato Blaster"};
    sf::CircleShape player{PLAYER_RADIUS};
    std::vector<Bullet> bullets; std::vector<Enemy> enemies;
    std::vector<EnemyProjectile> enemyProjectiles;
    std::vector<Particle> particles; std::vector<Shockwave> shockwaves; std::vector<FloatingText> floatingTexts;
    BonusSystem bonusSystem; LevelManager level; MusicManager music; sf::Clock clock;
    float timeLeft=TOTAL_TIME, gameTime=0.0f, spawnTimer=0.0f, spawnInterval=SPAWN_INTERVAL_START;
    int score=0, comboCount=0; float comboTimer=0.0f, shakeTimer=0.0f, shootCooldown=0.0f;
    float speedBoostTimer=0.0f, rapidFireTimer=0.0f, pierceTimer=0.0f, hitstopTimer=0.0f, slowMoTimer=0.0f, flashAlpha=0.0f, shootSquash=0.0f;
    float bossContactCooldown=0.0f;
    float laserCharge=0.0f, laserTimer=0.0f;
    sf::Vector2f laserDirection{1.0f, 0.0f};
    bool laserKeyHeld=false;
    AppState state=AppState::Menu; int menuIndex=0, optionsIndex=0; float musicVolume=70.0f;
    sf::Font font; bool fontLoaded=false;
    sf::Texture menuTexture; std::optional<sf::Sprite> menuSprite; bool menuBgLoaded=false;
    sf::SoundBuffer menuBuffer; std::optional<sf::Sound> menuSound; bool menuSoundLoaded=false;
    std::optional<sf::Music> menuMusic; bool menuMusicLoaded=false;
    sf::Texture productionLogoTexture; std::optional<sf::Sprite> productionLogoSprite; bool productionLogoLoaded=false;
    float splashTimer=0.0f;
    sf::Texture grassTexture; std::optional<sf::Sprite> grassSprite; bool grassLoaded=false;
};
