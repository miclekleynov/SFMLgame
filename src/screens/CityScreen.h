//
// Created by mishka on 15.09.2025.
//

#ifndef CITYSCREEN_H
#define CITYSCREEN_H

#pragma once
#include <optional>
#include <SFML/Graphics.hpp>
#include "core/Screen.h"
#include "core/ScreenManager.h"
#include "core/GameState.h"
#include "resources/DBManager.h"
#include "resources/Fonts.h"
#include "ui/Button.h"
#include "ui/Sidebar.h"

class CityScreen : public Screen {
public:
    CityScreen(AppContext context, GameState& state, DBManager& db_manager);

    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& target) override;

private:
    GameState& game_state_;
    DBManager& db_manager_;

    Sidebar sidebar_;
    sf::Text title_text_;

    Button foodshop_button_;
    Button workshop_button_;
    Button bank_button_;
    Button back_button_;

    std::optional<ScreenID> pending_switch_;
};
#endif //CITYSCREEN_H
