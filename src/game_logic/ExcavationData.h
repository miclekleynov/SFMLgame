//
// Created by mishka on 15.09.2025.
//

#ifndef EXCAVATIONDATA_H
#define EXCAVATIONDATA_H

#pragma once
#include <string>
#include <vector>

struct ExChoice {
    std::string description;   // текст кнопки
    int tool_id = -1;          // 0 pickaxe, 1 shovel, 2 brush
    int stamina_cost = 0;
    int durability_cost = 0;
    enum class Tag { Artifact, Fail, Again } tag = Tag::Fail;
    int artifact_id = -1;      // если tag == Artifact
    std::string result_text;   // результат для игрока
};

struct ExEvent {
    std::string description;       // описание ситуации
    std::vector<ExChoice> choices; // варианты действий
};

struct ExLocation {
    std::string name;      // имя локации (ключ из json)
    int enter_cost = 0;    // стоимость входа
    std::string choice_desc;
    std::string enter_desc;
    std::string exit_desc;
    std::vector<ExEvent> events; // пул событий
};

// Контейнер без IO
class ExcavationData {
public:
    const std::vector<ExLocation>& locations() const { return locations_; }
    void set_locations(std::vector<ExLocation> v) { locations_ = std::move(v); }
private:
    std::vector<ExLocation> locations_;
};


#endif //EXCAVATIONDATA_H
