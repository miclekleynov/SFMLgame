//
// Created by mishka on 15.09.2025.
//

#include "Fonts.h"
#include <SFML/Graphics.hpp>
#include <filesystem>

bool Fonts::loaded_ = false;
sf::Font Fonts::main_{};

std::string Fonts::GuessPath() {
    namespace fs = std::filesystem;
    // Try a few common spots relative to the executable
    const char* candidates[] = {
        "content/arialmt.ttf",
        };
    for (auto* p : candidates) {
        if (fs::exists(p)) return p;
    }
    return "assets/fonts/DejaVuSans.ttf"; // fallback; may fail, but error will be visible
}


bool Fonts::EnsureLoaded() {
    if (loaded_) return true;
    auto path = GuessPath();
    loaded_ = main_.loadFromFile(path);
    return loaded_;
}


sf::Font& Fonts::Main() {
    if (!EnsureLoaded()) {
        // As a last resort, create a minimal empty font to avoid crashes; text won't render.
        static sf::Font empty;
        return empty;
    }
    return main_;
}