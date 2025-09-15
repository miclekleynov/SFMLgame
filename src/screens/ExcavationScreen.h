//
// Created by mishka on 15.09.2025.
//

#ifndef EXCAVATIONSCREEN_H
#define EXCAVATIONSCREEN_H

#pragma once
#include <optional>
#include <random>
#include <SFML/Graphics.hpp>
#include "core/Screen.h"
#include "core/ScreenManager.h"
#include "core/GameState.h"
#include "resources/DBManager.h"
#include "resources/Fonts.h"
#include "ui/Sidebar.h"
#include "ui/Button.h"
#include "game_logic/ExcavationData.h"

class ExcavationScreen : public Screen {
public:
    ExcavationScreen(AppContext context, GameState& state, DBManager& db_manager);

    void handleEvent(const sf::Event& e) override;
    void update(float) override;
    void draw(sf::RenderTarget& target) override;

private:
    enum class Phase { SelectLocation, InLocation };

    GameState& game_state_;
    DBManager& db_manager_;

    Sidebar sidebar_;
    sf::Text title_text_;
    sf::Text info_text_;

    std::vector<Button> location_buttons_;
    Button back_to_base_button_;

    std::vector<Button> choice_buttons_;
    Button leave_button_;
    Button continue_button_;

    sf::Text right_loc_title_;
    sf::Text right_progress_text_;

    std::optional<ScreenID> pending_switch_;

    ExcavationData data_;
    Phase phase_ = Phase::SelectLocation;
    int selected_location_ = -1;

    std::mt19937 rng_ { std::random_device{}() };
    int current_event_index_ = -1;

    bool showing_result_ = false;

private:
    void build_locations_ui();
    void build_event_ui();

    void enter_location(int loc_index);
    void leave_location();

    void roll_new_event();
    void on_choice_clicked(size_t choice_index);

    void refresh_right_hud();
};
#endif //EXCAVATIONSCREEN_H
