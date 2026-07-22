// widgets_demo/gesture_detector/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"
#include <string>

using namespace vaura;

class GestureDetectorDemoState : public State {
    std::string status = "Waiting for gesture...";
    Color box_color = 0xFF334155; // Slate 700
    bool is_hovered = false;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        auto title = std::make_shared<FlexBox>();
        title->margin(YGEdgeBottom, 40)
             .child(Text("Gesture Detector", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));
        root->child(title);

        auto inner_box = std::make_shared<FlexBox>();
        inner_box->width(300).height(300)
                 .justifyContent(YGJustifyCenter)
                 .alignItems(YGAlignCenter)
                 .backgroundColor(is_hovered ? 0xFF475569 : box_color)
                 .borderRadius(24.0f);
                 
        inner_box->child(Text(status, {.font_size = 18.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        GestureDetectorConfig gd_cfg;
        gd_cfg.child = inner_box;
        
        gd_cfg.on_hover_enter = [this]() {
            setState([this]() { is_hovered = true; status = "Hovering!"; });
        };
        gd_cfg.on_hover_exit = [this]() {
            setState([this]() { is_hovered = false; status = "Waiting for gesture..."; box_color = 0xFF334155; });
        };
        gd_cfg.on_tap_down = [this]() {
            setState([this]() { box_color = 0xFF0EA5E9; status = "Pointer Down!"; });
        };
        gd_cfg.on_tap_up = [this]() {
            setState([this]() { box_color = 0xFF10B981; status = "Pointer Up!"; });
        };
        gd_cfg.on_tap = [this]() {
            setState([this]() { status = "Tapped!"; });
        };
        gd_cfg.on_right_click = [this](Point p) {
            setState([this]() { box_color = 0xFFEF4444; status = "Right Clicked!"; });
        };
        
        auto wrapper = std::make_shared<FlexBox>();
        wrapper->child(GestureDetector(gd_cfg));
        
        root->child(wrapper);
        return root;
    }
};

class GestureDetectorDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "GestureDetectorDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<GestureDetectorDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Gesture Detector Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<GestureDetectorDemo>(), config);
}
