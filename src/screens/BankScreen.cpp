//
// Created by mishka on 15.09.2025.
//

#include "screens/BankScreen.h"
#include "ui/Layout.h"

BankScreen::BankScreen(AppContext context, GameState& state, DBManager& db_manager)
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
    title_text_.setCharacterSize(28); title_text_.setString("Bank");
    Layout::centerTextX(title_text_, *ctx_.window, 20.f);

    result_text_.setFont(Fonts::Main()); result_text_.setFillColor(sf::Color::White);
    result_text_.setCharacterSize(20); result_text_.setString("");
    Layout::centerTextX(result_text_, *ctx_.window, 80.f);

    // Простые кредиты (можно заменить логикой из GDD)
    const int loans[] = {50, 100, 200};
    float y = Layout::belowTextY(result_text_, 40.f);
    for (int i=0; i<3; ++i) {
        Button b; b.setSize({360.f, 56.f});
        Layout::centerButtonX(b, *ctx_.window, y);
        b.setText("Take loan: +$" + std::to_string(loans[i]), Fonts::Main(), 20);
        b.setOnClick([this, amount=loans[i]]{
            game_state_.player.earn_money(amount);
            db_manager_.save_player_data(game_state_.player);
            result_text_.setString("Loan taken: +$" + std::to_string(amount));
            Layout::centerTextX(result_text_, *ctx_.window, 80.f);
        });
        option_buttons_.push_back(b);
        y += 72.f;
    }

    back_button_.setSize({240.f, 48.f});
    back_button_.setText("Back to City", Fonts::Main(), 20);
    Layout::centerButtonX(back_button_, *ctx_.window, static_cast<float>(ctx_.window->getSize().y) - 70.f);
    back_button_.setOnClick([this]{ pending_switch_ = ScreenID::City; });
}

void BankScreen::handleEvent(const sf::Event& e) {
    for (auto& b : option_buttons_) b.handleEvent(e, *ctx_.window);
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
    for (auto& b : option_buttons_) target.draw(b);
    target.draw(back_button_);
}