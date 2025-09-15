//
// Created by mishka on 15.09.2025.
//

#ifndef FONTS_H
#define FONTS_H

#pragma once
#include <SFML/Graphics/Font.hpp>
#include <string>


class Fonts {
public:
    static sf::Font& Main();
    static bool EnsureLoaded();
private:
    static bool loaded_;
    static sf::Font main_;
    static std::string GuessPath();
};



#endif //FONTS_H
