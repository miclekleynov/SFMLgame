//
// Created by mishka on 15.09.2025.
//

#ifndef COLLECTIONSCREEN_H
#define COLLECTIONSCREEN_H

#pragma once
#include <optional>
#include <vector>
#include <SFML/Graphics.hpp>
#include "core/Screen.h"
#include "core/GameState.h"
#include "core/ScreenManager.h"
#include "resources/DBManager.h"
#include "resources/Fonts.h"
#include "ui/Sidebar.h"
#include "ui/UiUtils.h"
#include "ui/Button.h"

class CollectionScreen : public Screen {
public:
    CollectionScreen(AppContext context, GameState& state, DBManager& db_manager);

    void handleEvent(const sf::Event& e) override;
    void update(float) override;
    void draw(sf::RenderTarget& target) override;

private:
    GameState& game_state_;
    DBManager& db_manager_;

    Sidebar sidebar_;
    sf::Text title_text_;
    sf::Text result_text_;

    std::vector<Button> option_buttons_;
    std::vector<int> visible_artifact_ids_;
    Button back_button_;

    std::optional<ScreenID> pending_switch_;

    void rebuild_buttons();
    void on_sell_clicked(size_t visual_index);
};


#endif //COLLECTIONSCREEN_H
