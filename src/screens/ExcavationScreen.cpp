//
// Created by mishka on 15.09.2025.
//

#include "screens/ExcavationScreen.h"
#include "game_logic/Tool.h"
#include "game_logic/Artifact.h"
#include "game_logic/Collection.h"
#include "ui/Layout.h"
#include <unordered_set>

ExcavationScreen::ExcavationScreen(AppContext context, GameState& state, DBManager& db_manager)
    : Screen(context), game_state_(state), db_manager_(db_manager) {
    data_ = db_manager_.get_excavation_data();

    const auto size = context.window->getSize();

    // Sidebar слева
    sidebar_.set_position({0.f, 0.f});
    sidebar_.set_provider([this]{
        BaseStats st{}; st.money = game_state_.player.read_money(); st.satiety = game_state_.player.read_stamina();
        int px=0,sh=0,br=0; for (const auto& t : game_state_.inventory.read_tools()){
            std::string n=t.read_name(); for(auto& c:n) c=(char)std::tolower(c);
            if(n.find("pickaxe")!=std::string::npos) px=t.read_cur_durability();
            else if(n.find("shovel")!=std::string::npos) sh=t.read_cur_durability();
            else if(n.find("brush")!=std::string::npos) br=t.read_cur_durability();
        } st.pickaxe=px; st.shovel=sh; st.brush=br; return st; });

    // Заголовки по центру
    title_text_.setFont(Fonts::Main()); title_text_.setFillColor(sf::Color::White);
    title_text_.setCharacterSize(32); title_text_.setString("Excavation");
    Layout::centerTextX(title_text_, *ctx_.window, 20.f);

    info_text_.setFont(Fonts::Main()); info_text_.setFillColor(sf::Color::White);
    info_text_.setCharacterSize(20); info_text_.setString("");
    Layout::centerTextX(info_text_, *ctx_.window, 80.f);

    // Кнопки внутри локации — по центру
    leave_button_.setSize({160.f, 44.f});
    Layout::centerButtonX(leave_button_, *ctx_.window, static_cast<float>(size.y) - 70.f);
    leave_button_.setText("Leave", Fonts::Main(), 20);
    leave_button_.setOnClick([this]{ leave_location(); });

    continue_button_.setSize({180.f, 44.f});
    Layout::centerButtonX(continue_button_, *ctx_.window, static_cast<float>(size.y) * 0.6f);
    continue_button_.setText("Continue", Fonts::Main(), 20);
    continue_button_.setOnClick([this]{
        if (selected_location_ < 0) return;
        const auto& loc = data_.locations()[selected_location_];
        const int done  = db_manager_.get_location_completed_count(loc.name);
        const int total = (int)loc.events.size();
        showing_result_ = false;
        if (done >= total) { leave_location(); }
        else { roll_new_event(); }
    });

    // Back на базу при выборе локации
    back_to_base_button_.setSize({160.f, 44.f});
    Layout::centerButtonX(back_to_base_button_, *ctx_.window, static_cast<float>(size.y) - 70.f);
    back_to_base_button_.setText("Back", Fonts::Main(), 20);
    back_to_base_button_.setOnClick([this]{ pending_switch_ = ScreenID::Base; });

    // Правый HUD
    right_loc_title_.setFont(Fonts::Main()); right_loc_title_.setFillColor(sf::Color::White); right_loc_title_.setCharacterSize(22);
    right_progress_text_.setFont(Fonts::Main()); right_progress_text_.setFillColor(sf::Color::White); right_progress_text_.setCharacterSize(18);

    build_locations_ui();
}

void ExcavationScreen::build_locations_ui() {
    location_buttons_.clear();

    // Кнопки локаций начинаются чуть ниже info_text_
    float y = Layout::belowTextY(info_text_, 40.f);
    const float button_width = std::max(480.f, static_cast<float>(ctx_.window->getSize().x) * 0.55f);

    const auto& L = data_.locations();
    for (size_t i=0; i<L.size(); ++i) {
        const auto& loc = L[i];
        const int done  = db_manager_.get_location_completed_count(loc.name);
        const int total = static_cast<int>(loc.events.size());
        Button b; b.setSize({button_width, 52});
        Layout::centerButtonX(b, *ctx_.window, y);
        std::string label = loc.name + " — enter: " + std::to_string(loc.enter_cost) + "$";
        label += " (Events: " + std::to_string(done) + "/" + std::to_string(total) + ") — ";
        label += loc.choice_desc; // всё одной строкой
        if (done >= total && total > 0) label += " — [Completed]";
        b.setText(label, Fonts::Main(), 18);
        b.setOnClick([this, i]{ enter_location((int)i); });
        location_buttons_.push_back(b);
        y += 64.f;
    }
}

void ExcavationScreen::enter_location(int loc_index) {
    if (loc_index < 0 || loc_index >= (int)data_.locations().size()) return;
    const auto& loc = data_.locations()[loc_index];

    const int done  = db_manager_.get_location_completed_count(loc.name);
    const int total = (int)loc.events.size();
    if (total > 0 && done >= total) {
        info_text_.setString("Location '" + loc.name + "' is already completed.");
        Layout::centerTextX(info_text_, *ctx_.window, 80.f);
        return; // Не входим, деньги не списываем
    }

    const int money = game_state_.player.read_money();
    if (money < loc.enter_cost) {
        info_text_.setString("Not enough money to enter " + loc.name + ". Need " + std::to_string(loc.enter_cost - money) + "$ more.");
        Layout::centerTextX(info_text_, *ctx_.window, 80.f);
        return;
    }

    game_state_.player.spend_money(loc.enter_cost);
    db_manager_.save_player_data(game_state_.player);

    selected_location_ = loc_index;
    phase_ = Phase::InLocation;
    showing_result_ = false;
    info_text_.setString(loc.enter_desc);
    Layout::centerTextX(info_text_, *ctx_.window, 80.f);

    refresh_right_hud();
    roll_new_event();
}

void ExcavationScreen::leave_location() {
    if (selected_location_ >= 0) {
        info_text_.setString(data_.locations()[selected_location_].exit_desc);
        Layout::centerTextX(info_text_, *ctx_.window, 80.f);
    }
    selected_location_ = -1;
    phase_ = Phase::SelectLocation;
    showing_result_ = false;
    build_locations_ui();
}

void ExcavationScreen::roll_new_event() {
    choice_buttons_.clear();
    current_event_index_ = -1;

    const auto& loc = data_.locations()[selected_location_];
    const int total = static_cast<int>(loc.events.size());
    if (total == 0) { leave_location(); return; }

    const std::vector<int> done = db_manager_.get_location_completed_list(loc.name);
    std::unordered_set<int> done_set(done.begin(), done.end());
    std::vector<int> remaining; remaining.reserve(total);
    for (int i = 0; i < total; ++i) if (!done_set.count(i)) remaining.push_back(i);

    if (remaining.empty()) {
        continue_button_.setText("Leave", Fonts::Main(), 20);
        showing_result_ = true;
        return;
    }

    std::uniform_int_distribution<int> dist(0, (int)remaining.size()-1);
    current_event_index_ = remaining[dist(rng_)];

    build_event_ui();
}

void ExcavationScreen::build_event_ui() {
    choice_buttons_.clear();

    const auto& loc = data_.locations()[selected_location_];
    const auto& ev = loc.events.at(current_event_index_);

    if (!showing_result_) {
        info_text_.setString(ev.description);
        Layout::centerTextX(info_text_, *ctx_.window, 80.f);
    }

    float y = Layout::belowTextY(info_text_, 40.f);
    const float button_width = std::max(480.f, static_cast<float>(ctx_.window->getSize().x) * 0.55f);

    for (size_t i=0; i<ev.choices.size(); ++i) {
        const auto& ch = ev.choices[i];
        Button b; b.setSize({button_width, 48});
        Layout::centerButtonX(b, *ctx_.window, y);
        std::string tool;
        switch (ch.tool_id) { case 0: tool = "Pickaxe"; break; case 1: tool = "Shovel"; break; case 2: tool = "Brush"; break; default: tool = "Tool"; }
        const std::string label = ch.description +
            "  [tool: " + tool + ", -" + std::to_string(ch.stamina_cost) + "% stamina, -" + std::to_string(ch.durability_cost) + "% dur]";
        b.setText(label, Fonts::Main(), 18);
        b.setOnClick([this, i]{ on_choice_clicked(i); });
        choice_buttons_.push_back(b);
        y += 64.f;
    }
}

void ExcavationScreen::on_choice_clicked(size_t choice_index) {
    const auto& loc = data_.locations()[selected_location_];
    const auto& ev = loc.events.at(current_event_index_);
    if (choice_index >= ev.choices.size()) return;
    const auto& ch = ev.choices[choice_index];

    try {
        Tool& tool = game_state_.inventory.get_tool_by_id(ch.tool_id);
        const int cur_dur = tool.read_cur_durability();
        const int cur_sta = game_state_.player.read_stamina();
        if (cur_sta < ch.stamina_cost) { info_text_.setString("Not enough stamina."); Layout::centerTextX(info_text_, *ctx_.window, 80.f); showing_result_ = true; continue_button_.setText("Continue", Fonts::Main(), 20); return; }
        if (cur_dur < ch.durability_cost) { info_text_.setString("Your tool is too worn out."); Layout::centerTextX(info_text_, *ctx_.window, 80.f); showing_result_ = true; continue_button_.setText("Continue", Fonts::Main(), 20); return; }

        game_state_.player.work(ch.stamina_cost);
        tool.use_tool(ch.durability_cost);
        db_manager_.save_player_data(game_state_.player);
        db_manager_.save_tool_data(tool);

        if (ch.tag == ExChoice::Tag::Artifact) {
            Artifact art = db_manager_.get_artifact_from_id(ch.artifact_id);
            game_state_.collection.add_artifact(art);
            db_manager_.save_collection_data(game_state_.collection);
            const std::string line = "You found \"" + art.read_name() + "\" — " + art.read_description() +
                                     " (worth " + std::to_string(art.read_cost()) + "$). Added to collection.";
            info_text_.setString(line);
        } else if (ch.tag == ExChoice::Tag::Again) {
            info_text_.setString("Try again: " + ch.result_text);
        } else {
            info_text_.setString("Fail: " + ch.result_text);
        }
        Layout::centerTextX(info_text_, *ctx_.window, 80.f);

        db_manager_.mark_event_completed(loc.name, current_event_index_);
        refresh_right_hud();

        const int done = db_manager_.get_location_completed_count(loc.name);
        const int total = (int)loc.events.size();
        showing_result_ = true;
        continue_button_.setText(done >= total ? "Leave" : "Continue", Fonts::Main(), 20);
    } catch (const std::exception&) {
        info_text_.setString("Action failed.");
        Layout::centerTextX(info_text_, *ctx_.window, 80.f);
        showing_result_ = true; continue_button_.setText("Continue", Fonts::Main(), 20);
    }
}

void ExcavationScreen::refresh_right_hud() {
    if (selected_location_ < 0) return;
    const auto& loc = data_.locations()[selected_location_];
    const int done = db_manager_.get_location_completed_count(loc.name);
    const int total = static_cast<int>(loc.events.size());

    right_loc_title_.setString("Location: " + loc.name);
    right_progress_text_.setString("Events: " + std::to_string(done) + "/" + std::to_string(total));

    const auto size = ctx_.window->getSize();
    const float panel_x = static_cast<float>(size.x) - 320.f + 16.f; // справа отступ
    right_loc_title_.setPosition({panel_x, 16.f});
    right_progress_text_.setPosition({panel_x, 46.f});
}

void ExcavationScreen::handleEvent(const sf::Event& e) {
    if (phase_ == Phase::SelectLocation) {
        for (auto& b : location_buttons_) b.handleEvent(e, *ctx_.window);
        back_to_base_button_.handleEvent(e, *ctx_.window);
    } else {
        if (showing_result_) {
            continue_button_.handleEvent(e, *ctx_.window);
            leave_button_.handleEvent(e, *ctx_.window);
        } else {
            for (auto& b : choice_buttons_) b.handleEvent(e, *ctx_.window);
            leave_button_.handleEvent(e, *ctx_.window);
        }
    }
}

void ExcavationScreen::update(float) {
    if (pending_switch_ && ctx_.manager) {
        const ScreenID id = *pending_switch_;
        pending_switch_.reset();
        ctx_.manager->switchTo(id);
    }
}

void ExcavationScreen::draw(sf::RenderTarget& target) {
    sidebar_.draw(target);
    target.draw(title_text_);
    target.draw(info_text_);

    if (phase_ == Phase::SelectLocation) {
        for (auto& b : location_buttons_) target.draw(b);
        target.draw(back_to_base_button_);
    } else {
        if (showing_result_) {
            target.draw(continue_button_);
        } else {
            for (auto& b : choice_buttons_) target.draw(b);
        }
        target.draw(leave_button_);
        target.draw(right_loc_title_);
        target.draw(right_progress_text_);
    }
}