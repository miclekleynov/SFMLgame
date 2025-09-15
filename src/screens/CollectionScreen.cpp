//
// Created by mishka on 15.09.2025.
//

#include "screens/CollectionScreen.h"
#include "game_logic/Artifact.h"
#include "game_logic/Collection.h"

CollectionScreen::CollectionScreen(AppContext context, GameState& state, DBManager& db_manager)
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
    title_text_.setCharacterSize(32); title_text_.setString("Collection");
    title_text_.setPosition(cx, 20.f); center_text_horiz(title_text_, cx);

    result_text_.setFont(Fonts::Main()); result_text_.setFillColor(sf::Color::White);
    result_text_.setCharacterSize(20); result_text_.setString("");
    result_text_.setPosition(cx, 80.f); center_text_horiz(result_text_, cx);

    back_button_.setSize({140.f, 44.f});
    back_button_.setPosition({size.x - 160.f, size.y - 70.f});
    back_button_.setText("Back", Fonts::Main(), 20);
    back_button_.setOnClick([this]{ pending_switch_ = ScreenID::Base; });

    rebuild_buttons();
}

void CollectionScreen::rebuild_buttons() {
    option_buttons_.clear();
    visible_artifact_ids_.clear();

    const auto size = ctx_.window->getSize();
    const float left = sidebar_.width() + 100.f;
    const float button_width = size.x - left - 140.f;
    const float button_x = left + (size.x - left) * 0.5f - button_width * 0.5f;
    float y = 160.f;

    std::vector<Artifact> arts = game_state_.collection.read_artifacts();
    for (const auto& a : arts) {
        Button b; b.setSize({button_width, 48}); b.setPosition({button_x, y});
        const std::string label =
            a.read_name() + " | " + a.read_description() +  " | " + " (sell for " + std::to_string(a.read_cost()) + "$)";
        b.setText(label, Fonts::Main(), 18);
        const size_t visual_index = visible_artifact_ids_.size();
        b.setOnClick([this, visual_index]{ on_sell_clicked(visual_index); });
        option_buttons_.push_back(b);
        visible_artifact_ids_.push_back(a.read_id());
        y += 64.f;
    }

    if (option_buttons_.empty()) {
        Button b; b.setSize({button_width, 48}); b.setPosition({button_x, y});
        b.setText("No artifacts in collection", Fonts::Main(), 18);
        option_buttons_.push_back(b);
    }
}

void CollectionScreen::on_sell_clicked(size_t visual_index) {
    if (visual_index >= visible_artifact_ids_.size()) return;
    const int art_id = visible_artifact_ids_[visual_index];

    try {
        // Найдём текущий артефакт по id (список мог измениться после rebuild)
        std::vector<Artifact> arts = game_state_.collection.read_artifacts();
        const Artifact* ptr = nullptr;
        for (const auto& a : arts) if (a.read_id() == art_id) { ptr = &a; break; }
        if (!ptr) return;

        const int price = ptr->read_cost();
        game_state_.player.earn_money(price);
        game_state_.collection.remove_artifact_by_id(art_id);

        // Сохраняем: деньги и коллекцию
        db_manager_.save_player_data(game_state_.player);
        db_manager_.save_collection_data(game_state_.collection); // метод должен быть в DBManager

        result_text_.setString("You sold \"" + ptr->read_name() + "\" for " + std::to_string(price) + "$");
        rebuild_buttons();
    } catch(...) {
        // fail silent
    }
}

void CollectionScreen::handleEvent(const sf::Event& e) {
    for (auto& b : option_buttons_) b.handleEvent(e, *ctx_.window);
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
    for (auto& b : option_buttons_) target.draw(b);
    target.draw(back_button_);
}
