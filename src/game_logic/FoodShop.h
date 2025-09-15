//
// Created by mishka on 15.09.2025.
//

#ifndef FOODSHOP_H
#define FOODSHOP_H
#include <vector>
#include "Food.h"

class FoodShop {
private:
    std::vector<Food> food_assortment_;

public:
    FoodShop(const std::vector<Food>& food_assortment);

    std::vector<Food> read_food_assortment()
    {
        return food_assortment_;
    }
};



#endif //FOODSHOP_H
