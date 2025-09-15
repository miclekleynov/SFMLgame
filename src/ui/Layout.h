//
// Created by mishka on 15.09.2025.
//

#ifndef LAYOUT_H
#define LAYOUT_H

#pragma once
#include <SFML/Graphics.hpp>
#include "Button.h"

namespace Layout {
    inline float windowCenterX(const sf::RenderWindow& w) {
        return static_cast<float>(w.getSize().x) * 0.5f;
    }

    inline float centerXForWidth(const sf::RenderWindow& w, float width) {
        return windowCenterX(w) - width * 0.5f;
    }

    // Центруем sf::Text по X: выставляем origin и позицию (Y передаётся вызывающим кодом)
    inline void centerTextX(sf::Text& t, const sf::RenderWindow& w, float y) {
        const auto r = t.getLocalBounds();
        t.setOrigin(r.left + r.width * 0.5f, 0.f);
        t.setPosition(windowCenterX(w), y);
    }

    // Возвращаем Y, находящийся чуть ниже текста (gap пикселей)
    inline float belowTextY(const sf::Text& t, float gap) {
        const auto r = t.getGlobalBounds();
        return r.top + r.height + gap;
    }

    // Центруем кнопку по X (нужна ширина кнопки)
    inline void centerButtonX(Button& b, const sf::RenderWindow& w, float y) {
        const sf::Vector2f sz = b.getSize();
        b.setPosition(sf::Vector2f(centerXForWidth(w, sz.x), y));
    }
}

#endif //LAYOUT_H
