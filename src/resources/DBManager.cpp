//
// Created by mishka on 15.09.2025.
//

#include "DBManager.h"
#include "../game_logic/Artifact.h"
#include "../game_logic/Tool.h"
#include "../game_logic/Player.h"
#include "../game_logic/Inventory.h"
#include "../game_logic/Food.h"
#include "../game_logic/FoodShop.h"
#include "../game_logic/Collection.h"
#include <fstream>


json DBManager::read_json(const std::string &file_path) const {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Can't open the file: " + file_path);
    }

    json data;
    file >> data;
    file.close();
    return data;
}

void DBManager::update_json(const std::string &file_path, const json& new_data) const {
    std::ofstream output(file_path);
    if (!output.is_open()) {
        throw std::runtime_error("Can't open the file: " + file_path);
    }
    output << new_data.dump(4);
    output.close();
}

bool DBManager::if_safe_exist() {
    std::ifstream file(SAVE_FILE_PATH);
    const bool result = file.is_open();
    return result;
}

void DBManager::setup_save() {
    json data;

    data["money"] = start_money;
    data["stamina"] = start_stamina;
    data["collection"] = json::array();
    data["event_progress"] = json::object();

    json tool_data = read_json(TOOLS_FILE_PATH);
    for (const auto& tool_info : tool_data.items()) {
        data["tools"][tool_info.key()]["cur_dur"] = start_dur;
    }

    update_json(SAVE_FILE_PATH, data);
}

static ExChoice::Tag parse_tag_str(const std::string& s){
    if (s == "artifact") return ExChoice::Tag::Artifact;
    if (s == "again")    return ExChoice::Tag::Again;
    return ExChoice::Tag::Fail;
}

ExcavationData DBManager::get_excavation_data() const {
    const json root = read_json(EVENTS_FILE_PATH);
    std::vector<ExLocation> locs;

    for (auto it = root.begin(); it != root.end(); ++it) {
        const std::string name = it.key();
        const json& jl = it.value();

        ExLocation loc; loc.name = name;
        loc.enter_cost = jl.at("enter_cost").get<int>();
        loc.choice_desc = jl.at("choice_desc").get<std::string>();
        loc.enter_desc  = jl.at("enter_desc").get<std::string>();
        loc.exit_desc   = jl.at("exit_desc").get<std::string>();

        const json& jevents = jl.at("events");
        for (auto e = jevents.begin(); e != jevents.end(); ++e) {
            const json& je = e.value();
            ExEvent ev; ev.description = je.at("description").get<std::string>();
            for (const auto& jc : je.at("choices")) {
                ExChoice ch;
                ch.description     = jc.at("description").get<std::string>();
                ch.tool_id         = jc.at("tool_id").get<int>();
                ch.stamina_cost    = jc.at("stamina_cost").get<int>();
                ch.durability_cost = jc.at("durability_cost").get<int>();
                const auto& jr = jc.at("result");
                ch.tag         = parse_tag_str(jr.at("tag").get<std::string>());
                ch.result_text = jr.at("description").get<std::string>();
                if (ch.tag == ExChoice::Tag::Artifact && jr.contains("artifact_id"))
                    ch.artifact_id = jr.at("artifact_id").get<int>();
                ev.choices.push_back(std::move(ch));
            }
            loc.events.push_back(std::move(ev));
        }
        locs.push_back(std::move(loc));
    }

    ExcavationData data; data.set_locations(std::move(locs));
    return data;
}

int DBManager::get_location_completed_count(const std::string& loc_name) const {
    const json save = read_json(SAVE_FILE_PATH);
    if (!save.contains("event_progress")) return 0;
    const auto& prog = save.at("event_progress");
    if (!prog.contains(loc_name)) return 0;
    return static_cast<int>(prog.at(loc_name).size());
}

std::vector<int> DBManager::get_location_completed_list(const std::string& loc_name) const {
    std::vector<int> out;
    const json save = read_json(SAVE_FILE_PATH);
    if (!save.contains("event_progress")) return out;
    const auto& prog = save.at("event_progress");
    if (!prog.contains(loc_name)) return out;
    for (const auto& v : prog.at(loc_name)) out.push_back(v.get<int>());
    return out;
}

void DBManager::mark_event_completed(const std::string& loc_name, const int event_index) const {
    json save = read_json(SAVE_FILE_PATH);
    auto& prog = save["event_progress"]; // создастся при отсутствии

    // убедимся, что для локации есть массив
    if (!prog.contains(loc_name)) prog[loc_name] = json::array();

    // уникально добавляем индекс
    bool has = false;
    for (const auto& v : prog[loc_name]) if (v.get<int>() == event_index) { has = true; break; }
    if (!has) prog[loc_name].push_back(event_index);

    update_json(SAVE_FILE_PATH, save);
}


Artifact DBManager::get_artifact_from_id(const int id) const {
    json data = read_json(ARTIFACT_FILE_PATH);
    json artifact_data = data.at(std::to_string(id));

    const int cost = artifact_data.at("cost").get<int>();
    const std::string& name = artifact_data.at("name").get<std::string>();
    const std::string& description = artifact_data.at("description").get<std::string>();

    return Artifact(id, cost, name, description);
}

Tool DBManager::get_tool_from_id(const int id) const {

    json save_data = read_json(SAVE_FILE_PATH);
    json tools_data = read_json(TOOLS_FILE_PATH);;
    json tool_data = tools_data.at(std::to_string(id));
    json tool_save_data = save_data.at("tools").at(std::to_string(id));

    const std::string& name = tool_data.at("name");
    const int repair_cost = tool_data.at("repair_cost");
    const int durability = tool_save_data.at("cur_dur");
    return Tool(id, name, repair_cost, durability);
}

Inventory DBManager::get_inventory() const {
    json tool_data = read_json(TOOLS_FILE_PATH);
    std::vector<Tool> tools;

    for (const auto& tool_info : tool_data.items()) {

        const int tool_id = std::stoi(tool_info.key());
        Tool tool = get_tool_from_id(tool_id);
        tools.push_back(tool);
    }
    return Inventory(tools);
}

Collection DBManager::get_collection() const {
    json data = read_json(SAVE_FILE_PATH);
    std::vector<Artifact> artifacts;

    auto artifacts_ids = data.at("collection");
    for (const int artifact_id : artifacts_ids) {
        Artifact artifact = get_artifact_from_id(artifact_id);
        artifacts.push_back(artifact);
    }
    return Collection(artifacts);
}

Player DBManager::get_player() const {
    json data = read_json(SAVE_FILE_PATH);

    const int money = data.at("money");
    const int stamina = data.at("stamina");
    return Player(money, stamina);
}

void DBManager::save_tool_data(const Tool &tool) const {
    json data = read_json(SAVE_FILE_PATH);

    const int id = tool.read_id();
    const int durability = tool.read_cur_durability();

    data.at("tools").at(std::to_string(id)).at("cur_dur") = durability;

    update_json(SAVE_FILE_PATH, data);
}

void DBManager::save_player_data(const Player& player) const {
    json data = read_json(SAVE_FILE_PATH);

    const int money = player.read_money();
    const int stamina = player.read_stamina();

    data.at("money") = money;
    data.at("stamina") = stamina;

    update_json(SAVE_FILE_PATH, data);
}

void DBManager::save_collection_data(const Collection &collection) const {
    json data = read_json(SAVE_FILE_PATH);
    std::vector<Artifact> artifacts = collection.read_artifacts();
    std::vector<int> artifact_ids;

    for (const Artifact& artifact : artifacts) {
        artifact_ids.push_back(artifact.read_id());
    }
    std::sort(artifact_ids.begin(), artifact_ids.end());
    data["collection"] = artifact_ids;

    update_json(SAVE_FILE_PATH, data);
}

Food DBManager::get_food_from_id(const int id) const {
    json data = read_json(FOOD_FILE_PATH);
    json food_data = data.at(std::to_string(id));

    const std::string& name = food_data.at("name").get<std::string>();
    const std::string& description = food_data.at("description").get<std::string>();
    const int cost = food_data.at("cost").get<int>();
    const int stamina_recovery = food_data.at("stamina_recovery").get<int>();

    return Food(id, name, description, cost, stamina_recovery);

}

FoodShop DBManager::get_food_shop() const {
    json data = read_json(FOOD_FILE_PATH);

    std::vector<Food> food_assortment;
    for (const auto& item : data.items())
    {
        int food_id = std::stoi(item.key());
        food_assortment.push_back(get_food_from_id(food_id));
    }

    return FoodShop(food_assortment);
}


