#include "game/AppFlow.hpp"
#include "game/MenuRenderer.hpp"
#include "game/GameOverRenderer.hpp"

#include <algorithm>

namespace {
constexpr float SPLASH_HOLD_TIME = 2.0f;
constexpr float SPLASH_FADE_TIME = 1.2f;
}

void appRenderStateScreens(App& a) {
    if (a.state == AppState::Splash) {
        if (a.menuMusicLoaded && a.menuMusic->getStatus() != sf::SoundSource::Status::Playing) a.menuMusic->play();
        if (!a.productionLogoLoaded || !a.productionLogoSprite) {
            drawMenuScreen(a.window, a.menuSprite, a.fontLoaded ? &a.font : nullptr, a.menuIndex);
            a.window.display();
            return;
        }

        if (a.splashTimer < SPLASH_HOLD_TIME) {
            a.window.clear(sf::Color::Black);
            a.window.draw(*a.productionLogoSprite);
            a.window.display();
            return;
        }

        drawMenuScreen(a.window, a.menuSprite, a.fontLoaded ? &a.font : nullptr, a.menuIndex);
        float fadeT = std::clamp((a.splashTimer - SPLASH_HOLD_TIME) / SPLASH_FADE_TIME, 0.0f, 1.0f);
        uint8_t alpha = static_cast<uint8_t>(255.0f * (1.0f - fadeT));
        sf::Sprite logo = *a.productionLogoSprite;
        logo.setColor(sf::Color(255, 255, 255, alpha));
        a.window.draw(logo);
        a.window.display();
    } else if (a.state == AppState::Menu) {
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
