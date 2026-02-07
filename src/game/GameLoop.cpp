#include "game/GameLoop.hpp"
#include "game/AppFlow.hpp"
#include "game/AppContext.hpp"
#include <ctime>

int runGame() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    App a; appInit(a);
    while (a.window.isOpen()) {
        float rawDt = a.clock.restart().asSeconds(); if (rawDt > 0.05f) rawDt = 0.05f; float dt = rawDt;
        appHandleEvents(a, rawDt, dt);
        if (a.state != AppState::Playing) { appRenderStateScreens(a); continue; }
        appUpdatePlaying(a, rawDt, dt);
        if (a.state != AppState::Playing) { appRenderStateScreens(a); continue; }
        appRenderPlaying(a);
    }
    return 0;
}

