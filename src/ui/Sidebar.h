//
// Created by mishka on 15.09.2025.
//

#ifndef SIDEBAR_H
#define SIDEBAR_H

#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>
#include "resources/Fonts.h"

struct BaseStats { int money{}; int satiety{}; int pickaxe{}; int shovel{}; int brush{}; };
using stats_provider_t = std::function<BaseStats()>;

class Sidebar {
public:
    Sidebar();

    void set_position(sf::Vector2f pos);
    void set_width(float w);
    void set_provider(stats_provider_t provider);

    float width() const { return width_; }
    void draw(sf::RenderTarget& target) const;

private:
    sf::Vector2f position_ {0.f, 0.f};
    float width_ = 320.f;
    stats_provider_t stats_provider_;

    mutable sf::Text title_;
    mutable sf::Text money_text_;
    mutable sf::Text satiety_text_;
    mutable sf::Text tools_text_;
};


#endif //SIDEBAR_H
