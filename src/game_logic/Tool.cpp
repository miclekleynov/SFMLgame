//
// Created by mishka on 15.09.2025.
//

#include "Tool.h"

Tool::Tool(const int id, const std::string &name, const int repair_cost, const int durability) {
    id_ = id;
    name_ = name;
    repair_cost_ = repair_cost;
    durability_ = durability;
}

void Tool::repair() {
    durability_ = 100;
}

void Tool::use_tool(const int dur_cost) {
    durability_ = std::max(0, durability_ - dur_cost);
}
