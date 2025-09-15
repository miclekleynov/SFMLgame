//
// Created by mishka on 15.09.2025.
//

#include "Button.h"

#include "Button.h"


Button::Button() {
    box_.setFillColor(sf::Color::Transparent);
    box_.setOutlineColor(sf::Color::White);
    box_.setOutlineThickness(2.f);


    label_.setFillColor(sf::Color::White);
}


void Button::setSize(const sf::Vector2f& size) {
    box_.setSize(size);
    updateLabelOrigin();
    cached_size_ = size;
}


void Button::setPosition(const sf::Vector2f& pos) {
    box_.setPosition(pos);
    updateLabelOrigin();
}


void Button::setText(const sf::String& str, const sf::Font& font, unsigned int charSize) {
    label_.setFont(font);
    label_.setString(str);
    label_.setCharacterSize(charSize);
    updateLabelOrigin();
}


void Button::setOnClick(std::function<void()> cb) { onClick_ = std::move(cb); }


sf::FloatRect Button::bounds() const { return box_.getGlobalBounds(); }

sf::Vector2f Button::getSize() const { return cached_size_; }


void Button::handleEvent(const sf::Event& e, const sf::RenderWindow& window) {
    if (e.type == sf::Event::MouseMoved) {
        auto mouse = sf::Vector2f(sf::Mouse::getPosition(window));
        hovered_ = bounds().contains(mouse);
        box_.setOutlineThickness(hovered_ ? 3.f : 2.f);
    }
    if (e.type == sf::Event::MouseButtonPressed && e.mouseButton.button == sf::Mouse::Left) {
        auto mouse = sf::Vector2f(sf::Mouse::getPosition(window));
        if (bounds().contains(mouse) && onClick_) onClick_();
    }
}


void Button::update(float) {
    // no-op for now (could add animations)
}


void Button::updateLabelOrigin() {
    // Center the label inside the rectangle
    auto rect = box_.getGlobalBounds();
    auto lb = label_.getLocalBounds();
    label_.setOrigin(lb.left + lb.width / 2.f, lb.top + lb.height / 2.f);
    label_.setPosition(rect.left + rect.width / 2.f, rect.top + rect.height / 2.f);
}


void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(box_, states);
    target.draw(label_, states);
}