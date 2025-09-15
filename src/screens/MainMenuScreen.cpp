//
// Created by mishka on 15.09.2025.
//

#include "screens/MainMenuScreen.h"
#include "ui/Layout.h"

MainMenuScreen::MainMenuScreen(AppContext context)
    : Screen(context) {
    // Title
    title_text_.setFont(Fonts::Main());
    title_text_.setFillColor(sf::Color::White);
    title_text_.setCharacterSize(32);
    title_text_.setString("Main Menu");
    Layout::centerTextX(title_text_, *ctx_.window, 40.f);

    // Play button
    play_button_.setSize({280.f, 64.f});
    play_button_.setText("Play", Fonts::Main(), 24);
    Layout::centerButtonX(play_button_, *ctx_.window, Layout::belowTextY(title_text_, 40.f));
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
    target.clear(sf::Color::Black);
    target.draw(title_text_);
    target.draw(play_button_);
}