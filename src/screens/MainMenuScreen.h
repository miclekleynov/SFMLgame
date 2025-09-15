//
// Created by mishka on 15.09.2025.
//

#ifndef MAINMENUSCREEN_H
#define MAINMENUSCREEN_H

#pragma once
#include <optional>
#include <SFML/Graphics.hpp>
#include "core/Screen.h"
#include "core/ScreenManager.h" // ScreenID
#include "resources/Fonts.h"
#include "ui/Button.h"

class MainMenuScreen : public Screen {
public:
    explicit MainMenuScreen(AppContext context);

    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& target) override;

private:
    sf::Text title_text_;
    Button play_button_;

    std::optional<ScreenID> pending_switch_;
};

#endif //MAINMENUSCREEN_H
