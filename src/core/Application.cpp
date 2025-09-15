//
// Created by mishka on 15.09.2025.
//

#include "Application.h"
#include "resources/Fonts.h"
#include "screens/MainMenuScreen.h"
#include "screens/BaseScreen.h"
#include "screens/CityScreen.h"
#include "screens/WorkshopScreen.h"
#include "screens/FoodShopScreen.h"
#include "screens/BankScreen.h"
#include "screens/CollectionScreen.h"
#include "screens/ExcavationScreen.h"
#include <algorithm>

void Application::ensure_save() {
    if (!db_manager_.if_safe_exist()) {
        db_manager_.setup_save();
    }
}

Application::Application()
    : window_(), screens_({ &window_, nullptr }) // manager set inside ScreenManager
{
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window_.create(desktop, "Paleo Simulator", sf::Style::Titlebar | sf::Style::Close);
    window_.setVerticalSyncEnabled(true);

    Fonts::EnsureLoaded();

    // 1) Ensure we have a save file, then 2) load models into memory
    ensure_save();
    state_.load_from_db(db_manager_);

    screens_.registerFactory(ScreenID::Base,     [this](AppContext c){ return std::make_unique<BaseScreen>(c, state_, db_manager_); });
    screens_.registerFactory(ScreenID::MainMenu, [](AppContext c){ return std::make_unique<MainMenuScreen>(c); });
    screens_.registerFactory(ScreenID::City,     [this](AppContext c){ return std::make_unique<CityScreen>(c, state_, db_manager_); });
    screens_.registerFactory(ScreenID::Workshop, [this](AppContext c){ return std::make_unique<WorkshopScreen>(c, state_, db_manager_); });
    screens_.registerFactory(ScreenID::FoodShop, [this](AppContext c){ return std::make_unique<FoodShopScreen>(c, state_, db_manager_); });
    screens_.registerFactory(ScreenID::Bank,     [this](AppContext c){ return std::make_unique<BankScreen>(c, state_, db_manager_); });
    screens_.registerFactory(ScreenID::Collection, [this](AppContext c){return std::make_unique<CollectionScreen>(c, state_, db_manager_);});
    screens_.registerFactory(ScreenID::Excavation, [this](AppContext c){return std::make_unique<ExcavationScreen>(c, state_, db_manager_);});

    screens_.switchTo(ScreenID::MainMenu);
}

void Application::run() {
    sf::Clock clock;
    while (window_.isOpen()) {
        sf::Event e{};
        while (window_.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window_.close();
            screens_.handleEvent(e);
        }

        float dt = clock.restart().asSeconds();
        screens_.update(dt);

        window_.clear(sf::Color::Black);
        screens_.draw();
        window_.display();
    }
}