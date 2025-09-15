//
// Created by mishka on 15.09.2025.
//

#include "Inventory.h"

#include <stdexcept>

Inventory::Inventory(const std::vector<Tool>& tools) {
    tools_ = tools;
}

Tool& Inventory::get_tool_by_id(const int id)
{
    for (Tool& tool : tools_)
    {
        if (tool.read_id() == id)
        {
            return tool;
        }
    }
    throw std::runtime_error("Tool with id " + std::to_string(id) + " not found in inventory");
}
