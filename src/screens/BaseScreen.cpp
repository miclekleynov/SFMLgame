//
// Created by mishka on 15.09.2025.
//

#include "screens/BaseScreen.h"
#include "game_logic/Tool.h"
#include "ui/Layout.h"

BaseScreen::BaseScreen(AppContext context, GameState& state, DBManager& db_manager)
    : Screen(context), game_state_(state), db_manager_(db_manager) {
    // Sidebar provider
    sidebar_.set_position({0.f, 0.f});
    sidebar_.set_provider([this]{
        BaseStats st{}; st.money = game_state_.player.read_money(); st.satiety = game_state_.player.read_stamina();
        int px=0,sh=0,br=0; for (const auto& t : game_state_.inventory.read_tools()){
            std::string n=t.read_name(); for(auto& c:n) c=(char)std::tolower(c);
            if(n.find("pickaxe")!=std::string::npos) px=t.read_cur_durability();
            else if(n.find("shovel")!=std::string::npos) sh=t.read_cur_durability();
            else if(n.find("brush")!=std::string::npos) br=t.read_cur_durability();
        } st.pickaxe=px; st.shovel=sh; st.brush=br; return st; });

    setup_ui();
}

void BaseScreen::setup_ui() {
    title_text_.setFont(Fonts::Main()); title_text_.setFillColor(sf::Color::White);
    title_text_.setCharacterSize(28); title_text_.setString("Base");
    Layout::centerTextX(title_text_, *ctx_.window, 20.f);

    float y = Layout::belowTextY(title_text_, 40.f);

    excavation_button_.setSize({360.f, 56.f});
    excavation_button_.setText("Go to Excavation", Fonts::Main(), 22);
    Layout::centerButtonX(excavation_button_, *ctx_.window, y);
    excavation_button_.setOnClick([this]{ pending_switch_ = ScreenID::Excavation; });
    y += 72.f;

    city_button_.setSize({360.f, 56.f});
    city_button_.setText("Go to City", Fonts::Main(), 22);
    Layout::centerButtonX(city_button_, *ctx_.window, y);
    city_button_.setOnClick([this]{ pending_switch_ = ScreenID::City; });
    y += 72.f;

    collection_button_.setSize({360.f, 56.f});
    collection_button_.setText("Collection", Fonts::Main(), 22);
    Layout::centerButtonX(collection_button_, *ctx_.window, y);
    collection_button_.setOnClick([this]{ pending_switch_ = ScreenID::Collection; });
    y += 72.f;

    back_button_.setSize({240.f, 48.f});
    back_button_.setText("Back to Main Menu", Fonts::Main(), 20);
    Layout::centerButtonX(back_button_, *ctx_.window, static_cast<float>(ctx_.window->getSize().y) - 70.f);
    back_button_.setOnClick([this]{ pending_switch_ = ScreenID::MainMenu; });
}

void BaseScreen::handleEvent(const sf::Event& e) {
    //sidebar_.handle_event(e, *ctx_.window); // если у тебя есть такая функция — иначе можно убрать
    excavation_button_.handleEvent(e, *ctx_.window);
    city_button_.handleEvent(e, *ctx_.window);
    collection_button_.handleEvent(e, *ctx_.window);
    back_button_.handleEvent(e, *ctx_.window);
}

void BaseScreen::update(float) {
    if (pending_switch_ && ctx_.manager) {
        const ScreenID id = *pending_switch_;
        pending_switch_.reset();
        ctx_.manager->switchTo(id);
    }
}

void BaseScreen::draw(sf::RenderTarget& target) {
    sidebar_.draw(target);
    target.draw(title_text_);
    target.draw(excavation_button_);
    target.draw(city_button_);
    target.draw(collection_button_);
    target.draw(back_button_);
}