#pragma once

inline constexpr float BASE_SHOOT_COOLDOWN = 0.15f;

inline constexpr float DASH_SPEED = 800.0f;
inline constexpr float DASH_DURATION = 0.15f;
inline constexpr float DASH_COOLDOWN = 1.0f;
inline constexpr float DASH_IFRAME_DURATION = 0.2f;

inline constexpr float SHOTGUN_SPREAD = 15.0f;
inline constexpr int SHOTGUN_PELLETS = 3;
inline constexpr float SHOTGUN_TIME_COST = 0.15f;
inline constexpr float SHOTGUN_COOLDOWN = 0.35f;

inline constexpr float CHARGE_MAX_TIME = 1.5f;
inline constexpr float CHARGE_MIN_TIME = 0.3f;
inline constexpr float CHARGE_MAX_RADIUS = 20.0f;
inline constexpr float CHARGE_EXPLOSION_RADIUS = 60.0f;
inline constexpr float CHARGE_TIME_COST = 0.6f;

inline constexpr float MINE_RADIUS = 8.0f;
inline constexpr float MINE_EXPLOSION_RADIUS = 50.0f;
inline constexpr float MINE_TIME_COST = 0.4f;
inline constexpr float MINE_LIFETIME = 15.0f;
inline constexpr int MINE_MAX_COUNT = 5;

inline constexpr float COMBO_PITCH_BASE = 1.0f;
inline constexpr float COMBO_PITCH_STEP = 0.05f;

inline constexpr float ENEMY_HEALTH_BAR_WIDTH = 30.0f;
inline constexpr float ENEMY_HEALTH_BAR_HEIGHT = 4.0f;
inline constexpr float ENEMY_HEALTH_BAR_OFFSET = 8.0f;

inline constexpr float PLAYER_TRAIL_INTERVAL = 0.02f;
inline constexpr float PLAYER_TRAIL_LIFETIME = 0.2f;
inline constexpr float WALK_OSCILLATION_SPEED = 12.0f;
inline constexpr float WALK_OSCILLATION_AMP = 0.05f;

inline constexpr float FREEZE_FRAME_DURATION = 0.06f;
inline constexpr float FREEZE_ZOOM_SCALE = 1.03f;

inline constexpr float TILE_SIZE = 64.0f;

inline constexpr int HIGHSCORE_MAX_ENTRIES = 10;
inline constexpr const char* HIGHSCORE_FILE = "highscores.dat";

