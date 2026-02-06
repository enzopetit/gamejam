#include "utils/FontLoader.hpp"

bool loadFont(sf::Font& font) {
    const char* candidates[] = {
        "assets/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf"
    };
    for (const char* path : candidates) {
        if (font.openFromFile(path))
            return true;
    }
    return false;
}
