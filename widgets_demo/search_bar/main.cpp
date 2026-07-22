// widgets_demo/search_bar/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/search_bar.hpp"
#include "vaura/state/state.hpp"
#include <iostream>

using namespace vaura;

class SearchBarDemoState : public State {
    std::string current_query = "";
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(Text("Search Bar Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(40);
        root->child(spacer);

        SearchBarConfig search_cfg;
        search_cfg.placeholder = "Search anything...";
        search_cfg.background_color = 0xFF1E293B;
        search_cfg.text_color = 0xFFFFFFFF;
        search_cfg.icon_color = 0xFF94A3B8;
        search_cfg.border_radius = 24.0f; // Rounded search bar
        search_cfg.width = 400.0f;
        search_cfg.height = 48.0f;
        search_cfg.on_changed = [this](const std::string& query) {
            setState([this, query]() { current_query = query; });
        };
        search_cfg.on_submitted = [](const std::string& query) {
            std::cout << "Search submitted: " << query << std::endl;
        };

        root->child(SearchBar(search_cfg));
        
        auto spacer2 = std::make_shared<FlexBox>();
        spacer2->height(40);
        root->child(spacer2);

        root->child(Text("Results for: " + (current_query.empty() ? "None" : current_query), 
            {.font_size = 18.0f, .color = 0xFF94A3B8}));

        return root;
    }
};

class SearchBarDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "SearchBarDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<SearchBarDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Search Bar Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<SearchBarDemo>(), config);
}
