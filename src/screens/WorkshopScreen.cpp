//
// Created by mishka on 15.09.2025.
//

#include "screens/WorkshopScreen.h"
#include "game_logic/Tool.h" // complete type

WorkshopScreen::WorkshopScreen(AppContext context, GameState& state, DBManager& db_manager)
    : Screen(context), game_state_(state), db_manager_(db_manager) {
    const auto size = context.window->getSize();
    const float cx = size.x * 0.5f;

    sidebar_.set_position({0.f, 0.f});
    sidebar_.set_provider([this]{
        BaseStats st{}; st.money = game_state_.player.read_money(); st.satiety = game_state_.player.read_stamina();
        int px=0,sh=0,br=0; for (const auto& t : game_state_.inventory.read_tools()){
            std::string n=t.read_name(); for(auto& c:n) c=(char)std::tolower(c);
            if(n.find("pickaxe")!=std::string::npos) px=t.read_cur_durability();
            else if(n.find("shovel")!=std::string::npos) sh=t.read_cur_durability();
            else if(n.find("brush")!=std::string::npos) br=t.read_cur_durability();
        } st.pickaxe=px; st.shovel=sh; st.brush=br; return st; });

    title_text_.setFont(Fonts::Main()); title_text_.setFillColor(sf::Color::White);
    title_text_.setCharacterSize(32); title_text_.setString("Workshop");
    title_text_.setPosition(cx, 20.f); center_text_horiz(title_text_, cx);

    result_text_.setFont(Fonts::Main()); result_text_.setFillColor(sf::Color::White);
    result_text_.setCharacterSize(20); result_text_.setString("");
    result_text_.setPosition(cx, 80.f); center_text_horiz(result_text_, cx);

    back_button_.setSize({140.f, 44.f});
    back_button_.setPosition({size.x - 160.f, size.y - 70.f});
    back_button_.setText("Back", Fonts::Main(), 20);
    back_button_.setOnClick([this]{ pending_switch_ = ScreenID::City; });

    rebuild_buttons();
}

void WorkshopScreen::rebuild_buttons() {
    option_buttons_.clear();
    visible_tool_ids_.clear();

    const auto size = ctx_.window->getSize();
    const float left = sidebar_.width() + 100.f;
    const float button_width = size.x - left - 140.f;
    const float button_x = left + (size.x - left) * 0.5f - button_width * 0.5f;
    float y = 160.f;

    std::vector<Tool> tools = game_state_.inventory.read_tools();
    for (const auto& t : tools) {
        if (t.read_cur_durability() >= 100) continue;
        Button b; b.setSize({button_width, 48}); b.setPosition({button_x, y});
        const std::string label = "Repair " + t.read_name() +
            " for " + std::to_string(t.read_repair_cost()) + "$ (" +
            std::to_string(t.read_cur_durability()) + "%)->(100%)";
        b.setText(label, Fonts::Main(), 18);
        const size_t visual_index = visible_tool_ids_.size();
        b.setOnClick([this, visual_index]{ on_repair_clicked(visual_index); });
        option_buttons_.push_back(b);
        visible_tool_ids_.push_back(t.read_id());
        y += 64.f;
    }

    if (option_buttons_.empty()) {
        Button b; b.setSize({button_width, 48}); b.setPosition({button_x, y});
        b.setText("No tools in need of repair", Fonts::Main(), 18);
        option_buttons_.push_back(b);
    }
}

void WorkshopScreen::on_repair_clicked(size_t visual_index) {
    if (visual_index >= visible_tool_ids_.size()) return;
    const int tool_id = visible_tool_ids_[visual_index];

    try {
        Tool& tool = game_state_.inventory.get_tool_by_id(tool_id);
        const int cost = tool.read_repair_cost();
        const int money = game_state_.player.read_money();
        if (cost > money) {
            const int need = cost - money;
            result_text_.setString("Not enough money for " + tool.read_name() +
                                   " repairing. You need an additional " + std::to_string(need) + "$");
            return;
        }
        if (tool.read_cur_durability() >= 100) {
            result_text_.setString("Already at 100%.");
            return;
        }
        game_state_.player.spend_money(cost);
        tool.repair();
        db_manager_.save_player_data(game_state_.player);
        db_manager_.save_tool_data(tool);
        result_text_.setString("You repaired the " + tool.read_name() + " for " + std::to_string(cost) +
                               "$, the current durability is 100%.");
        rebuild_buttons();
    } catch(...) {
        // ignore errors
    }
}

void WorkshopScreen::handleEvent(const sf::Event& e) {
    for (auto& b : option_buttons_) b.handleEvent(e, *ctx_.window);
    back_button_.handleEvent(e, *ctx_.window);
}

void WorkshopScreen::update(float) {
    if (pending_switch_ && ctx_.manager) {
        const ScreenID id = *pending_switch_;
        pending_switch_.reset();
        ctx_.manager->switchTo(id);
    }
}

void WorkshopScreen::draw(sf::RenderTarget& target) {
    sidebar_.draw(target);
    target.draw(title_text_);
    target.draw(result_text_);
    for (auto& b : option_buttons_) target.draw(b);
    target.draw(back_button_);
}
