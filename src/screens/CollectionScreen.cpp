//
// Created by mishka on 15.09.2025.
//

#include "screens/CollectionScreen.h"
#include "ui/Layout.h"
#include "game_logic/Artifact.h"
#include "game_logic/Collection.h"

CollectionScreen::CollectionScreen(AppContext context, GameState& state, DBManager& db_manager)
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
    title_text_.setCharacterSize(28); title_text_.setString("Collection");
    Layout::centerTextX(title_text_, *ctx_.window, 20.f);

    result_text_.setFont(Fonts::Main()); result_text_.setFillColor(sf::Color::White);
    result_text_.setCharacterSize(20); result_text_.setString("");
    Layout::centerTextX(result_text_, *ctx_.window, 80.f);

    back_button_.setSize({240.f, 48.f});
    back_button_.setText("Back to Base", Fonts::Main(), 20);
    Layout::centerButtonX(back_button_, *ctx_.window, static_cast<float>(ctx_.window->getSize().y) - 70.f);
    back_button_.setOnClick([this]{ pending_switch_ = ScreenID::Base; });

    rebuild_buttons();
}

void CollectionScreen::rebuild_buttons() {
    artifact_buttons_.clear();

    float y = Layout::belowTextY(result_text_, 40.f);
    const float button_width = std::max(500.f, static_cast<float>(ctx_.window->getSize().x) * 0.6f);

    auto arts = game_state_.collection.read_artifacts();
    for (size_t i=0; i<arts.size(); ++i) {
        const Artifact& a = arts[i];
        Button b; b.setSize({button_width, 48});
        Layout::centerButtonX(b, *ctx_.window, y);
        const std::string label = a.read_name() + " | " + a.read_description() + " (" + std::to_string(a.read_cost()) + "$)";
        b.setText(label, Fonts::Main(), 18);
        b.setOnClick([this, i]{ sell_click(i); });
        artifact_buttons_.push_back(b);
        y += 56.f;
    }

    if (artifact_buttons_.empty()) {
        result_text_.setString("No artifacts in collection.");
        Layout::centerTextX(result_text_, *ctx_.window, 80.f);
    }
}

void CollectionScreen::sell_click(size_t idx) {
    auto arts = game_state_.collection.read_artifacts();
    if (idx >= arts.size()) return;
    const Artifact& a = arts[idx];

    game_state_.player.earn_money(a.read_cost());
    game_state_.collection.remove_artifact_by_id(a.read_id());
    db_manager_.save_player_data(game_state_.player);
    db_manager_.save_collection_data(game_state_.collection);

    result_text_.setString("Sold \"" + a.read_name() + "\" for $" + std::to_string(a.read_cost()) + ".");
    Layout::centerTextX(result_text_, *ctx_.window, 80.f);

    rebuild_buttons();
}

void CollectionScreen::handleEvent(const sf::Event& e) {
    for (auto& b : artifact_buttons_) b.handleEvent(e, *ctx_.window);
    back_button_.handleEvent(e, *ctx_.window);
}

void CollectionScreen::update(float) {
    if (pending_switch_ && ctx_.manager) {
        const ScreenID id = *pending_switch_;
        pending_switch_.reset();
        ctx_.manager->switchTo(id);
    }
}

void CollectionScreen::draw(sf::RenderTarget& target) {
    sidebar_.draw(target);
    target.draw(title_text_);
    target.draw(result_text_);
    for (auto& b : artifact_buttons_) target.draw(b);
    target.draw(back_button_);
}
