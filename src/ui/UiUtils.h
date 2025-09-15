//
// Created by mishka on 15.09.2025.
//

#ifndef UIUTILS_H
#define UIUTILS_H

#pragma once
#include <SFML/Graphics.hpp>

inline void center_text_horiz(sf::Text& t, float center_x) {
    const auto b = t.getLocalBounds();
    t.setOrigin(b.left + b.width * 0.5f, 0.f);
    t.setPosition(center_x, t.getPosition().y);
}

#endif //UIUTILS_H
