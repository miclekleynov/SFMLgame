//
// Created by mishka on 15.09.2025.
//

#include "Player.h"
#include "Food.h"

#include <algorithm>

Player::Player(const int money, const int stamina) {
    money_ = money;
    stamina_ = stamina;
}

void Player::work(const int stamina) {
    stamina_ = std::max(0, stamina_ - stamina);
}

void Player::spend_money(const int money) {
    money_ = std::max(0, money_ - money);
}

void Player::earn_money(const int money) {
    money_ += money;
}

void Player::eat(const Food &food) {
    const int stamina_rec = food.read_stamina();
    stamina_ = std::min(100, stamina_ + stamina_rec);
}
