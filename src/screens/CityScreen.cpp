//
// Created by mishka on 15.09.2025.
//

#include "screens/CityScreen.h"

CityScreen::CityScreen(AppContext context, GameState& state, DBManager& db_manager)
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
    title_text_.setCharacterSize(32); title_text_.setString("City");
    title_text_.setPosition(cx, 20.f); center_text_horiz(title_text_, cx);

    result_text_.setFont(Fonts::Main()); result_text_.setFillColor(sf::Color::White);
    result_text_.setCharacterSize(20); result_text_.setString("");
    result_text_.setPosition(cx, 80.f); center_text_horiz(result_text_, cx);

    const float left = sidebar_.width() + 100.f;
    const float button_width = size.x - left - 140.f;
    const float button_x = left + (size.x - left) * 0.5f - button_width * 0.5f;
    float y = 150.f;

    food_button_.setSize({button_width, 48});
    food_button_.setPosition({button_x, y});
    food_button_.setText("Food store", Fonts::Main(), 20);
    food_button_.setOnClick([this]{ pending_switch_ = ScreenID::FoodShop; }); y += 64.f;

    workshop_button_.setSize({button_width, 48});
    workshop_button_.setPosition({button_x, y});
    workshop_button_.setText("Workshop", Fonts::Main(), 20);
    workshop_button_.setOnClick([this]{ pending_switch_ = ScreenID::Workshop; }); y += 64.f;

    bank_button_.setSize({button_width, 48});
    bank_button_.setPosition({button_x, y});
    bank_button_.setText("Bank", Fonts::Main(), 20);
    bank_button_.setOnClick([this]{ pending_switch_ = ScreenID::Bank; });

    back_button_.setSize({140.f, 44.f});
    back_button_.setPosition({size.x - 160.f, size.y - 70.f});
    back_button_.setText("Back", Fonts::Main(), 20);
    back_button_.setOnClick([this]{ pending_switch_ = ScreenID::Base; });
}

void CityScreen::handleEvent(const sf::Event& e) {
    food_button_.handleEvent(e, *ctx_.window);
    workshop_button_.handleEvent(e, *ctx_.window);
    bank_button_.handleEvent(e, *ctx_.window);
    back_button_.handleEvent(e, *ctx_.window);
}

void CityScreen::update(float) {
    if (pending_switch_ && ctx_.manager) {
        const ScreenID id = *pending_switch_;
        pending_switch_.reset();
        ctx_.manager->switchTo(id);
    }
}

void CityScreen::draw(sf::RenderTarget& target) {
    sidebar_.draw(target);
    target.draw(title_text_);
    target.draw(result_text_);
    target.draw(food_button_);
    target.draw(workshop_button_);
    target.draw(bank_button_);
    target.draw(back_button_);
}
