// widgets_demo/navigation_rail/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/navigation_rail.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class NavigationRailDemoState : public State {
    int selected_idx = 0;
    bool extended = false;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionRow)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        // Header for Rail
        auto header = std::make_shared<FlexBox>();
        header->height(80).justifyContent(YGJustifyCenter).alignItems(YGAlignCenter);
        header->child(Icon(Icons::Star, {.size = 32.0f, .color = 0xFF38BDF8}));

        // Navigation Rail
        NavigationRailConfig rail_cfg;
        rail_cfg.extended = extended;
        rail_cfg.selected_index = selected_idx;
        rail_cfg.header = header;
        
        rail_cfg.items.push_back({Icon(Icons::House, {.color = 0xFFFFFFFF}), "Home"});
        rail_cfg.items.push_back({Icon(Icons::Person, {.color = 0xFFFFFFFF}), "Profile"});
        rail_cfg.items.push_back({Icon(Icons::Gear, {.color = 0xFFFFFFFF}), "Settings"});
        
        rail_cfg.on_changed = [this](int idx) {
            setState([this, idx]() { selected_idx = idx; });
        };
        
        root->child(NavigationRail(rail_cfg));

        // Content Area
        auto content = std::make_shared<FlexBox>();
        content->flexGrow(1.0f)
               .flexDirection(YGFlexDirectionColumn)
               .justifyContent(YGJustifyCenter)
               .alignItems(YGAlignCenter)
               .backgroundColor(0xFF1E293B);

        std::string page_names[] = {"Home Page", "Profile Page", "Settings Page"};
        content->child(Text(page_names[selected_idx], {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        ButtonConfig toggle_cfg;
        toggle_cfg.child = Text(extended ? "Collapse Rail" : "Extend Rail", {.font_size = 14.0f, .color = 0xFFFFFFFF});
        toggle_cfg.color = 0xFF3B82F6;
        toggle_cfg.on_pressed = [this]() {
            setState([this]() { extended = !extended; });
        };
        
        auto btn_wrap = std::make_shared<FlexBox>();
        btn_wrap->margin(YGEdgeTop, 20).child(Button(toggle_cfg));
        content->child(btn_wrap);

        root->child(content);
        return root;
    }
};

class NavigationRailDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "NavigationRailDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<NavigationRailDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Navigation Rail Demo";
    config.width = 900;
    config.height = 600;
    return runApp(std::make_shared<NavigationRailDemo>(), config);
}
