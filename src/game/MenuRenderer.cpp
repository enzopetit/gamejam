#include "game/MenuRenderer.hpp"

#include "game/GameConfig.hpp"

void drawMenuScreen(
    sf::RenderWindow& window,
    const std::optional<sf::Sprite>& menuSprite,
    const sf::Font* font,
    int menuIndex
) {
    window.clear(sf::Color(20, 15, 25));
    if (menuSprite)
        window.draw(*menuSprite);
    if (!font) return;

    const char* items[] = {"START", "OPTIONS", "QUIT"};
    for (int i = 0; i < 3; ++i) {
        sf::Text t(*font, items[i], 28);
        t.setFillColor(i == menuIndex ? sf::Color(255, 220, 120) : sf::Color(220, 220, 220));
        sf::FloatRect b = t.getLocalBounds();
        t.setOrigin(sf::Vector2f{b.position.x + b.size.x / 2.0f, b.position.y + b.size.y / 2.0f});
        t.setPosition({WIN_W / 2.0f, 260.0f + i * 46.0f});
        window.draw(t);
    }
}

void drawOptionsScreen(
    sf::RenderWindow& window,
    const std::optional<sf::Sprite>& menuSprite,
    const sf::Font* font,
    int optionsIndex,
    float musicVolume
) {
    window.clear(sf::Color(18, 14, 22));
    if (menuSprite) {
        sf::Sprite sprite = *menuSprite;
        sprite.setColor(sf::Color(200, 200, 200));
        window.draw(sprite);
    }
    if (!font) return;

    sf::Text title(*font, "OPTIONS", 42);
    title.setFillColor(sf::Color(230, 230, 230));
    sf::FloatRect tb = title.getLocalBounds();
    title.setOrigin(sf::Vector2f{tb.position.x + tb.size.x / 2.0f, tb.position.y + tb.size.y / 2.0f});
    title.setPosition({WIN_W / 2.0f, 140.0f});
    window.draw(title);

    std::string vol = "MUSIC VOLUME: " + std::to_string(static_cast<int>(musicVolume));
    sf::Text volText(*font, vol, 24);
    volText.setFillColor(optionsIndex == 0 ? sf::Color(255, 220, 120) : sf::Color(220, 220, 220));
    sf::FloatRect vb = volText.getLocalBounds();
    volText.setOrigin(sf::Vector2f{vb.position.x + vb.size.x / 2.0f, vb.position.y + vb.size.y / 2.0f});
    volText.setPosition({WIN_W / 2.0f, 260.0f});
    window.draw(volText);

    sf::Text backText(*font, "BACK", 24);
    backText.setFillColor(optionsIndex == 1 ? sf::Color(255, 220, 120) : sf::Color(220, 220, 220));
    sf::FloatRect bb = backText.getLocalBounds();
    backText.setOrigin(sf::Vector2f{bb.position.x + bb.size.x / 2.0f, bb.position.y + bb.size.y / 2.0f});
    backText.setPosition({WIN_W / 2.0f, 306.0f});
    window.draw(backText);
}

