//
// Created by mishka on 15.09.2025.
//

#ifndef APPLICATION_H
#define APPLICATION_H

#pragma once
#include <SFML/Graphics.hpp>
#include "ScreenManager.h"
#include "resources/DBManager.h"
#include "core/GameState.h"

class Application {
public:
    Application();
    void run();
private:
    sf::RenderWindow window_;
    ScreenManager screens_;

    DBManager db_manager_{}; // resources/DBManager.h
    GameState state_{};      // Player, Inventory, Collection

    void ensure_save();      // create save if it doesn't exist
};

#endif //APPLICATION_H
