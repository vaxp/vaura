#include "vaura/app/app.hpp"
#include "vaura/state/state.hpp"
#include "vaura/widgets/slider.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/rendering/color.hpp"
#include <iostream>

using namespace vaura;

class SliderDemo : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "SliderDemo"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class SliderDemoState : public State {
    float brightness = 0.5f;

public:
    WidgetPtr build(BuildContext& context) override {
        // Dynamically change background brightness based on slider
        uint8_t luma = static_cast<uint8_t>(brightness * 255);
        Color bg_color = (0xFF << 24) | (luma << 16) | (luma << 8) | luma;

        auto col = std::make_shared<FlexBox>();
        col->flexDirection(YGFlexDirectionColumn)
           .justifyContent(YGJustifyCenter)
           .alignItems(YGAlignCenter)
           .gap(YGGutterAll, 40.0f)
           .widthPercent(100.0f)
           .heightPercent(100.0f)
           .backgroundColor(bg_color);

        // Title
        TextConfig title_cfg;
        title_cfg.font_size = 32.0f;
        // Inverse color for text
        title_cfg.color = luma > 128 ? 0xFF000000 : 0xFFFFFFFF; 
        col->child(std::make_shared<TextWidget>("Brightness Control", title_cfg));

        // Slider
        SliderConfig slider_cfg;
        slider_cfg.value = brightness;
        slider_cfg.min_value = 0.0f;
        slider_cfg.max_value = 1.0f;
        slider_cfg.width = 300.0f;
        slider_cfg.active_color = 0xFFF59E0B; // Amber 500
        slider_cfg.inactive_color = 0xFF94A3B8; // Slate 400
        slider_cfg.thumb_color = 0xFFFFFFFF;
        slider_cfg.on_changed = [this](float val) {
            setState([this, val]() {
                brightness = val;
            });
        };
        col->child(Slider(slider_cfg));

        // Value text
        TextConfig val_cfg;
        val_cfg.font_size = 24.0f;
        val_cfg.color = luma > 128 ? 0xFF334155 : 0xFFCBD5E1; 
        col->child(std::make_shared<TextWidget>(std::to_string(static_cast<int>(brightness * 100)) + "%", val_cfg));

        return col;
    }
};

std::unique_ptr<State> SliderDemo::createState() {
    return std::make_unique<SliderDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Slider Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<SliderDemo>(), config);
}
