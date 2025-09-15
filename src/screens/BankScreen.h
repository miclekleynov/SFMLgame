//
// Created by mishka on 15.09.2025.
//

#ifndef BANKSCREEN_H
#define BANKSCREEN_H

#pragma once
#include <optional>
#include <vector>
#include <SFML/Graphics.hpp>
#include "core/Screen.h"
#include "core/ScreenManager.h"
#include "core/GameState.h"
#include "resources/DBManager.h"
#include "resources/Fonts.h"
#include "ui/Button.h"
#include "ui/Sidebar.h"

class BankScreen : public Screen {
public:
    BankScreen(AppContext context, GameState& state, DBManager& db_manager);

    void handleEvent(const sf::Event& e) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& target) override;

private:
    GameState& game_state_;
    DBManager& db_manager_;

    Sidebar sidebar_;
    sf::Text title_text_;
    sf::Text result_text_;

    std::vector<Button> option_buttons_;
    Button back_button_;

    std::optional<ScreenID> pending_switch_;
};


#endif //BANKSCREEN_H
