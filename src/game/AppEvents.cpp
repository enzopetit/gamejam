#include "game/AppFlow.hpp"

static void playMenuSound(App& a){ if (a.menuSoundLoaded) a.menuSound->play(); }

void appHandleEvents(App& a, float, float) {
    while (const auto event = a.window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) a.window.close();
        if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
            if (a.state == AppState::Menu) {
                if (key->code == sf::Keyboard::Key::Escape) a.window.close();
                else if (key->code == sf::Keyboard::Key::Up || key->code == sf::Keyboard::Key::Z || key->code == sf::Keyboard::Key::W) { a.menuIndex = (a.menuIndex + 2) % 3; playMenuSound(a);} 
                else if (key->code == sf::Keyboard::Key::Down || key->code == sf::Keyboard::Key::S) { a.menuIndex = (a.menuIndex + 1) % 3; playMenuSound(a);} 
                else if (key->code == sf::Keyboard::Key::Enter || key->code == sf::Keyboard::Key::Space) {
                    playMenuSound(a);
                    if (a.menuIndex == 0) { if (a.menuMusicLoaded) a.menuMusic->stop(); appResetGame(a); a.state = AppState::Playing; }
                    else if (a.menuIndex == 1) a.state = AppState::Options;
                    else if (a.menuIndex == 2) a.window.close();
                }
            } else if (a.state == AppState::Options) {
                if (key->code == sf::Keyboard::Key::Escape) a.state = AppState::Menu;
                else if (key->code == sf::Keyboard::Key::Up || key->code == sf::Keyboard::Key::Z || key->code == sf::Keyboard::Key::W) { a.optionsIndex = (a.optionsIndex + 1) % 2; playMenuSound(a);} 
                else if (key->code == sf::Keyboard::Key::Down || key->code == sf::Keyboard::Key::S) { a.optionsIndex = (a.optionsIndex + 1) % 2; playMenuSound(a);} 
                else if (a.optionsIndex == 0 && (key->code == sf::Keyboard::Key::Left || key->code == sf::Keyboard::Key::Q || key->code == sf::Keyboard::Key::A)) { a.musicVolume = std::max(0.0f, a.musicVolume - 5.0f); a.music.setVolume(a.musicVolume); if (a.menuSoundLoaded) a.menuSound->setVolume(a.musicVolume); if (a.menuMusicLoaded) a.menuMusic->setVolume(a.musicVolume); playMenuSound(a);} 
                else if (a.optionsIndex == 0 && (key->code == sf::Keyboard::Key::Right || key->code == sf::Keyboard::Key::D)) { a.musicVolume = std::min(100.0f, a.musicVolume + 5.0f); a.music.setVolume(a.musicVolume); if (a.menuSoundLoaded) a.menuSound->setVolume(a.musicVolume); if (a.menuMusicLoaded) a.menuMusic->setVolume(a.musicVolume); playMenuSound(a);} 
                else if (key->code == sf::Keyboard::Key::Enter || key->code == sf::Keyboard::Key::Space) { playMenuSound(a); if (a.optionsIndex == 1) a.state = AppState::Menu; }
            } else if (a.state == AppState::Playing) {
                if (key->code == sf::Keyboard::Key::Escape) { a.state = AppState::Menu; a.music.stop(); }
            } else if (a.state == AppState::GameOver) {
                if (key->code == sf::Keyboard::Key::R || key->code == sf::Keyboard::Key::Enter) { if (a.menuMusicLoaded) a.menuMusic->stop(); appResetGame(a); a.state = AppState::Playing; }
                else if (key->code == sf::Keyboard::Key::Escape) { a.state = AppState::Menu; a.music.stop(); }
            }
        }
    }
}

