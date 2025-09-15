//
// Created by mishka on 15.09.2025.
//

#ifndef DBMANAGER_H
#define DBMANAGER_H

#pragma once
#include <nlohmann/json.hpp>
#include <string>

// forwardы твоих игровых классов
class Tool; class Artifact; class Player; class Inventory; class Collection; class FoodShop; class Food;

// типы раскопок
#include "../game_logic/ExcavationData.h"

using json = nlohmann::json;

class DBManager {
private:
    const std::string SAVE_FILE_PATH   = "../save.json";
    const std::string EVENTS_FILE_PATH = "../content/events.json";
    const std::string TOOLS_FILE_PATH  = "../content/tools.json";
    const std::string ARTIFACT_FILE_PATH = "../content/artifacts.json";
    const std::string FOOD_FILE_PATH   = "../content/food.json";

    const int start_money = 100;
    const int start_stamina = 100;
    const int start_dur = 100;

    json read_json(const std::string& file_path) const;
    void update_json(const std::string& file_path, const json& new_data) const;

public:
    bool if_safe_exist();
    void setup_save();

    // --- раскопки ---
    ExcavationData get_excavation_data() const; // НОВОЕ: парсит events.json

    int  get_location_completed_count(const std::string& loc_name) const;         // сколько уникальных ивентов завершено
    void mark_event_completed(const std::string& loc_name, int event_index) const;
    std::vector<int> get_location_completed_list(const std::string& loc_name) const;

    // --- предметы/игрок ---
    Artifact  get_artifact_from_id(int id) const;
    Tool      get_tool_from_id(int id) const;
    Inventory get_inventory() const;
    Collection get_collection() const;
    Player    get_player() const;

    void save_tool_data(const Tool& tool) const;
    void save_player_data(const Player& player) const;
    void save_collection_data(const Collection& collection) const;

    // --- еда ---
    Food     get_food_from_id(int id) const;
    FoodShop get_food_shop() const;
};



#endif //DBMANAGER_H
