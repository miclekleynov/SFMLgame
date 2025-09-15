//
// Created by mishka on 15.09.2025.
//

#ifndef TOOL_H
#define TOOL_H

#include <iostream>

class Tool
{
private:
    int id_;
    std::string name_;
    int repair_cost_;
    int durability_;


public:
    Tool(const int id, const std::string& name, const int repair_cost, const int durability);

    std::string read_name() const
    {
        return name_;
    }

    int read_cur_durability() const
    {
        return durability_;
    }

    int read_repair_cost() const
    {
        return repair_cost_;
    }

    int read_id() const
    {
        return id_;
    }

    void repair();

    void use_tool(const int dur_cost);
};



#endif //TOOL_H
