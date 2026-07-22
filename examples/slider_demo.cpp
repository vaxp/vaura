#include "vaura/app/app.hpp"
#include "vaura/widgets/slider.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/state/state.hpp"
#include <string>

using namespace vaura;

class SliderDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "SliderDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class SliderDemoState : public State {
public:
    float current_value = 50.0f;

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF1E293B)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter);

        auto content_col = std::make_shared<FlexBox>();
        content_col->flexDirection(YGFlexDirectionColumn)
                   .alignItems(YGAlignCenter);

        auto title = text("Brightness: " + std::to_string(static_cast<int>(current_value)) + "%", {
            .font_size = 22,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        });
        
        auto title_c = std::make_shared<FlexBox>();
        title_c->margin(YGEdgeBottom, 20).child(title);

        auto s = slider({
            .value = current_value,
            .min_value = 0.0f,
            .max_value = 100.0f,
            .thumb_radius = 10.0f,
            .on_changed = [this](float val) {
                setState([this, val]() {
                    current_value = val;
                });
            },
            .width = 300.0f,
            .active_color = 0xFF06B6D4, // Cyan
            .inactive_color = 0xFF475569, // Slate 600
            .thumb_color = 0xFFFFFFFF,
            .track_height = 6.0f
        });

        content_col->child(title_c);
        content_col->child(s);

        auto c = card({
            .color = 0xFF0F172A,
            .border_radius = 20.0f,
            .elevation = 15.0f,
            .shadow_color = 0x80000000,
            .padding = {40, 40, 40, 40}
        }, content_col);

        root->child(c);
        return root;
    }
};

std::unique_ptr<State> SliderDemoApp::createState() {
    return std::make_unique<SliderDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Slider Demo",
        .width = 600,
        .height = 400,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<SliderDemoApp>(), config);
}
