#include "game/AppFlow.hpp"
#include "game/MenuRenderer.hpp"
#include "game/GameOverRenderer.hpp"

void appRenderStateScreens(App& a) {
    if (a.state == AppState::Menu) {
        if (a.menuMusicLoaded && a.menuMusic->getStatus() != sf::SoundSource::Status::Playing) a.menuMusic->play();
        drawMenuScreen(a.window, a.menuSprite, a.fontLoaded ? &a.font : nullptr, a.menuIndex);
        a.window.display();
    } else if (a.state == AppState::Options) {
        if (a.menuMusicLoaded && a.menuMusic->getStatus() != sf::SoundSource::Status::Playing) a.menuMusic->play();
        drawOptionsScreen(a.window, a.menuSprite, a.fontLoaded ? &a.font : nullptr, a.optionsIndex, a.musicVolume);
        a.window.display();
    } else if (a.state == AppState::GameOver) {
        drawGameOverScreen(a.window, a.fontLoaded ? &a.font : nullptr, a.score);
        a.window.display();
    }
}

