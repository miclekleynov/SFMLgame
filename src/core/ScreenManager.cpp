//
// Created by mishka on 15.09.2025.
//

#include "ScreenManager.h"

ScreenManager::ScreenManager(AppContext ctx) : ctx_(ctx) {
    ctx_.manager = this; // allow screens to switch
}

void ScreenManager::registerFactory(ScreenID id, Factory factory) {
    factories_[id] = std::move(factory);
}

void ScreenManager::switchTo(ScreenID id) {
    auto it = factories_.find(id);
    if (it == factories_.end()) return;
    current_ = it->second(ctx_);
}

void ScreenManager::handleEvent(const sf::Event& e) {
    if (current_) current_->handleEvent(e);
}

void ScreenManager::update(float dt) {
    if (current_) current_->update(dt);
}

void ScreenManager::draw() {
    if (current_) current_->draw(*ctx_.window);
}
