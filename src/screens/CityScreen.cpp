//
// Created by mishka on 15.09.2025.
//

#include "screens/CityScreen.h"
#include "game_logic/Tool.h"
#include "ui/Layout.h"

CityScreen::CityScreen(AppContext context, GameState& state, DBManager& db_manager)
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

    // Title
    title_text_.setFont(Fonts::Main()); title_text_.setFillColor(sf::Color::White);
    title_text_.setCharacterSize(28); title_text_.setString("City");
    Layout::centerTextX(title_text_, *ctx_.window, 20.f);

    float y = Layout::belowTextY(title_text_, 40.f);

    foodshop_button_.setSize({360.f, 56.f});
    foodshop_button_.setText("Food Shop", Fonts::Main(), 22);
    Layout::centerButtonX(foodshop_button_, *ctx_.window, y);
    foodshop_button_.setOnClick([this]{ pending_switch_ = ScreenID::FoodShop; });
    y += 72.f;

    workshop_button_.setSize({360.f, 56.f});
    workshop_button_.setText("Workshop", Fonts::Main(), 22);
    Layout::centerButtonX(workshop_button_, *ctx_.window, y);
    workshop_button_.setOnClick([this]{ pending_switch_ = ScreenID::Workshop; });
    y += 72.f;

    bank_button_.setSize({360.f, 56.f});
    bank_button_.setText("Bank", Fonts::Main(), 22);
    Layout::centerButtonX(bank_button_, *ctx_.window, y);
    bank_button_.setOnClick([this]{ pending_switch_ = ScreenID::Bank; });
    y += 72.f;

    back_button_.setSize({240.f, 48.f});
    back_button_.setText("Back to Base", Fonts::Main(), 20);
    Layout::centerButtonX(back_button_, *ctx_.window, static_cast<float>(ctx_.window->getSize().y) - 70.f);
    back_button_.setOnClick([this]{ pending_switch_ = ScreenID::Base; });
}

void CityScreen::handleEvent(const sf::Event& e) {
    //sidebar_.handle_event(e, *ctx_.window);
    foodshop_button_.handleEvent(e, *ctx_.window);
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
    target.draw(foodshop_button_);
    target.draw(workshop_button_);
    target.draw(bank_button_);
    target.draw(back_button_);
}
