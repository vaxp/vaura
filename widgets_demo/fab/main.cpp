// widgets_demo/fab/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/fab.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class FABDemoState : public State {
    int counter = 0;
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A) // Slate 900
            .widthPercent(100).heightPercent(100);

        root->child(Text("Counter: " + std::to_string(counter), {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        // Stack-like absolute positioning for the FAB
        FABConfig fab_cfg;
        fab_cfg.icon = Icon(Icons::Plus, {.color = 0xFFFFFFFF});
        fab_cfg.background_color = 0xFF6366F1; // Indigo 500
        fab_cfg.on_pressed = [this]() {
            setState([this]() { counter++; });
        };
        
        auto fab_widget = FAB(fab_cfg);
        
        auto fab_wrap = std::make_shared<FlexBox>();
        fab_wrap->positionType(YGPositionTypeAbsolute)
                .position(YGEdgeBottom, 40.0f)
                .position(YGEdgeRight, 40.0f)
                .child(fab_widget);

        // Extended FAB
        FABConfig ext_fab_cfg;
        ext_fab_cfg.icon = Icon(Icons::Pencil, {.color = 0xFFFFFFFF});
        ext_fab_cfg.label = "Reset";
        ext_fab_cfg.background_color = 0xFFEF4444; // Red 500
        ext_fab_cfg.on_pressed = [this]() {
            setState([this]() { counter = 0; });
        };
        
        auto ext_fab_wrap = std::make_shared<FlexBox>();
        ext_fab_wrap->positionType(YGPositionTypeAbsolute)
                    .position(YGEdgeBottom, 40.0f)
                    .position(YGEdgeLeft, 40.0f)
                    .child(FAB(ext_fab_cfg));

        root->child(fab_wrap);
        root->child(ext_fab_wrap);

        return root;
    }
};

class FABDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "FABDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<FABDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA FAB Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<FABDemo>(), config);
}
