//
// Created by mishka on 15.09.2025.
//

#include "screens/WorkshopScreen.h"
#include "ui/Layout.h"
#include "game_logic/Tool.h"

WorkshopScreen::WorkshopScreen(AppContext context, GameState& state, DBManager& db_manager)
    : Screen(context), game_state_(state), db_manager_(db_manager) {
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
    title_text_.setCharacterSize(28); title_text_.setString("Workshop");
    Layout::centerTextX(title_text_, *ctx_.window, 20.f);

    result_text_.setFont(Fonts::Main()); result_text_.setFillColor(sf::Color::White);
    result_text_.setCharacterSize(20); result_text_.setString("");
    Layout::centerTextX(result_text_, *ctx_.window, 80.f);

    back_button_.setSize({240.f, 48.f});
    back_button_.setText("Back to City", Fonts::Main(), 20);
    Layout::centerButtonX(back_button_, *ctx_.window, static_cast<float>(ctx_.window->getSize().y) - 70.f);
    back_button_.setOnClick([this]{ pending_switch_ = ScreenID::City; });

    rebuild_buttons();
}

void WorkshopScreen::rebuild_buttons() {
    option_buttons_.clear();

    float y = Layout::belowTextY(result_text_, 40.f);
    const float button_width = std::max(440.f, static_cast<float>(ctx_.window->getSize().x) * 0.55f);

    auto tools = game_state_.inventory.read_tools();
    for (size_t i=0; i<tools.size(); ++i) {
        const Tool& t = tools[i];
        if (t.read_cur_durability() >= 100) continue; // показываем только те, что нуждаются в ремонте
        Button b; b.setSize({button_width, 48});
        Layout::centerButtonX(b, *ctx_.window, y);
        const std::string label = "Repair " + t.read_name() + " (" + std::to_string(t.read_repair_cost()) + "$) (" + std::to_string(t.read_cur_durability()) + "% -> 100%)";
        b.setText(label, Fonts::Main(), 18);
        b.setOnClick([this, i]{ repair_click(i); });
        option_buttons_.push_back(b);
        y += 64.f;
    }

    if (option_buttons_.empty()) {
        result_text_.setString("All tools are at 100%.");
        Layout::centerTextX(result_text_, *ctx_.window, 80.f);
    }
}

void WorkshopScreen::repair_click(size_t idx) {
    auto tools = game_state_.inventory.read_tools();
    if (idx >= tools.size()) return;
    Tool& t = game_state_.inventory.get_tool_by_id(tools[idx].read_id());

    const int cost = t.read_repair_cost();
    const int money = game_state_.player.read_money();
    if (money < cost) {
        result_text_.setString("Not enough money to repair " + t.read_name() + ". Need +" + std::to_string(cost - money) + "$.");
        Layout::centerTextX(result_text_, *ctx_.window, 80.f);
        return;
    }

    // списываем деньги и чиним
    game_state_.player.spend_money(cost);
    t.repair();
    db_manager_.save_player_data(game_state_.player);
    db_manager_.save_tool_data(t);

    result_text_.setString("Repaired " + t.read_name() + " to 100% for " + std::to_string(cost) + "$.");
    Layout::centerTextX(result_text_, *ctx_.window, 80.f);

    rebuild_buttons();
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
