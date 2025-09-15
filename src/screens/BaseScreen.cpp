//
// Created by mishka on 15.09.2025.
//

#include "screens/BaseScreen.h"

BaseScreen::BaseScreen(AppContext context, GameState& state, DBManager& db_manager)
    : Screen(context), game_state_(state), db_manager_(db_manager) {
    const auto size = context.window->getSize();
    const float cx = size.x * 0.5f;

    // Sidebar provider
    sidebar_.set_position({0.f, 0.f});
    sidebar_.set_provider([this]{
        BaseStats st{};
        st.money = game_state_.player.read_money();
        st.satiety = game_state_.player.read_stamina();
        int px=0, sh=0, br=0;
        for (const auto& t : game_state_.inventory.read_tools()) {
            std::string n = t.read_name();
            for (auto& c : n) c = (char)std::tolower(c);
            if (n.find("pickaxe") != std::string::npos)      px = t.read_cur_durability();
            else if (n.find("shovel") != std::string::npos)  sh = t.read_cur_durability();
            else if (n.find("brush")  != std::string::npos)  br = t.read_cur_durability();
        }
        st.pickaxe = px; st.shovel = sh; st.brush = br; return st;
    });

    // Title
    title_text_.setFont(Fonts::Main());
    title_text_.setFillColor(sf::Color::White);
    title_text_.setCharacterSize(32);
    title_text_.setString("Base");
    title_text_.setPosition(cx, 20.f);
    center_text_horiz(title_text_, cx);

    // Buttons layout
    const float left = sidebar_.width() + 100.f;
    const float button_width = size.x - left - 140.f;
    const float button_x = left + (size.x - left) * 0.5f - button_width * 0.5f;
    float y = 150.f;

    excavation_button_.setSize({button_width, 48});
    excavation_button_.setPosition({button_x, y});
    excavation_button_.setText("Excavation", Fonts::Main(), 20);
    excavation_button_.setOnClick([this]{ pending_switch_ = ScreenID::Excavation; });
    y += 64.f;

    city_button_.setSize({button_width, 48});
    city_button_.setPosition({button_x, y});
    city_button_.setText("City", Fonts::Main(), 20);
    city_button_.setOnClick([this]{ pending_switch_ = ScreenID::City; });
    y += 64.f;

    collection_button_.setSize({button_width, 48});
    collection_button_.setPosition({button_x, y});
    collection_button_.setText("Collection", Fonts::Main(), 20);
    collection_button_.setOnClick([this]{ pending_switch_ = ScreenID::Collection; });

    back_to_menu_button_.setSize({180.f, 44.f});
    back_to_menu_button_.setPosition({size.x - 200.f, size.y - 70.f});
    back_to_menu_button_.setText("Main menu", Fonts::Main(), 20);
    back_to_menu_button_.setOnClick([this]{ pending_switch_ = ScreenID::MainMenu; });
}

void BaseScreen::handleEvent(const sf::Event& e) {
    excavation_button_.handleEvent(e, *ctx_.window);
    city_button_.handleEvent(e, *ctx_.window);
    collection_button_.handleEvent(e, *ctx_.window);
    back_to_menu_button_.handleEvent(e, *ctx_.window);
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
    target.draw(back_to_menu_button_);
}