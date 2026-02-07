#pragma once

#include <SFML/Graphics.hpp>
#include <optional>

void drawMenuScreen(
    sf::RenderWindow& window,
    const std::optional<sf::Sprite>& menuSprite,
    const sf::Font* font,
    int menuIndex
);

void drawOptionsScreen(
    sf::RenderWindow& window,
    const std::optional<sf::Sprite>& menuSprite,
    const sf::Font* font,
    int optionsIndex,
    float musicVolume
);
