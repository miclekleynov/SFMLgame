//
// Created by mishka on 15.09.2025.
//

#ifndef BUTTON_H
#define BUTTON_H

#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

class Button : public sf::Drawable {
public:
    Button();
    void setSize(const sf::Vector2f& size);
    void setPosition(const sf::Vector2f& pos);
    void setText(const sf::String& str, const sf::Font& font, unsigned int charSize = 32);
    void setOnClick(std::function<void()> cb);


    void handleEvent(const sf::Event& e, const sf::RenderWindow& window);
    void update(float /*dt*/);


    // Helpers
    sf::FloatRect bounds() const; // global bounds of the button rect

    sf::Vector2f getSize() const;

private:
    sf::RectangleShape box_{};
    sf::Text label_{};
    std::function<void()> onClick_{};
    bool hovered_ = false;

    sf::Vector2f cached_size_{0.f, 0.f};

    void updateLabelOrigin();
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};



#endif //BUTTON_H
