//
// Created by mishka on 15.09.2025.
//

#include "Food.h"

Food::Food(const int id, const std::string &name, const std::string &description, const int cost, const int stamina_rec) {
    id_ = id;
    name_ = name;
    description_ = description;
    cost_ = cost;
    stamina_recovery_ = stamina_rec;
}
