//
// Created by mishka on 15.09.2025.
//

#ifndef INVENTORY_H
#define INVENTORY_H
#include <vector>
#include "Tool.h"


class Inventory {
private:
    std::vector<Tool> tools_;

public:
    Inventory(const std::vector<Tool>& tools);

    Tool& get_tool_by_id(const int id);

    std::vector<Tool> read_tools() const {
        return tools_;
    };

};


#endif //INVENTORY_H
