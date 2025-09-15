//
// Created by mishka on 15.09.2025.
//

#ifndef FOOD_H
#define FOOD_H

#include <iostream>

class Food
{
private:
    int id_;
    std::string name_;
    std::string description_;
    int cost_;
    int stamina_recovery_;

public:
    Food(const int id, const std::string& name, const std::string& description, const int cost, const int stamina_rec);

    int read_id() const {
        return id_;
    }

    int read_stamina() const
    {
        return stamina_recovery_;
    }

    int read_cost() const
    {
        return cost_;
    }

    std::string read_description() const
    {
        return description_;
    }

    std::string read_name() const
    {
        return name_;
    }
};


#endif //FOOD_H
