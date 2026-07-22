// widgets_demo/flexbox/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class FlexBoxDemoState : public State {
    YGFlexDirection current_dir = YGFlexDirectionRow;
    YGJustify current_justify = YGJustifySpaceEvenly;
    YGAlign current_align = YGAlignCenter;
    
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .backgroundColor(0xFF1E293B) // Slate 800
            .widthPercent(100).heightPercent(100);

        // Control Panel
        auto controls = std::make_shared<FlexBox>();
        controls->flexDirection(YGFlexDirectionRow)
                .justifyContent(YGJustifyCenter)
                .alignItems(YGAlignCenter)
                .padding(YGEdgeAll, 20.0f)
                .backgroundColor(0xFF0F172A)
                .widthPercent(100);

        ButtonConfig toggle_dir_btn;
        toggle_dir_btn.child = Text("Toggle Direction", {.font_size = 14.0f, .color = 0xFFFFFFFF});
        toggle_dir_btn.color = 0xFF3B82F6;
        toggle_dir_btn.on_pressed = [this]() {
            setState([this]() {
                current_dir = (current_dir == YGFlexDirectionRow) ? YGFlexDirectionColumn : YGFlexDirectionRow;
            });
        };
        controls->child(Button(toggle_dir_btn));
        
        root->child(controls);

        // Demo Area
        auto demo_area = std::make_shared<FlexBox>();
        demo_area->flexGrow(1.0f)
                 .flexDirection(current_dir)
                 .justifyContent(current_justify)
                 .alignItems(current_align)
                 .widthPercent(100);

        // Add 5 colored boxes
        std::vector<Color> colors = {0xFFEF4444, 0xFFF59E0B, 0xFF10B981, 0xFF3B82F6, 0xFF8B5CF6};
        for (int i = 0; i < 5; ++i) {
            auto box = std::make_shared<FlexBox>();
            box->width(80).height(80).margin(YGEdgeAll, 10).backgroundColor(colors[i]).borderRadius(12.0f);
            
            auto t = Text(std::to_string(i + 1), {.font_size = 24.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold});
            auto inner = std::make_shared<FlexBox>();
            inner->widthPercent(100).heightPercent(100).justifyContent(YGJustifyCenter).alignItems(YGAlignCenter).child(t);
            
            box->child(inner);
            demo_area->child(box);
        }

        root->child(demo_area);
        return root;
    }
};

class FlexBoxDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "FlexBoxDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<FlexBoxDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA FlexBox Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<FlexBoxDemo>(), config);
}
