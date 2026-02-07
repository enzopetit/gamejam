#pragma once

#include "game/AppContext.hpp"

void appInit(App& a);
void appResetGame(App& a);
void appHandleEvents(App& a, float rawDt, float dt);
void appRenderStateScreens(App& a);
void appUpdatePlaying(App& a, float rawDt, float dt);
void appRenderPlaying(App& a);

void updateMotionAndShooting(App& a, float rawDt, float dt);
void updateSpawns(App& a, float dt);
void updateEnemiesAndCollisions(App& a, float rawDt, float dt);
void updatePost(App& a, float rawDt);
