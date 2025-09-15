//
// Created by mishka on 15.09.2025.
//

#include "ui/Sidebar.h"

Sidebar::Sidebar() {
    title_.setFont(Fonts::Main()); money_text_.setFont(Fonts::Main());
    satiety_text_.setFont(Fonts::Main()); tools_text_.setFont(Fonts::Main());
    title_.setCharacterSize(24); money_text_.setCharacterSize(18);
    satiety_text_.setCharacterSize(18); tools_text_.setCharacterSize(18);
    title_.setFillColor(sf::Color::White);
    money_text_.setFillColor(sf::Color::White);
    satiety_text_.setFillColor(sf::Color::White);
    tools_text_.setFillColor(sf::Color::White);
}

void Sidebar::set_position(sf::Vector2f pos) { position_ = pos; }
void Sidebar::set_width(float w) { width_ = w; }
void Sidebar::set_provider(stats_provider_t provider) { stats_provider_ = std::move(provider); }

void Sidebar::draw(sf::RenderTarget& target) const {
    if (!stats_provider_) return;
    const BaseStats s = stats_provider_();

    // Border box
    //sf::RectangleShape border({width_, target.getView().getSize().y});
    //border.setPosition(position_);
    //border.setFillColor(sf::Color(0,0,0,0));
    //border.setOutlineThickness(1.f);
    //border.setOutlineColor(sf::Color::White);
    //target.draw(border);

    // Texts
    title_.setString("Stats");
    title_.setPosition(position_.x + 16.f, position_.y + 16.f);

    money_text_.setString("Balance: " + std::to_string(s.money) + "$");
    satiety_text_.setString("Satiety: " + std::to_string(s.satiety) + "%");
    tools_text_.setString("Pickaxe - " + std::to_string(s.pickaxe) + "%  |  "
                          "Shovel - " + std::to_string(s.shovel)  + "%  |  "
                          "Brush - "  + std::to_string(s.brush)   + "%");

    money_text_.setPosition(position_.x + 16.f, position_.y + 56.f);
    satiety_text_.setPosition(position_.x + 16.f, position_.y + 80.f);
    tools_text_.setPosition(position_.x + 16.f, position_.y + 112.f);

    target.draw(title_);
    target.draw(money_text_);
    target.draw(satiety_text_);
    target.draw(tools_text_);
}