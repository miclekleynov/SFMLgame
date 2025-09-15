//
// Created by mishka on 15.09.2025.
//

#include "screens/BankScreen.h"

BankScreen::BankScreen(AppContext context, GameState& state, DBManager& db_manager)
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
    title_text_.setCharacterSize(32); title_text_.setString("Bank");
    title_text_.setPosition(cx, 20.f); center_text_horiz(title_text_, cx);

    result_text_.setFont(Fonts::Main()); result_text_.setFillColor(sf::Color::White);
    result_text_.setCharacterSize(20); result_text_.setString("");
    result_text_.setPosition(cx, 80.f); center_text_horiz(result_text_, cx);

    const float left = sidebar_.width() + 100.f; const float button_width = size.x - left - 140.f;
    const float button_x = left + (size.x - left) * 0.5f - button_width * 0.5f; float y = 160.f;

    loan100_button_.setSize({button_width, 48});
    loan100_button_.setPosition({button_x, y});
    loan100_button_.setText("Take a microloan (100$)", Fonts::Main(), 18);
    loan100_button_.setOnClick([this]{ take_loan(100, "A microloan of $100 has been approved for you"); }); y += 64.f;

    loan1000_button_.setSize({button_width, 48});
    loan1000_button_.setPosition({button_x, y});
    loan1000_button_.setText("Take a loan (1000$)", Fonts::Main(), 18);
    loan1000_button_.setOnClick([this]{ take_loan(1000, "A loan of $1000 has been approved for you"); });

    back_button_.setSize({140.f, 44.f});
    back_button_.setPosition({size.x - 160.f, size.y - 70.f});
    back_button_.setText("Back", Fonts::Main(), 20);
    back_button_.setOnClick([this]{ pending_switch_ = ScreenID::City; });
}

void BankScreen::take_loan(int amount, const char* phrase) {
    game_state_.player.earn_money(amount);
    db_manager_.save_player_data(game_state_.player);
    result_text_.setString(phrase);
}

void BankScreen::handleEvent(const sf::Event& e) {
    loan100_button_.handleEvent(e, *ctx_.window);
    loan1000_button_.handleEvent(e, *ctx_.window);
    back_button_.handleEvent(e, *ctx_.window);
}

void BankScreen::update(float) {
    if (pending_switch_ && ctx_.manager) {
        const ScreenID id = *pending_switch_;
        pending_switch_.reset();
        ctx_.manager->switchTo(id);
    }
}

void BankScreen::draw(sf::RenderTarget& target) {
    sidebar_.draw(target);
    target.draw(title_text_);
    target.draw(result_text_);
    target.draw(loan100_button_);
    target.draw(loan1000_button_);
    target.draw(back_button_);
}