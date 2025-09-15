//
// Created by mishka on 15.09.2025.
//

#ifndef SCREEN_H
#define SCREEN_H

#pragma once
#include <SFML/Graphics.hpp>

class ScreenManager; // forward

struct AppContext {
    sf::RenderWindow* window{}; // non-owning
    ScreenManager* manager{};   // non-owning, set by ScreenManager
};

class Screen {
public:
    explicit Screen(AppContext ctx) : ctx_(ctx) {}
    virtual ~Screen() = default;

    virtual void handleEvent(const sf::Event& e) = 0;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderTarget& rt) = 0;

protected:
    AppContext ctx_{};
};

#endif //SCREEN_H
