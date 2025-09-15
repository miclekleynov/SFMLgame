//
// Created by mishka on 15.09.2025.
//

#include "screens/FoodShopScreen.h"
#include "game_logic/Food.h"
#include "game_logic/FoodShop.h"

FoodShopScreen::FoodShopScreen(AppContext context, GameState& state, DBManager& db_manager)
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
    title_text_.setCharacterSize(32); title_text_.setString("Food Shop");
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

void FoodShopScreen::rebuild_buttons() {
    option_buttons_.clear();

    const auto size = ctx_.window->getSize();
    const float left = sidebar_.width() + 100.f;
    const float button_width = size.x - left - 140.f;
    const float button_x = left + (size.x - left) * 0.5f - button_width * 0.5f;
    float y = 160.f;

    auto shop = db_manager_.get_food_shop();
    std::vector<Food> foods = shop.read_food_assortment();
    for (size_t i=0; i<foods.size(); ++i) {
        const auto& f = foods[i];
        Button b; b.setSize({button_width, 48}); b.setPosition({button_x, y});
        const std::string label = f.read_name() + " (" + std::to_string(f.read_cost()) + "$): " +
                                  f.read_description() + " (+" + std::to_string(f.read_stamina()) + "% satiety)";
        b.setText(label, Fonts::Main(), 18);
        b.setOnClick([this, i]{ on_buy_clicked(i); });
        option_buttons_.push_back(b);
        y += 64.f;
    }
}

void FoodShopScreen::on_buy_clicked(size_t idx) {
    auto shop = db_manager_.get_food_shop();
    std::vector<Food> foods = shop.read_food_assortment();
    if (idx >= foods.size()) return;

    const Food& f = foods[idx];
    const int cost = f.read_cost();
    const int money = game_state_.player.read_money();
    if (cost > money) {
        const int need = cost - money;
        result_text_.setString("Not enough money for " + f.read_name() + ". You need an additional " + std::to_string(need) + "$");
        return;
    }
    if ((100 - game_state_.player.read_stamina()) < f.read_stamina()) {
        result_text_.setString("You are too full to eat this food");
        return;
    }
    game_state_.player.spend_money(cost);
    game_state_.player.eat(f);
    db_manager_.save_player_data(game_state_.player);
    result_text_.setString("You purchased " + f.read_name() + " for " + std::to_string(cost) + "$. Current satiety " +
                           std::to_string(game_state_.player.read_stamina()) + "%.");
}

void FoodShopScreen::handleEvent(const sf::Event& e) {
    for (auto& b : option_buttons_) b.handleEvent(e, *ctx_.window);
    back_button_.handleEvent(e, *ctx_.window);
}

void FoodShopScreen::update(float) {
    if (pending_switch_ && ctx_.manager) {
        const ScreenID id = *pending_switch_;
        pending_switch_.reset();
        ctx_.manager->switchTo(id);
    }
}

void FoodShopScreen::draw(sf::RenderTarget& target) {
    sidebar_.draw(target);
    target.draw(title_text_);
    target.draw(result_text_);
    for (auto& b : option_buttons_) target.draw(b);
    target.draw(back_button_);
}
