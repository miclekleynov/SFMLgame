//
// Created by mishka on 15.09.2025.
//

#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#pragma once
#include <memory>
#include <map>
#include <functional>
#include "Screen.h"

enum class ScreenID { MainMenu, Base, Excavation, City, Collection, Workshop, FoodShop, Bank };

class ScreenManager {
public:
    using Factory = std::function<std::unique_ptr<Screen>(AppContext)>;

    explicit ScreenManager(AppContext ctx);

    void registerFactory(ScreenID id, Factory factory);
    void switchTo(ScreenID id);

    void handleEvent(const sf::Event& e);
    void update(float dt);
    void draw();

private:
    AppContext ctx_{};
    std::map<ScreenID, Factory> factories_;
    std::unique_ptr<Screen> current_;
};

#endif //SCREENMANAGER_H
