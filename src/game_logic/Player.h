//
// Created by mishka on 15.09.2025.
//

#ifndef PLAYER_H
#define PLAYER_H

class Food;

class Player
{
private:
    int money_;
    int stamina_;

public:
    Player(const int money, const int stamina);

    void work(const int stamina);

    void spend_money(const int money);

    void earn_money(const int money);

    bool is_tired() const
    {
        return stamina_ == 0;
    }

    void eat(const Food& food);

    int read_money() const
    {
        return money_;
    }

    int read_stamina() const
    {
        return stamina_;
    }

};



#endif //PLAYER_H
