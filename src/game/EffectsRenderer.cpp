#include "game/EffectsRenderer.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include "game/GameConfig.hpp"

void drawShockwaves(sf::RenderWindow& window, const std::vector<Shockwave>& shockwaves) {
    for (const auto& sw : shockwaves) {
        float t = 1.0f - sw.lifetime / sw.maxLifetime;
        float alpha = (1.0f - t) * 200.0f;
        sf::CircleShape ring(sw.radius);
        ring.setOrigin({sw.radius, sw.radius});
        ring.setPosition(sw.position);
        ring.setFillColor(sf::Color::Transparent);
        sf::Color outlineColor = sw.color;
        outlineColor.a = static_cast<uint8_t>(std::max(0.0f, alpha));
        ring.setOutlineColor(outlineColor);
        float thickness = 3.0f * (1.0f - t);
        ring.setOutlineThickness(std::max(0.5f, thickness));
        window.draw(ring);
    }
}

void drawFlashOverlay(sf::RenderWindow& window, float alpha) {
    if (alpha <= 0.0f) return;
    sf::RectangleShape flash({static_cast<float>(WIN_W), static_cast<float>(WIN_H)});
    flash.setFillColor(sf::Color(255, 255, 255, static_cast<uint8_t>(alpha)));
    window.draw(flash);
}

void drawFloatingTexts(sf::RenderWindow& window, const sf::Font* font, const std::vector<FloatingText>& texts) {
    if (!font) return;
    for (const auto& ft : texts) {
        float alpha = ft.lifetime / ft.maxLifetime;
        sf::Text text(*font, ft.text, ft.size);
        sf::Color c = ft.color;
        c.a = static_cast<uint8_t>(alpha * 255);
        text.setFillColor(c);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin(sf::Vector2f{bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
        text.setPosition(ft.position);
        float scale = 0.5f + 0.5f * alpha;
        text.setScale({scale, scale});
        window.draw(text);
    }
}
