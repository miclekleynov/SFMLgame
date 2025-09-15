//
// Created by mishka on 15.09.2025.
//

#ifndef GAMESTATE_H
#define GAMESTATE_H

#pragma once
#include "resources/DBManager.h"
#include "game_logic/Player.h"
#include "game_logic/Inventory.h"
#include "game_logic/Collection.h"

// Central place to keep the current in-memory game model.
// Load once from DBManager; scenes mutate these objects and persist via DBManager.
struct GameState {
    Player player{0,0};
    Inventory inventory{{}}; // constructed from vector<Tool>
    Collection collection{{}}; // constructed from vector<Artifact>

    void load_from_db(DBManager& db) {
        player     = db.get_player();
        inventory  = db.get_inventory();
        collection = db.get_collection();
    }
};


#endif //GAMESTATE_H
