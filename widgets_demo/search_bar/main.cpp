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
        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Search Bar Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Column({.height = 40.0f}),
                SearchBar({
                    .on_changed = [this](const std::string& query) {
                        setState([this, query]() { current_query = query; });
                    },
                    .on_submitted = [](const std::string& query) {
                        std::cout << "Search submitted: " << query << std::endl;
                    },
                    .placeholder = "Search anything...",
                    .background_color = 0xFF1E293B,
                    .text_color = 0xFFFFFFFF,
                    .icon_color = 0xFF94A3B8,
                    .border_radius = 24.0f, // Rounded search bar
                    .width = 400.0f,
                    .height = 48.0f
                }),
                Column({.height = 40.0f}),
                Text("Results for: " + (current_query.empty() ? "None" : current_query), 
                    {.font_size = 18.0f, .color = 0xFF94A3B8})
            }
        });
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
