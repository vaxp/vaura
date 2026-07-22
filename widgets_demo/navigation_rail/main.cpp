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
        // Navigation Rail
        NavigationRailConfig rail_cfg;
        rail_cfg.extended = extended;
        rail_cfg.selected_index = selected_idx;
        rail_cfg.header = Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .height = 80.0f,
            .children = {
                Icon(Icons::Star, {.size = 32.0f, .color = 0xFF38BDF8})
            }
        });
        
        rail_cfg.items.push_back({Icon(Icons::House, {.color = 0xFFFFFFFF}), "Home"});
        rail_cfg.items.push_back({Icon(Icons::Person, {.color = 0xFFFFFFFF}), "Profile"});
        rail_cfg.items.push_back({Icon(Icons::Gear, {.color = 0xFFFFFFFF}), "Settings"});
        
        rail_cfg.on_changed = [this](int idx) {
            setState([this, idx]() { selected_idx = idx; });
        };
        
        std::string page_names[] = {"Home Page", "Profile Page", "Settings Page"};

        return Row({
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                NavigationRail(rail_cfg),
                
                // Content Area
                Column({
                    .justifyContent = YGJustifyCenter,
                    .alignItems = YGAlignCenter,
                    .flexGrow = 1.0f,
                    .backgroundColor = 0xFF1E293B,
                    .children = {
                        Text(page_names[selected_idx], {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                        Column({
                            .margin = std::pair{YGEdgeTop, 20.0f},
                            .children = {
                                Button({
                                    .child = Text(extended ? "Collapse Rail" : "Extend Rail", {.font_size = 14.0f, .color = 0xFFFFFFFF}),
                                    .on_pressed = [this]() {
                                        setState([this]() { extended = !extended; });
                                    },
                                    .color = 0xFF3B82F6
                                })
                            }
                        })
                    }
                })
            }
        });
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
