//
// Created by mishka on 15.09.2025.
//

#include "screens/MainMenuScreen.h"
#include "core/ScreenManager.h"
#include "ui/UiUtils.h"

MainMenuScreen::MainMenuScreen(AppContext context) : Screen(context) {
    const auto size = context.window->getSize();
    const float center_x = size.x * 0.5f;

    play_button_.setSize({260.f, 64.f});
    play_button_.setPosition({center_x - 130.f, size.y * 0.5f - 32.f});
    play_button_.setText("Play", Fonts::Main(), 28);
    play_button_.setOnClick([this]{ pending_switch_ = ScreenID::Base; });
}

void MainMenuScreen::handleEvent(const sf::Event& e) {
    play_button_.handleEvent(e, *ctx_.window);
}

void MainMenuScreen::update(float) {
    if (pending_switch_ && ctx_.manager) {
        const ScreenID id = *pending_switch_;
        pending_switch_.reset();
        ctx_.manager->switchTo(id);
    }
}

void MainMenuScreen::draw(sf::RenderTarget& target) {
    target.draw(play_button_);
}