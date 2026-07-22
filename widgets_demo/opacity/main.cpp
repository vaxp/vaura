// widgets_demo/opacity/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/slider.hpp"
#include "vaura/widgets/opacity.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class OpacityDemoState : public State {
    float current_opacity = 1.0f;

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
             .child(text("Opacity Control", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));
        root->child(title);

        // Control Panel
        SliderConfig s_cfg;
        s_cfg.min_value = 0.0f;
        s_cfg.max_value = 1.0f;
        s_cfg.value = current_opacity;
        s_cfg.active_color = 0xFF10B981;
        s_cfg.width = 300.0f;
        s_cfg.on_changed = [this](float v) {
            setState([this, v]() { current_opacity = v; });
        };
        
        auto slider_box = std::make_shared<FlexBox>();
        slider_box->margin(YGEdgeBottom, 40).child(slider(s_cfg));
        root->child(slider_box);

        // Target content inside opacity widget
        auto content_col = std::make_shared<FlexBox>();
        content_col->flexDirection(YGFlexDirectionColumn)
                   .alignItems(YGAlignCenter);
                   
        auto box = std::make_shared<FlexBox>();
        box->width(200).height(200)
           .backgroundColor(0xFF3B82F6) // Blue
           .borderRadius(20.0f)
           .justifyContent(YGJustifyCenter)
           .alignItems(YGAlignCenter);
           
        box->child(text("Fading Box", {.font_size = 20.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));
        content_col->child(box);

        auto c = card({
            .color = 0xFF1E293B,
            .border_radius = 20.0f,
            .elevation = 10.0f,
            .padding = {20, 20, 20, 20}
        }, content_col);

        // Apply Opacity
        auto opacity_widget = opacity(current_opacity, c);
        
        root->child(opacity_widget);
        return root;
    }
};

class OpacityDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "OpacityDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<OpacityDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Opacity Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<OpacityDemo>(), config);
}
